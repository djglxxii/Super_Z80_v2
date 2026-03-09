#include "emulator_core.h"

#include <array>
#include <iostream>
#include <string>

#if defined(SUPER_Z80_HAS_SDL)
#include <SDL.h>
#endif

namespace {

void print_header() {
    std::cout << "Super_Z80_v2 Emulator" << std::endl;
}

void print_help() {
    print_header();
    std::cout << "Usage: super_z80 [--help] [--version] [--sdl-input] [--sdl-audio]" << std::endl;
    std::cout << "  --help       Show this help message" << std::endl;
    std::cout << "  --version    Show version information" << std::endl;
    std::cout << "  --sdl-input  Run the SDL controller input shell" << std::endl;
    std::cout << "  --sdl-audio  Run the SDL audio playback shell" << std::endl;
}

void print_version() {
    print_header();
    std::cout << "Version: dev" << std::endl;
}

#if defined(SUPER_Z80_HAS_SDL)
constexpr std::size_t kSdlQueueTargetSamples = 2048U;
constexpr std::size_t kSdlQueueChunkSamples = 512U;

void program_demo_tone(EmulatorCore& core) {
    constexpr uint16_t kToneAPeriod = 254U;

    core.bus().write_port(superz80::Bus::kAudioToneALowPort,
                          static_cast<uint8_t>(kToneAPeriod & 0x00FFU));
    core.bus().write_port(superz80::Bus::kAudioToneAHighPort,
                          static_cast<uint8_t>((kToneAPeriod >> 8U) & 0x0FU));
    core.bus().write_port(superz80::Bus::kAudioVolumeAPort, 0x00U);
    core.bus().write_port(superz80::Bus::kAudioControlPort, 0x01U);
}

bool pump_audio_output(EmulatorCore& core, SDL_AudioDeviceID device) {
    std::array<EmulatorCore::AudioSample, kSdlQueueChunkSamples> chunk = {};

    while ((SDL_GetQueuedAudioSize(device) / sizeof(EmulatorCore::AudioSample)) < kSdlQueueTargetSamples) {
        if (core.available_audio_samples() == 0U) {
            core.step_scanline();
            continue;
        }

        const std::size_t queued_samples =
            SDL_GetQueuedAudioSize(device) / sizeof(EmulatorCore::AudioSample);
        const std::size_t available_capacity = kSdlQueueTargetSamples - queued_samples;
        const std::size_t requested_samples =
            (available_capacity < chunk.size()) ? available_capacity : chunk.size();
        const std::size_t copied_samples = core.consume_audio_samples(chunk.data(), requested_samples);
        if (copied_samples == 0U) {
            break;
        }

        if (SDL_QueueAudio(device, chunk.data(),
                           static_cast<Uint32>(copied_samples * sizeof(EmulatorCore::AudioSample))) != 0) {
            std::cerr << "SDL audio queue failed: " << SDL_GetError() << std::endl;
            return false;
        }
    }

    return true;
}

bool map_keyboard_key(superz80::Bus& bus, const SDL_KeyboardEvent& event) {
    const bool pressed = event.type == SDL_KEYDOWN;
    switch (event.keysym.sym) {
    case SDLK_UP:
        bus.set_controller_button(superz80::IO::Button::Up, pressed);
        return true;
    case SDLK_DOWN:
        bus.set_controller_button(superz80::IO::Button::Down, pressed);
        return true;
    case SDLK_LEFT:
        bus.set_controller_button(superz80::IO::Button::Left, pressed);
        return true;
    case SDLK_RIGHT:
        bus.set_controller_button(superz80::IO::Button::Right, pressed);
        return true;
    case SDLK_z:
        bus.set_controller_button(superz80::IO::Button::A, pressed);
        return true;
    case SDLK_x:
        bus.set_controller_button(superz80::IO::Button::B, pressed);
        return true;
    case SDLK_RETURN:
        bus.set_controller_button(superz80::IO::Button::Start, pressed);
        return true;
    default:
        return false;
    }
}

bool map_gamepad_button(superz80::Bus& bus, SDL_GameControllerButton button, bool pressed) {
    switch (button) {
    case SDL_CONTROLLER_BUTTON_DPAD_UP:
        bus.set_controller_button(superz80::IO::Button::Up, pressed);
        return true;
    case SDL_CONTROLLER_BUTTON_DPAD_DOWN:
        bus.set_controller_button(superz80::IO::Button::Down, pressed);
        return true;
    case SDL_CONTROLLER_BUTTON_DPAD_LEFT:
        bus.set_controller_button(superz80::IO::Button::Left, pressed);
        return true;
    case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:
        bus.set_controller_button(superz80::IO::Button::Right, pressed);
        return true;
    case SDL_CONTROLLER_BUTTON_A:
        bus.set_controller_button(superz80::IO::Button::A, pressed);
        return true;
    case SDL_CONTROLLER_BUTTON_B:
        bus.set_controller_button(superz80::IO::Button::B, pressed);
        return true;
    case SDL_CONTROLLER_BUTTON_START:
        bus.set_controller_button(superz80::IO::Button::Start, pressed);
        return true;
    default:
        return false;
    }
}

void map_gamepad_axis(superz80::Bus& bus, SDL_GameControllerAxis axis, Sint16 value) {
    constexpr Sint16 kThreshold = 16000;
    if (axis == SDL_CONTROLLER_AXIS_LEFTX) {
        bus.set_controller_button(superz80::IO::Button::Left, value <= -kThreshold);
        bus.set_controller_button(superz80::IO::Button::Right, value >= kThreshold);
        if (value > -kThreshold && value < kThreshold) {
            bus.set_controller_button(superz80::IO::Button::Left, false);
            bus.set_controller_button(superz80::IO::Button::Right, false);
        }
    }

    if (axis == SDL_CONTROLLER_AXIS_LEFTY) {
        bus.set_controller_button(superz80::IO::Button::Up, value <= -kThreshold);
        bus.set_controller_button(superz80::IO::Button::Down, value >= kThreshold);
        if (value > -kThreshold && value < kThreshold) {
            bus.set_controller_button(superz80::IO::Button::Up, false);
            bus.set_controller_button(superz80::IO::Button::Down, false);
        }
    }
}

void print_controller_state(superz80::Bus& bus) {
    std::cout << "PAD1=0x" << std::hex << std::uppercase
              << static_cast<unsigned int>(bus.read_port(superz80::Bus::kPad1Port))
              << " PAD1_SYS=0x"
              << static_cast<unsigned int>(bus.read_port(superz80::Bus::kPad1SysPort))
              << std::dec << std::nouppercase << std::endl;
}

int run_sdl_audio_shell(EmulatorCore& core) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO) != 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Super_Z80_v2 Audio Shell",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        640,
        240,
        SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "SDL window creation failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_AudioSpec desired = {};
    desired.freq = static_cast<int>(superz80::APU::kSampleRateHz);
    desired.format = AUDIO_S16LSB;
    desired.channels = 1;
    desired.samples = 1024U;

    SDL_AudioSpec obtained = {};
    const SDL_AudioDeviceID device =
        SDL_OpenAudioDevice(nullptr, 0, &desired, &obtained, 0);
    if (device == 0U) {
        std::cerr << "SDL audio device open failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    if (obtained.freq != desired.freq || obtained.format != desired.format || obtained.channels != desired.channels) {
        std::cerr << "SDL audio device does not support required mono 48 kHz signed-16 little-endian output."
                  << std::endl;
        SDL_CloseAudioDevice(device);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    program_demo_tone(core);
    if (!pump_audio_output(core, device)) {
        SDL_CloseAudioDevice(device);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    SDL_PauseAudioDevice(device, 0);

    std::cout << "SDL audio shell active. Tone A demo is routed through SDL audio. "
                 "Close the window or press Escape to quit."
              << std::endl;

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event) == 1) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN && event.key.repeat == 0 &&
                       event.key.keysym.sym == SDLK_ESCAPE) {
                running = false;
            }
        }

        if (!pump_audio_output(core, device)) {
            running = false;
            break;
        }

        SDL_Delay(1);
    }

    SDL_CloseAudioDevice(device);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

