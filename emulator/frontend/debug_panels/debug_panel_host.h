#ifndef SUPER_Z80_FRONTEND_DEBUG_PANEL_HOST_H
#define SUPER_Z80_FRONTEND_DEBUG_PANEL_HOST_H

#include <array>
#include <cstddef>
#include <cstdint>
#include <string_view>
#include <string>
#include <vector>

namespace superz80::frontend {

struct CpuDebugState {
    bool available = false;
    uint16_t af = 0U;
    uint16_t bc = 0U;
    uint16_t de = 0U;
    uint16_t hl = 0U;
    uint16_t ix = 0U;
    uint16_t iy = 0U;
    uint16_t pc = 0U;
    uint16_t sp = 0U;
    uint8_t i = 0U;
    uint8_t r = 0U;
    uint8_t interrupt_mode = 0U;
    bool iff1 = false;
    bool iff2 = false;
    bool halted = false;
    bool int_line = false;
};

struct MemoryViewerState {
    bool available = false;
    std::array<uint8_t, 0x8000U> rom = {};
    std::array<uint8_t, 0x4000U> ram = {};
};

struct VramViewerState {
    bool available = false;
    std::array<uint8_t, 0x10000U> vram = {};
};

struct SpriteDebugEntry {
    uint8_t x = 0U;
    uint8_t y = 0U;
    uint8_t tile_index = 0U;
    uint8_t attributes = 0U;
};

struct SpriteDebugState {
    bool available = false;
    std::array<SpriteDebugEntry, 64U> sprites = {};
};

struct DmaDebugState {
    bool available = false;
    uint16_t source_address = 0U;
    uint16_t destination_address = 0U;
    uint8_t transfer_length = 0U;
    uint8_t control = 0U;
    bool active = false;
};

struct InputVisualizationState {
    bool available = false;
    bool up = false;
    bool down = false;
    bool left = false;
    bool right = false;
    bool a = false;
    bool b = false;
    bool start = false;
    uint8_t pad1 = 0xFFU;
    uint8_t pad1_sys = 0xFFU;
};

struct AudioDebugPsgChannelState {
    uint16_t period = 0U;
    uint8_t volume = 0U;
    uint16_t divider_counter = 0U;
    bool phase_high = false;
};

struct AudioDebugPsgNoiseState {
    uint8_t control = 0U;
    uint8_t volume = 0U;
    uint16_t divider_counter = 0U;
    uint16_t lfsr = 0U;
    uint8_t output_bit = 0U;
};

struct AudioDebugPsgState {
    bool available = false;
    std::array<uint8_t, 12U> registers = {};
    std::array<AudioDebugPsgChannelState, 3U> tone_channels = {};
    AudioDebugPsgNoiseState noise = {};
    uint8_t control = 0U;
    bool enabled = false;
    bool muted = false;
    bool overrun = false;
    int16_t current_sample = 0;
};

struct AudioDebugYmChannelState {
    uint16_t frequency = 0U;
    uint8_t block = 0U;
    uint8_t algorithm = 0U;
    uint8_t feedback = 0U;
    uint8_t key_on_mask = 0U;
};

struct AudioDebugYmTimerState {
    uint16_t latch = 0U;
    uint32_t counter = 0U;
    bool enabled = false;
    bool overflow = false;
    bool irq_enabled = false;
};

struct AudioDebugYmState {
    bool available = false;
    uint8_t selected_register = 0U;
    std::array<uint8_t, 256U> registers = {};
    std::array<AudioDebugYmChannelState, 8U> channels = {};
    AudioDebugYmTimerState timer_a = {};
    AudioDebugYmTimerState timer_b = {};
    uint8_t status = 0U;
    bool irq_pending = false;
    int16_t current_sample = 0;
    uint64_t tick_call_count = 0U;
    uint64_t accumulated_cycles = 0U;
};

struct AudioDebugState {
    bool available = false;
    AudioDebugPsgState psg = {};
    AudioDebugYmState ym2151 = {};
};

struct FrameTimingState {
    bool available = false;
    uint32_t frame_counter = 0U;
    uint32_t scanline_counter = 0U;
    uint32_t scanlines_per_frame = 0U;
    uint32_t instructions_per_scanline = 0U;
    uint32_t ym2151_cycles_per_scanline = 0U;
    bool vblank_active = false;
    bool frame_ready = false;
    std::size_t buffered_audio_samples = 0U;
};

struct RuntimeControlState {
    bool running = true;
    unsigned int frame_counter = 0U;
    bool has_loaded_rom = false;
    bool rom_load_status_ok = false;
    bool snapshot_available = false;
    bool snapshot_status_ok = false;
    std::string current_rom_path;
    std::string rom_load_status_message;
    std::string snapshot_status_message;
    CpuDebugState cpu_debug_state = {};
    MemoryViewerState memory_viewer_state = {};
    VramViewerState vram_viewer_state = {};
    SpriteDebugState sprite_debug_state = {};
    DmaDebugState dma_debug_state = {};
    InputVisualizationState input_visualization_state = {};
    AudioDebugState audio_debug_state = {};
    FrameTimingState frame_timing_state = {};
};

struct RuntimeControlCommands {
    bool toggle_run_pause = false;
    bool reset_requested = false;
    bool step_frame_requested = false;
    bool save_snapshot_requested = false;
    bool restore_snapshot_requested = false;
    bool load_rom_requested = false;
    bool reload_rom_requested = false;
    bool display_scale_change_requested = false;
    unsigned int requested_display_scale = 0U;
    std::string rom_path_to_load;
    bool rom_browser_directory_changed = false;
    std::string rom_browser_directory;
};

class DebugPanelHost {
public:
    void initialize();
    void shutdown();
    void begin_frame();
    void set_display_scale(unsigned int scale);
    void set_persisted_rom_browser_directory(const std::string& directory);
    void set_runtime_control_state(const RuntimeControlState& state);
    RuntimeControlCommands consume_runtime_control_commands();
    void render(const std::string& runtime_name);
    void end_frame();

private:
    struct RomBrowserEntry {
        std::string label;
        std::string path;
        bool is_directory = false;
    };

