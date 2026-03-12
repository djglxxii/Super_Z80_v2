#include "debug_panel_host.h"

#include <algorithm>

#include "imgui.h"

namespace superz80::frontend {

void DebugPanelHost::initialize() {
    initialized_ = true;
    runtime_control_state_ = {};
    pending_runtime_control_commands_ = {};
    load_rom_popup_open_ = false;
    rom_path_input_.fill('\0');
    rom_path_input_cache_.clear();
}

void DebugPanelHost::shutdown() {
    initialized_ = false;
    runtime_control_state_ = {};
    pending_runtime_control_commands_ = {};
    load_rom_popup_open_ = false;
    rom_path_input_.fill('\0');
    rom_path_input_cache_.clear();
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

    if (ImGui::BeginMainMenuBar()) {
        if (ImGui::BeginMenu("File")) {
            if (ImGui::MenuItem("Load ROM...")) {
                load_rom_popup_open_ = true;
            }

            const bool reload_disabled = runtime_control_state_.current_rom_path.empty();
            if (reload_disabled) {
                ImGui::BeginDisabled();
            }
            if (ImGui::MenuItem("Reload ROM")) {
                pending_runtime_control_commands_.reload_rom_requested = true;
            }
            if (reload_disabled) {
                ImGui::EndDisabled();
            }

            ImGui::EndMenu();
        }
        ImGui::EndMainMenuBar();
    }

    if (load_rom_popup_open_) {
        sync_rom_path_input();
        ImGui::OpenPopup("Load ROM");
        load_rom_popup_open_ = false;
    }

    if (ImGui::BeginPopupModal("Load ROM", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGui::TextUnformatted("Enter a ROM path to load into the current runtime.");
        ImGui::InputText("ROM Path", rom_path_input_.data(), rom_path_input_.size());

        if (ImGui::Button("Load")) {
            pending_runtime_control_commands_.load_rom_requested = true;
            pending_runtime_control_commands_.rom_path_to_load = rom_path_input_.data();
            ImGui::CloseCurrentPopup();
        }

        ImGui::SameLine();
        if (ImGui::Button("Cancel")) {
            ImGui::CloseCurrentPopup();
        }

        ImGui::EndPopup();
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
    ImGui::Text("ROM Path: %s",
                runtime_control_state_.current_rom_path.empty()
                    ? "<none>"
                    : runtime_control_state_.current_rom_path.c_str());
    if (!runtime_control_state_.rom_load_status_message.empty()) {
        const ImVec4 status_color = runtime_control_state_.rom_load_status_ok
                                        ? ImVec4(0.39f, 0.83f, 0.47f, 1.0f)
                                        : ImVec4(0.93f, 0.33f, 0.30f, 1.0f);
        ImGui::TextColored(status_color,
                           "%s",
                           runtime_control_state_.rom_load_status_message.c_str());
    }
    ImGui::Text("Runtime: %s", runtime_name.c_str());
    ImGui::End();

    ImGui::Begin("Super_Z80 Frontend");
    ImGui::Text("Runtime: %s", runtime_name.c_str());
    ImGui::TextUnformatted("Control panel and ROM loader active. Debug subsystem panels arrive in later milestones.");
    ImGui::End();
}

void DebugPanelHost::end_frame() {
}

void DebugPanelHost::sync_rom_path_input() {
    if (runtime_control_state_.current_rom_path == rom_path_input_cache_) {
        return;
    }

    rom_path_input_.fill('\0');
    const std::size_t copy_length =
        std::min(runtime_control_state_.current_rom_path.size(), rom_path_input_.size() - 1U);
    runtime_control_state_.current_rom_path.copy(rom_path_input_.data(), copy_length);
    rom_path_input_cache_ = runtime_control_state_.current_rom_path;
}

} // namespace superz80::frontend
