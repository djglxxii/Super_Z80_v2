#include "emulator_core.h"
#include "frontend.h"

#include <array>
#include <cstdint>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

#include "crc32.h"

#if defined(SUPER_Z80_HAS_SDL)
#include <SDL.h>
#include "sdl_audio_output.h"
#endif

namespace {

struct Options {
    bool show_help = false;
    bool show_version = false;
    bool use_sdl_input = false;
    bool use_sdl_audio = false;
    bool headless = false;
    std::string rom_path;
    uint32_t frames = 1U;
};

void print_header() {
    std::cout << "Super_Z80_v2 Emulator" << std::endl;
}

void print_help() {
    print_header();
    std::cout << "Usage: super_z80 [--help] [--version] [--sdl-input] [--sdl-audio] [--rom <path>] [--headless] [--frames <count>]" << std::endl;
    std::cout << "  --help       Show this help message" << std::endl;
    std::cout << "  --version    Show version information" << std::endl;
    std::cout << "  --sdl-input  Run the SDL controller input shell" << std::endl;
    std::cout << "  --sdl-audio  Run the SDL audio playback shell" << std::endl;
    std::cout << "  --rom        Load an external ROM from disk" << std::endl;
    std::cout << "  --headless   Run the loaded ROM without SDL interaction" << std::endl;
    std::cout << "  --frames     Number of frames to execute in headless mode (default: 1)" << std::endl;
}

void print_version() {
    print_header();
    std::cout << "Version: dev" << std::endl;
}

bool parse_u32(const std::string& text, uint32_t& value) {
    try {
        std::size_t parsed = 0U;
        const unsigned long number = std::stoul(text, &parsed, 10);
        if (parsed != text.size() || number == 0UL) {
            return false;
        }

        value = static_cast<uint32_t>(number);
        return static_cast<unsigned long>(value) == number;
    } catch (...) {
        return false;
    }
}

bool parse_arguments(int argc, char* argv[], Options& options, std::string& error_message) {
    for (int index = 1; index < argc; ++index) {
        const std::string arg = argv[index];

        if (arg == "--help") {
            options.show_help = true;
            continue;
        }

        if (arg == "--version") {
            options.show_version = true;
            continue;
        }

        if (arg == "--sdl-input") {
            options.use_sdl_input = true;
            continue;
        }

        if (arg == "--sdl-audio") {
            options.use_sdl_audio = true;
            continue;
        }

        if (arg == "--headless") {
            options.headless = true;
            continue;
        }

        if (arg == "--rom") {
            if (index + 1 >= argc) {
                error_message = "Missing value for --rom.";
                return false;
            }

            options.rom_path = argv[++index];
            continue;
        }

        if (arg == "--frames") {
            if (index + 1 >= argc) {
                error_message = "Missing value for --frames.";
                return false;
            }

            if (!parse_u32(argv[++index], options.frames)) {
                error_message = "Invalid frame count for --frames: must be a positive integer.";
                return false;
            }
            continue;
        }

        error_message = "Unknown argument: " + arg;
        return false;
    }

    if (!options.rom_path.empty() &&
        !options.headless &&
        !options.use_sdl_input &&
        !options.use_sdl_audio) {
        options.headless = true;
    }

    if (options.headless && options.rom_path.empty()) {
        error_message = "Headless execution requires --rom <path>.";
        return false;
    }

    if (!options.rom_path.empty() && (options.use_sdl_input || options.use_sdl_audio)) {
        error_message = "External ROM execution currently supports headless mode only.";
        return false;
    }

    return true;
}

bool load_rom_file(const std::string& path, std::vector<uint8_t>& bytes, std::string& error_message) {
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        error_message = "Unable to open ROM file: " + path;
        return false;
    }

    file.seekg(0, std::ios::end);
    const std::streamoff length = file.tellg();
    if (length < 0) {
        error_message = "Unable to determine ROM file size: " + path;
        return false;
    }

    file.seekg(0, std::ios::beg);
    bytes.assign(static_cast<std::size_t>(length), 0U);
    if (!bytes.empty()) {
        file.read(reinterpret_cast<char*>(bytes.data()), length);
        if (!file) {
            error_message = "Unable to read ROM file: " + path;
            return false;
        }
    }

    return true;
}

std::vector<uint8_t> collect_ram_image(const EmulatorCore& core) {
    std::vector<uint8_t> ram(superz80::Bus::kRamWindowSize, 0U);
    for (std::size_t offset = 0U; offset < ram.size(); ++offset) {
        const uint16_t address = static_cast<uint16_t>(superz80::Bus::kRamStart + offset);
        ram[offset] = core.bus().read(address);
    }
    return ram;
}

std::vector<EmulatorCore::AudioSample> collect_audio_samples(EmulatorCore& core) {
    std::vector<EmulatorCore::AudioSample> samples(core.available_audio_samples(), 0);
    if (!samples.empty()) {
        core.consume_audio_samples(samples.data(), samples.size());
    }
    return samples;
}