    struct PanelVisibilityState {
        bool emulator_control = true;
        bool debug_overview = true;
        bool cpu_debug = true;
        bool memory_viewer = false;
        bool vram_viewer = false;
        bool sprite_debug = false;
        bool dma_debug = false;
        bool audio_debug = false;
        bool input_visualization = false;
        bool frame_timing_scheduler = true;
    };

    enum class MemoryRegion : uint8_t {
        Rom = 0U,
        Ram = 1U,
    };

    void render_menu_bar();
    void render_debug_overview_panel(const std::string& runtime_name);
    void render_emulator_control_panel(const std::string& runtime_name);
    void request_display_scale(unsigned int scale);
    void render_cpu_debug_panel();
    void sync_rom_path_input();
    void open_load_rom_popup();
    void refresh_rom_browser_entries(bool force_reset_selection);
    void navigate_rom_browser_to(const std::string& directory, bool force_reset_selection);
    bool is_likely_rom_file(std::string_view extension) const;
    bool has_rom_browser_selected_file() const;
    void request_rom_browser_directory_persist(const std::string& directory);
    void render_load_rom_popup();
    void render_memory_viewer();
    void render_vram_viewer();
    void render_sprite_debug_panel();
    void render_dma_debug_panel();
    void render_input_visualization_panel();
    void render_audio_debug_panel();
    void render_frame_timing_scheduler_panel();
    void clamp_memory_view_start();
    void clamp_vram_view_start();

    bool initialized_ = false;
    RuntimeControlState runtime_control_state_ = {};
    RuntimeControlCommands pending_runtime_control_commands_ = {};
    bool load_rom_popup_open_ = false;
    PanelVisibilityState panel_visibility_ = {};
    MemoryRegion memory_region_ = MemoryRegion::Rom;
    uint16_t memory_view_start_address_ = 0x0000U;
    uint32_t vram_view_start_address_ = 0x0000U;
    unsigned int display_scale_ = 2U;
    std::array<char, 512> rom_path_input_ = {};
    std::string rom_path_input_cache_;
    std::string persisted_rom_browser_directory_;
    std::string rom_browser_current_directory_;
    std::string rom_browser_directory_error_;
    std::vector<RomBrowserEntry> rom_browser_entries_;
    int rom_browser_selected_index_ = -1;
};

} // namespace superz80::frontend

#endif
