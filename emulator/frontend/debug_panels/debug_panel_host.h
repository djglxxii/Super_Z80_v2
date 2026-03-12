#ifndef SUPER_Z80_FRONTEND_DEBUG_PANEL_HOST_H
#define SUPER_Z80_FRONTEND_DEBUG_PANEL_HOST_H

#include <string>

namespace superz80::frontend {

struct RuntimeControlState {
    bool running = true;
    unsigned int frame_counter = 0U;
};

struct RuntimeControlCommands {
    bool toggle_run_pause = false;
    bool reset_requested = false;
    bool step_frame_requested = false;
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
    bool initialized_ = false;
    RuntimeControlState runtime_control_state_ = {};
    RuntimeControlCommands pending_runtime_control_commands_ = {};
};

} // namespace superz80::frontend

#endif