int run_headless_rom(EmulatorCore& core, const std::string& rom_path, uint32_t frames) {
    std::vector<uint8_t> rom_bytes;
    std::string error_message;
    if (!load_rom_file(rom_path, rom_bytes, error_message)) {
        std::cerr << error_message << std::endl;
        return 1;
    }

    core.load_rom(rom_bytes.data(), rom_bytes.size());
    core.step_scanlines(frames * superz80::Scheduler::kScanlinesPerFrame);

    const std::vector<uint8_t> ram_image = collect_ram_image(core);
    const std::vector<EmulatorCore::AudioSample> audio_samples = collect_audio_samples(core);
    const uint32_t ram_crc = superz80::crc32(ram_image.data(), ram_image.size());
    const uint32_t audio_crc = superz80::crc32(audio_samples.data(),
                                               audio_samples.size() * sizeof(EmulatorCore::AudioSample));

    std::ostringstream summary;
    summary << "HEADLESS_ROM_RESULT"
            << " rom_crc32=0x" << std::hex << std::uppercase << superz80::crc32(rom_bytes.data(), rom_bytes.size())
            << " ram_crc32=0x" << ram_crc
            << " audio_crc32=0x" << audio_crc
            << std::dec
            << " frames=" << frames
            << " frame_counter=" << core.frame()
            << " scanline_counter=" << core.scanline()
            << " audio_samples=" << audio_samples.size();
    std::cout << summary.str() << std::endl;
    return 0;
}

#if defined(SUPER_Z80_HAS_SDL)
constexpr std::size_t kSdlOutputChunkSamples = 512U;

void program_demo_tone(EmulatorCore& core) {
    constexpr uint16_t kToneAPeriod = 254U;

    core.bus().write_port(superz80::Bus::kAudioToneALowPort,
                          static_cast<uint8_t>(kToneAPeriod & 0x00FFU));
    core.bus().write_port(superz80::Bus::kAudioToneAHighPort,
                          static_cast<uint8_t>((kToneAPeriod >> 8U) & 0x0FU));
    core.bus().write_port(superz80::Bus::kAudioVolumeAPort, 0x00U);
    core.bus().write_port(superz80::Bus::kAudioControlPort, 0x01U);
}

void pump_audio_output(EmulatorCore& core, superz80::SdlAudioOutput& output) {
    std::array<EmulatorCore::AudioSample, kSdlOutputChunkSamples> chunk = {};

    while (output.buffered_samples() < superz80::SdlAudioOutput::kDefaultBufferCapacitySamples / 2U) {
        if (core.available_audio_samples() == 0U) {
            core.step_scanline();
            continue;
        }

        const std::size_t copied_samples = core.consume_audio_samples(chunk.data(), chunk.size());
        if (copied_samples == 0U) {
            break;
        }

        output.enqueue_samples(chunk.data(), copied_samples);
    }
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
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
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

    superz80::frontend::Frontend frontend;
    if (!frontend.initialize({"sdl-audio-shell"})) {
        std::cerr << "Frontend initialization failed for SDL audio shell." << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    superz80::SdlAudioOutput audio_output;
    if (!audio_output.initialize()) {
        std::cerr << "SDL audio device open failed: " << SDL_GetError() << std::endl;
        frontend.shutdown();
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    program_demo_tone(core);
    pump_audio_output(core, audio_output);
    audio_output.start();

    std::cout << "SDL audio shell active. Tone A demo is routed through SDL audio. "
                 "Close the window or press Escape to quit."
              << std::endl;

    bool running = true;
    while (running) {
        frontend.begin_frame();

        SDL_Event event;
        while (SDL_PollEvent(&event) == 1) {
            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN && event.key.repeat == 0 &&
                       event.key.keysym.sym == SDLK_ESCAPE) {
                running = false;
            }
        }

        pump_audio_output(core, audio_output);
        frontend.end_frame();

        SDL_Delay(1);
    }

    audio_output.shutdown();
    frontend.shutdown();
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

    superz80::frontend::Frontend frontend;
    if (!frontend.initialize({"sdl-input-shell"})) {
        std::cerr << "Frontend initialization failed for SDL input shell." << std::endl;
        SDL_DestroyWindow(window);
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
        frontend.begin_frame();

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

        frontend.end_frame();

        SDL_Delay(16);
    }

    if (controller != nullptr) {
        SDL_GameControllerClose(controller);
    }
    frontend.shutdown();
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

    Options options;
    std::string error_message;
    if (!parse_arguments(argc, argv, options, error_message)) {
        std::cerr << error_message << std::endl;
        print_help();
        return 1;
    }

    if (options.show_help) {
        print_help();
        return 0;
    }

    if (options.show_version) {
        print_version();
        return 0;
    }

    if (!options.rom_path.empty() && options.headless) {
        return run_headless_rom(core, options.rom_path, options.frames);
    }

    if (options.use_sdl_input) {
#if defined(SUPER_Z80_HAS_SDL)
        return run_sdl_input_shell(core);
#else
        std::cerr << "SDL input shell unavailable: SDL2 was not found at build time." << std::endl;
        return 1;
#endif
    }

    if (options.use_sdl_audio) {
#if defined(SUPER_Z80_HAS_SDL)
        return run_sdl_audio_shell(core);
#else
        std::cerr << "SDL audio shell unavailable: SDL2 was not found at build time." << std::endl;
        return 1;
#endif
    }

    print_help();
    return 0;
}
