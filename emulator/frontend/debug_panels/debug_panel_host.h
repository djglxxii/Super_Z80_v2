#ifndef SUPER_Z80_FRONTEND_DEBUG_PANEL_HOST_H
#define SUPER_Z80_FRONTEND_DEBUG_PANEL_HOST_H

#include <array>
#include <cstdint>
#include <string>

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

struct RuntimeControlState {
    bool running = true;
    unsigned int frame_counter = 0U;
    bool has_loaded_rom = false;
    bool rom_load_status_ok = false;
    std::string current_rom_path;
    std::string rom_load_status_message;
    CpuDebugState cpu_debug_state = {};
    MemoryViewerState memory_viewer_state = {};
    VramViewerState vram_viewer_state = {};
    SpriteDebugState sprite_debug_state = {};
    DmaDebugState dma_debug_state = {};
};

struct RuntimeControlCommands {
    bool toggle_run_pause = false;
    bool reset_requested = false;
    bool step_frame_requested = false;
    bool load_rom_requested = false;
    bool reload_rom_requested = false;
    std::string rom_path_to_load;
};

class DebugPanelHost {
public:
    void initialize();
    void shutdown();
    void begin_frame();
    void set_runtime_control_state(const RuntimeControlState& state);
    RuntimeControlCommands consume_runtime_control_commands();
    void render(const std::string& runtime_name);
    void end_frame();

private:
    enum class MemoryRegion : uint8_t {
        Rom = 0U,
        Ram = 1U,
    };

    void sync_rom_path_input();
    void render_memory_viewer();
    void render_vram_viewer();
    void render_sprite_debug_panel();
    void render_dma_debug_panel();
    void clamp_memory_view_start();
    void clamp_vram_view_start();

    bool initialized_ = false;
    RuntimeControlState runtime_control_state_ = {};
    RuntimeControlCommands pending_runtime_control_commands_ = {};
    bool load_rom_popup_open_ = false;
    MemoryRegion memory_region_ = MemoryRegion::Rom;
    uint16_t memory_view_start_address_ = 0x0000U;
    uint32_t vram_view_start_address_ = 0x0000U;
    std::array<char, 512> rom_path_input_ = {};
    std::string rom_path_input_cache_;
};

} // namespace superz80::frontend

#endif
