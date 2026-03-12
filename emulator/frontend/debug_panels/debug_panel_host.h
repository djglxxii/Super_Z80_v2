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

struct RuntimeControlState {
    bool running = true;
    unsigned int frame_counter = 0U;
    bool has_loaded_rom = false;
    bool rom_load_status_ok = false;
    std::string current_rom_path;
    std::string rom_load_status_message;
    CpuDebugState cpu_debug_state = {};
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
    void sync_rom_path_input();

    bool initialized_ = false;
    RuntimeControlState runtime_control_state_ = {};
    RuntimeControlCommands pending_runtime_control_commands_ = {};
    bool load_rom_popup_open_ = false;
    std::array<char, 512> rom_path_input_ = {};
    std::string rom_path_input_cache_;
};

} // namespace superz80::frontend

#endif
