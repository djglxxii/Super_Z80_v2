#include "debug_panel_host.h"

#include "imgui.h"

namespace superz80::frontend {

void DebugPanelHost::initialize() {
    initialized_ = true;
    runtime_control_state_ = {};
    pending_runtime_control_commands_ = {};
}

void DebugPanelHost::shutdown() {
    initialized_ = false;
    runtime_control_state_ = {};
    pending_runtime_control_commands_ = {};
}

void DebugPanelHost::begin_frame() {
}

void DebugPanelHost::set_runtime_control_state(const RuntimeControlState& state) {
    runtime_control_state_ = state;
}

RuntimeControlCommands DebugPanelHost::consume_runtime_control_commands() {
    const RuntimeControlCommands commands = pending_runtime_control_commands_;
    pending_runtime_control_commands_ = {};
    return commands;
}

void DebugPanelHost::render(const std::string& runtime_name) {
    if (!initialized_) {
        return;
    }

    ImGui::Begin("Emulator Control");
    if (ImGui::Button(runtime_control_state_.running ? "Pause" : "Run")) {
        pending_runtime_control_commands_.toggle_run_pause = true;
    }

    ImGui::SameLine();
    if (ImGui::Button("Reset")) {
        pending_runtime_control_commands_.reset_requested = true;
    }

    ImGui::SameLine();
    const bool controls_disabled = runtime_control_state_.running;
    if (controls_disabled) {
        ImGui::BeginDisabled();
    }
    if (ImGui::Button("Step Frame")) {
        pending_runtime_control_commands_.step_frame_requested = true;
    }
    if (controls_disabled) {
        ImGui::EndDisabled();
    }

    ImGui::Separator();
    ImGui::Text("Execution State: %s", runtime_control_state_.running ? "Running" : "Paused");
    ImGui::Text("Frame Counter: %u", runtime_control_state_.frame_counter);
    ImGui::Text("Runtime: %s", runtime_name.c_str());
    ImGui::End();

    ImGui::Begin("Super_Z80 Frontend");
    ImGui::Text("Runtime: %s", runtime_name.c_str());
    ImGui::TextUnformatted("Control panel active. Debug subsystem panels arrive in later milestones.");
    ImGui::End();
}

void DebugPanelHost::end_frame() {
}

} // namespace superz80::frontend