int run_sdl_input_shell(EmulatorCore& core) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) != 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow(
        "Super_Z80_v2 Input Shell",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        640,
        480,
        SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "SDL window creation failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_GameController* controller = nullptr;
    for (int index = 0; index < SDL_NumJoysticks(); ++index) {
        if (SDL_IsGameController(index) == SDL_TRUE) {
            controller = SDL_GameControllerOpen(index);
            if (controller != nullptr) {
                break;
            }
        }
    }

    std::cout << "SDL input shell active. Arrow keys, Z, X, Enter mapped to PAD1/PAD1_SYS. Close the window or press Escape to quit." << std::endl;
    print_controller_state(core.bus());

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event) == 1) {
            bool state_changed = false;

            switch (event.type) {
            case SDL_QUIT:
                running = false;
                break;
            case SDL_KEYDOWN:
            case SDL_KEYUP:
                if (event.key.keysym.sym == SDLK_ESCAPE && event.type == SDL_KEYDOWN) {
                    running = false;
                    break;
                }
                if (event.key.repeat == 0) {
                    state_changed = map_keyboard_key(core.bus(), event.key);
                }
                break;
            case SDL_CONTROLLERBUTTONDOWN:
            case SDL_CONTROLLERBUTTONUP:
                state_changed = map_gamepad_button(
                    core.bus(),
                    static_cast<SDL_GameControllerButton>(event.cbutton.button),
                    event.type == SDL_CONTROLLERBUTTONDOWN);
                break;
            case SDL_CONTROLLERAXISMOTION:
                map_gamepad_axis(
                    core.bus(),
                    static_cast<SDL_GameControllerAxis>(event.caxis.axis),
                    event.caxis.value);
                state_changed = true;
                break;
            default:
                break;
            }

            if (state_changed) {
                print_controller_state(core.bus());
            }
        }

        SDL_Delay(16);
    }

    if (controller != nullptr) {
        SDL_GameControllerClose(controller);
    }
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}
#endif

} // namespace

int main(int argc, char* argv[]) {
    EmulatorCore core;
    core.initialize();

    if (argc <= 1) {
        print_help();
        return 0;
    }

    const std::string arg = argv[1];
    if (arg == "--help") {
        print_help();
        return 0;
    }

    if (arg == "--version") {
        print_version();
        return 0;
    }

    if (arg == "--sdl-input") {
#if defined(SUPER_Z80_HAS_SDL)
        return run_sdl_input_shell(core);
#else
        std::cerr << "SDL input shell unavailable: SDL2 was not found at build time." << std::endl;
        return 1;
#endif
    }

    if (arg == "--sdl-audio") {
#if defined(SUPER_Z80_HAS_SDL)
        return run_sdl_audio_shell(core);
#else
        std::cerr << "SDL audio shell unavailable: SDL2 was not found at build time." << std::endl;
        return 1;
#endif
    }

    std::cerr << "Unknown argument: " << arg << std::endl;
    print_help();
    return 1;
}
