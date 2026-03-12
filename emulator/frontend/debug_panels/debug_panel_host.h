#ifndef SUPER_Z80_FRONTEND_DEBUG_PANEL_HOST_H
#define SUPER_Z80_FRONTEND_DEBUG_PANEL_HOST_H

#include <array>
#include <string>

namespace superz80::frontend {

struct RuntimeControlState {
    bool running = true;
    unsigned int frame_counter = 0U;
    bool has_loaded_rom = false;
    bool rom_load_status_ok = false;
    std::string current_rom_path;
    std::string rom_load_status_message;
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
