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
#include <optional>

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

struct RomLoadResult {
    bool ok = false;
    std::string message;
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

struct RuntimeLoopState {
    bool emulation_running = true;
    bool rom_load_status_ok = false;
    bool snapshot_available = false;
    bool snapshot_status_ok = false;
    std::string current_rom_path;
    std::string rom_load_status_message;
    std::string snapshot_status_message;
};

RomLoadResult load_runtime_rom(EmulatorCore& core, const std::string& rom_path) {
    if (rom_path.empty()) {
        return {false, "ROM path is empty."};
    }

    std::vector<uint8_t> rom_bytes;
    std::string error_message;
    if (!load_rom_file(rom_path, rom_bytes, error_message)) {
        return {false, error_message};
    }

    core.initialize();
    core.load_rom(rom_bytes.data(), rom_bytes.size());
    return {true, "Loaded ROM: " + rom_path};
}

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

void step_one_emulator_frame(EmulatorCore& core) {
    core.step_scanlines(superz80::Scheduler::kScanlinesPerFrame);
}

void populate_frontend_runtime_state(superz80::frontend::Frontend& frontend,
                                     const RuntimeLoopState& loop_state,
                                     const EmulatorCore& core) {
    const EmulatorCore::CpuSnapshot cpu_snapshot = core.cpu_snapshot();
    const EmulatorCore::DmaSnapshot dma_snapshot = core.dma_snapshot();
    const EmulatorCore::InputSnapshot input_snapshot = core.input_snapshot();
    const EmulatorCore::AudioSnapshot audio_snapshot = core.audio_snapshot();
    const EmulatorCore::TimingSnapshot timing_snapshot = core.timing_snapshot();
    const EmulatorCore::RomSnapshot rom_snapshot = core.rom_snapshot();
    const EmulatorCore::RamSnapshot ram_snapshot = core.ram_snapshot();
    const EmulatorCore::VramSnapshot vram_snapshot = core.vram_snapshot();
    const EmulatorCore::SpriteTableSnapshot sprite_table_snapshot = core.sprite_table_snapshot();
    superz80::frontend::RuntimeControlState runtime_state = {
        loop_state.emulation_running,
        static_cast<unsigned int>(core.frame()),
        !loop_state.current_rom_path.empty(),
        loop_state.rom_load_status_ok,
        loop_state.snapshot_available,
        loop_state.snapshot_status_ok,
        loop_state.current_rom_path,
        loop_state.rom_load_status_message,
        loop_state.snapshot_status_message,
        {
            true,
            cpu_snapshot.af,
            cpu_snapshot.bc,
            cpu_snapshot.de,
            cpu_snapshot.hl,
            cpu_snapshot.ix,
            cpu_snapshot.iy,
            cpu_snapshot.pc,
            cpu_snapshot.sp,
            cpu_snapshot.i,
            cpu_snapshot.r,
            cpu_snapshot.interrupt_mode,
            cpu_snapshot.iff1,
            cpu_snapshot.iff2,
            cpu_snapshot.halted,
            cpu_snapshot.int_line,
        },
        {
            true,
            rom_snapshot,
            ram_snapshot,
        },
        {
            true,
            vram_snapshot,
        },
        {
            true,
            {},
        },
        {
            true,
            dma_snapshot.source_address,
            dma_snapshot.destination_address,
            dma_snapshot.transfer_length,
            dma_snapshot.control,
            dma_snapshot.active,
        },
        {
            true,
            input_snapshot.up,
            input_snapshot.down,
            input_snapshot.left,
            input_snapshot.right,
            input_snapshot.a,
            input_snapshot.b,
            input_snapshot.start,
            input_snapshot.pad1,
            input_snapshot.pad1_sys,
        },
        {
            true,
            {
                true,
                audio_snapshot.apu.registers,
                {},
                {
                    audio_snapshot.apu.noise.control,
                    audio_snapshot.apu.noise.volume,
                    audio_snapshot.apu.noise.divider_counter,
                    audio_snapshot.apu.noise.lfsr,
                    audio_snapshot.apu.noise.output_bit,
                },
                audio_snapshot.apu.control,
                audio_snapshot.apu.enabled,
                audio_snapshot.apu.muted,
                audio_snapshot.apu.overrun,
                audio_snapshot.apu.current_sample,
            },
            {
                true,
                audio_snapshot.ym2151.selected_register,
                audio_snapshot.ym2151.registers,
                {},
                {
                    audio_snapshot.ym2151.timer_a.latch,
                    audio_snapshot.ym2151.timer_a.counter,
                    audio_snapshot.ym2151.timer_a.enabled,
                    audio_snapshot.ym2151.timer_a.overflow,
                    audio_snapshot.ym2151.timer_a.irq_enabled,
                },
                {
                    audio_snapshot.ym2151.timer_b.latch,
                    audio_snapshot.ym2151.timer_b.counter,
                    audio_snapshot.ym2151.timer_b.enabled,
                    audio_snapshot.ym2151.timer_b.overflow,
                    audio_snapshot.ym2151.timer_b.irq_enabled,
                },
                audio_snapshot.ym2151.status,
                audio_snapshot.ym2151.irq_pending,
                audio_snapshot.ym2151.current_sample,
                audio_snapshot.ym2151.tick_call_count,
                audio_snapshot.ym2151.accumulated_cycles,
            },
        },
        {
            true,
            timing_snapshot.frame_counter,
            timing_snapshot.scanline_counter,
            timing_snapshot.scanlines_per_frame,
            timing_snapshot.instructions_per_scanline,
            timing_snapshot.ym2151_cycles_per_scanline,
            timing_snapshot.vblank_active,
            timing_snapshot.frame_ready,
            timing_snapshot.buffered_audio_samples,
        },
    };

    for (std::size_t sprite_index = 0U; sprite_index < sprite_table_snapshot.size(); ++sprite_index) {
        runtime_state.sprite_debug_state.sprites[sprite_index] = {
            sprite_table_snapshot[sprite_index].x,
            sprite_table_snapshot[sprite_index].y,
            sprite_table_snapshot[sprite_index].tile_index,
            sprite_table_snapshot[sprite_index].attributes,
        };
    }

    for (std::size_t channel = 0U; channel < audio_snapshot.apu.tone_channels.size(); ++channel) {
        runtime_state.audio_debug_state.psg.tone_channels[channel] = {
            audio_snapshot.apu.tone_channels[channel].period,
            audio_snapshot.apu.tone_channels[channel].volume,
            audio_snapshot.apu.tone_channels[channel].divider_counter,
            audio_snapshot.apu.tone_channels[channel].phase_high,
        };
    }

    for (std::size_t channel = 0U; channel < audio_snapshot.ym2151.channels.size(); ++channel) {
        runtime_state.audio_debug_state.ym2151.channels[channel] = {
            audio_snapshot.ym2151.channels[channel].frequency,
            audio_snapshot.ym2151.channels[channel].block,
            audio_snapshot.ym2151.channels[channel].algorithm,
            audio_snapshot.ym2151.channels[channel].feedback,
            audio_snapshot.ym2151.channels[channel].key_on_mask,
        };
    }

    frontend.set_runtime_control_state(runtime_state);
}

void render_frontend_frame(superz80::frontend::Frontend& frontend,
                           SDL_Renderer* renderer,
                           const EmulatorCore& core) {
    const EmulatorCore::FramebufferSnapshot framebuffer = core.framebuffer_snapshot();
    frontend.update_framebuffer(framebuffer.data(), framebuffer.size());
    SDL_SetRenderDrawColor(renderer, 12, 16, 20, 255);
    SDL_RenderClear(renderer);
    frontend.render_framebuffer();
    frontend.render_overlay();
    SDL_RenderPresent(renderer);
}

template <typename ResetFn, typename LoadRomFn, typename SaveSnapshotFn, typename RestoreSnapshotFn>
void apply_runtime_control_commands(superz80::frontend::Frontend& frontend,
                                    RuntimeLoopState& loop_state,
                                    EmulatorCore& core,
                                    ResetFn&& reset_runtime,
                                    LoadRomFn&& load_rom,
                                    SaveSnapshotFn&& save_snapshot,
                                    RestoreSnapshotFn&& restore_snapshot) {
    const superz80::frontend::RuntimeControlCommands commands =
        frontend.consume_runtime_control_commands();

    if (commands.toggle_run_pause) {
        loop_state.emulation_running = !loop_state.emulation_running;
    }

    if (commands.reset_requested) {
        reset_runtime();
    }

    if (commands.load_rom_requested) {
        load_rom(commands.rom_path_to_load);
    }

    if (commands.reload_rom_requested) {
        if (loop_state.current_rom_path.empty()) {
            loop_state.rom_load_status_ok = false;
            loop_state.rom_load_status_message = "Reload failed: no ROM path is active.";
            std::cerr << loop_state.rom_load_status_message << std::endl;
        } else {
            load_rom(loop_state.current_rom_path);
        }
    }

    if (commands.save_snapshot_requested) {
        save_snapshot();
    }

    if (commands.restore_snapshot_requested) {
        restore_snapshot();
    }

    if (commands.step_frame_requested && !loop_state.emulation_running) {
        step_one_emulator_frame(core);
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

SDL_Renderer* create_window_renderer(SDL_Window* window) {
    SDL_Renderer* renderer = SDL_CreateRenderer(
        window,
        -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (renderer != nullptr) {
        return renderer;
    }

    return SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
}

int run_sdl_audio_shell(EmulatorCore& core, const std::string& startup_rom_path) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    const unsigned int initial_display_scale = superz80::frontend::load_persisted_display_scale();
    const superz80::frontend::DisplayWindowSize initial_window_size =
        superz80::frontend::window_size_for_display_scale(initial_display_scale);
    SDL_Window* window = SDL_CreateWindow(
        "Super_Z80_v2 Audio Shell",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        initial_window_size.width,
        initial_window_size.height,
        SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "SDL window creation failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = create_window_renderer(window);
    if (renderer == nullptr) {
        std::cerr << "SDL renderer creation failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    superz80::frontend::Frontend frontend;
    if (!frontend.initialize({"sdl-audio-shell", initial_display_scale, window, renderer})) {
        std::cerr << "Frontend initialization failed for SDL audio shell." << std::endl;
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    superz80::SdlAudioOutput audio_output;
    if (!audio_output.initialize()) {
        std::cerr << "SDL audio device open failed: " << SDL_GetError() << std::endl;
        frontend.shutdown();
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    RuntimeLoopState loop_state = {};
    std::optional<EmulatorCore::Snapshot> runtime_snapshot;
    const auto reset_runtime = [&core, &audio_output, &loop_state, &runtime_snapshot]() {
        audio_output.clear();
        runtime_snapshot.reset();
        if (!loop_state.current_rom_path.empty()) {
            const RomLoadResult result = load_runtime_rom(core, loop_state.current_rom_path);
            loop_state.rom_load_status_ok = result.ok;
            loop_state.rom_load_status_message = result.message;
            if (!result.ok) {
                std::cerr << result.message << std::endl;
                return;
            }
            std::cout << result.message << std::endl;
        } else {
            core.initialize();
            program_demo_tone(core);
            loop_state.rom_load_status_ok = true;
            loop_state.rom_load_status_message = "Reset runtime to demo audio startup.";
        }
        loop_state.snapshot_available = false;
        loop_state.snapshot_status_ok = true;
        loop_state.snapshot_status_message = "Snapshot cleared after reset.";
        pump_audio_output(core, audio_output);
    };
    const auto load_rom_into_runtime = [&core, &audio_output, &loop_state, &runtime_snapshot](const std::string& rom_path) {
        audio_output.clear();
        runtime_snapshot.reset();
        const RomLoadResult result = load_runtime_rom(core, rom_path);
        loop_state.rom_load_status_ok = result.ok;
        loop_state.rom_load_status_message = result.message;
        if (!result.ok) {
            std::cerr << result.message << std::endl;
            return;
        }

        loop_state.current_rom_path = rom_path;
        loop_state.emulation_running = true;
        loop_state.snapshot_available = false;
        loop_state.snapshot_status_ok = true;
        loop_state.snapshot_status_message = "Snapshot cleared after ROM load.";
        std::cout << result.message << std::endl;
        pump_audio_output(core, audio_output);
    };
    const auto save_snapshot = [&core, &audio_output, &loop_state, &runtime_snapshot]() {
        runtime_snapshot = core.capture_snapshot();
        audio_output.clear();
        pump_audio_output(core, audio_output);
        loop_state.snapshot_available = true;
        loop_state.snapshot_status_ok = true;
        loop_state.snapshot_status_message = "Saved in-memory snapshot.";
    };
    const auto restore_snapshot = [&core, &audio_output, &loop_state, &runtime_snapshot]() {
        if (!runtime_snapshot.has_value()) {
            loop_state.snapshot_available = false;
            loop_state.snapshot_status_ok = false;
            loop_state.snapshot_status_message = "Restore failed: no snapshot saved.";
            return;
        }

        core.restore_snapshot(*runtime_snapshot);
        audio_output.clear();
        pump_audio_output(core, audio_output);
        loop_state.snapshot_available = true;
        loop_state.snapshot_status_ok = true;
        loop_state.snapshot_status_message = "Restored in-memory snapshot.";
    };

    if (!startup_rom_path.empty()) {
        load_rom_into_runtime(startup_rom_path);
        if (!loop_state.rom_load_status_ok) {
            audio_output.shutdown();
            frontend.shutdown();
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return 1;
        }
    } else {
        program_demo_tone(core);
        pump_audio_output(core, audio_output);
        loop_state.rom_load_status_ok = true;
        loop_state.rom_load_status_message = "Demo audio startup active. Use File -> Load ROM to run a cartridge.";
    }

    audio_output.start();

    std::cout << "SDL audio shell active. Tone A demo is routed through SDL audio. "
                 "Close the window or press Escape to quit."
              << std::endl;

    bool running = true;
    while (running) {
        populate_frontend_runtime_state(frontend, loop_state, core);
        frontend.begin_frame();

        SDL_Event event;
        while (SDL_PollEvent(&event) == 1) {
            frontend.handle_event(event);

            if (event.type == SDL_QUIT) {
                running = false;
            } else if (event.type == SDL_KEYDOWN && event.key.repeat == 0 &&
                       event.key.keysym.sym == SDLK_ESCAPE) {
                running = false;
            }
        }
        frontend.compose_ui();

        apply_runtime_control_commands(
            frontend,
            loop_state,
            core,
            reset_runtime,
            load_rom_into_runtime,
            save_snapshot,
            restore_snapshot);

        if (loop_state.emulation_running) {
            step_one_emulator_frame(core);
        }

        pump_audio_output(core, audio_output);
        render_frontend_frame(frontend, renderer, core);
        frontend.end_frame();

        SDL_Delay(1);
    }

    audio_output.shutdown();
    frontend.shutdown();
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    return 0;
}

int run_sdl_input_shell(EmulatorCore& core, const std::string& startup_rom_path) {
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER) != 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        return 1;
    }

    const unsigned int initial_display_scale = superz80::frontend::load_persisted_display_scale();
    const superz80::frontend::DisplayWindowSize initial_window_size =
        superz80::frontend::window_size_for_display_scale(initial_display_scale);
    SDL_Window* window = SDL_CreateWindow(
        "Super_Z80_v2 Input Shell",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        initial_window_size.width,
        initial_window_size.height,
        SDL_WINDOW_SHOWN);
    if (window == nullptr) {
        std::cerr << "SDL window creation failed: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = create_window_renderer(window);
    if (renderer == nullptr) {
        std::cerr << "SDL renderer creation failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    superz80::frontend::Frontend frontend;
    if (!frontend.initialize({"sdl-input-shell", initial_display_scale, window, renderer})) {
        std::cerr << "Frontend initialization failed for SDL input shell." << std::endl;
        SDL_DestroyRenderer(renderer);
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

    RuntimeLoopState loop_state = {};
    std::optional<EmulatorCore::Snapshot> runtime_snapshot;
    const auto reset_runtime = [&core, &loop_state, &runtime_snapshot]() {
        runtime_snapshot.reset();
        if (!loop_state.current_rom_path.empty()) {
            const RomLoadResult result = load_runtime_rom(core, loop_state.current_rom_path);
            loop_state.rom_load_status_ok = result.ok;
            loop_state.rom_load_status_message = result.message;
            if (!result.ok) {
                std::cerr << result.message << std::endl;
                return;
            }
            std::cout << result.message << std::endl;
        } else {
            core.initialize();
            loop_state.rom_load_status_ok = true;
            loop_state.rom_load_status_message = "Reset runtime with no ROM loaded.";
        }
        loop_state.snapshot_available = false;
        loop_state.snapshot_status_ok = true;
        loop_state.snapshot_status_message = "Snapshot cleared after reset.";
    };
    const auto load_rom_into_runtime = [&core, &loop_state, &runtime_snapshot](const std::string& rom_path) {
        runtime_snapshot.reset();
        const RomLoadResult result = load_runtime_rom(core, rom_path);
        loop_state.rom_load_status_ok = result.ok;
        loop_state.rom_load_status_message = result.message;
        if (!result.ok) {
            std::cerr << result.message << std::endl;
            return;
        }

        loop_state.current_rom_path = rom_path;
        loop_state.emulation_running = true;
        loop_state.snapshot_available = false;
        loop_state.snapshot_status_ok = true;
        loop_state.snapshot_status_message = "Snapshot cleared after ROM load.";
        std::cout << result.message << std::endl;
    };
    const auto save_snapshot = [&core, &loop_state, &runtime_snapshot]() {
        runtime_snapshot = core.capture_snapshot();
        loop_state.snapshot_available = true;
        loop_state.snapshot_status_ok = true;
        loop_state.snapshot_status_message = "Saved in-memory snapshot.";
    };
    const auto restore_snapshot = [&core, &loop_state, &runtime_snapshot]() {
        if (!runtime_snapshot.has_value()) {
            loop_state.snapshot_available = false;
            loop_state.snapshot_status_ok = false;
            loop_state.snapshot_status_message = "Restore failed: no snapshot saved.";
            return;
        }

        core.restore_snapshot(*runtime_snapshot);
        loop_state.snapshot_available = true;
        loop_state.snapshot_status_ok = true;
        loop_state.snapshot_status_message = "Restored in-memory snapshot.";
    };

    if (!startup_rom_path.empty()) {
        load_rom_into_runtime(startup_rom_path);
        if (!loop_state.rom_load_status_ok) {
            if (controller != nullptr) {
                SDL_GameControllerClose(controller);
            }
            frontend.shutdown();
            SDL_DestroyRenderer(renderer);
            SDL_DestroyWindow(window);
            SDL_Quit();
            return 1;
        }
    } else {
        loop_state.rom_load_status_ok = true;
        loop_state.rom_load_status_message = "No ROM loaded. Use File -> Load ROM to begin.";
    }

    bool running = true;
    while (running) {
        populate_frontend_runtime_state(frontend, loop_state, core);
        frontend.begin_frame();

        SDL_Event event;
        while (SDL_PollEvent(&event) == 1) {
            frontend.handle_event(event);
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
        frontend.compose_ui();

        apply_runtime_control_commands(
            frontend,
            loop_state,
            core,
            reset_runtime,
            load_rom_into_runtime,
            save_snapshot,
            restore_snapshot);

        if (loop_state.emulation_running) {
            step_one_emulator_frame(core);
        }

        render_frontend_frame(frontend, renderer, core);
        frontend.end_frame();

        SDL_Delay(16);
    }

    if (controller != nullptr) {
        SDL_GameControllerClose(controller);
    }
    frontend.shutdown();
    SDL_DestroyRenderer(renderer);
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
        return run_sdl_input_shell(core, options.rom_path);
#else
        std::cerr << "SDL input shell unavailable: SDL2 was not found at build time." << std::endl;
        return 1;
#endif
    }

    if (options.use_sdl_audio) {
#if defined(SUPER_Z80_HAS_SDL)
        return run_sdl_audio_shell(core, options.rom_path);
#else
        std::cerr << "SDL audio shell unavailable: SDL2 was not found at build time." << std::endl;
        return 1;
#endif
    }

    print_help();
    return 0;
}
