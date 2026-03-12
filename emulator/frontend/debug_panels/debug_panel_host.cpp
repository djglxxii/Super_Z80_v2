#include "debug_panel_host.h"

#include <algorithm>
#include <cstdint>

#include "imgui.h"

namespace superz80::frontend {

namespace {

uint8_t high_byte(uint16_t value) {
    return static_cast<uint8_t>((value >> 8U) & 0x00FFU);
}

uint8_t low_byte(uint16_t value) {
    return static_cast<uint8_t>(value & 0x00FFU);
}

bool flag_set(uint8_t flags, uint8_t mask) {
    return (flags & mask) != 0U;
}

void render_flag_row(const char* label, bool enabled) {
    ImGui::TableNextColumn();
    ImGui::TextUnformatted(label);
    ImGui::TableNextColumn();
    ImGui::TextUnformatted(enabled ? "ON" : "OFF");
}

} // namespace

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
    ImGui::TextUnformatted("Control panel, ROM loader, and CPU inspection panel are active.");
    ImGui::End();

    ImGui::Begin("CPU Debug");
    if (!runtime_control_state_.cpu_debug_state.available) {
        ImGui::TextUnformatted("CPU state is unavailable.");
        ImGui::End();
        return;
    }

    const CpuDebugState& cpu = runtime_control_state_.cpu_debug_state;
    const uint8_t a = high_byte(cpu.af);
    const uint8_t f = low_byte(cpu.af);
    const uint8_t b = high_byte(cpu.bc);
    const uint8_t c = low_byte(cpu.bc);
    const uint8_t d = high_byte(cpu.de);
    const uint8_t e = low_byte(cpu.de);
    const uint8_t h = high_byte(cpu.hl);
    const uint8_t l = low_byte(cpu.hl);

    ImGui::TextUnformatted("Registers");
    ImGui::Separator();
    ImGui::Text("A: %02X    F: %02X", a, f);
    ImGui::Text("B: %02X    C: %02X", b, c);
    ImGui::Text("D: %02X    E: %02X", d, e);
    ImGui::Text("H: %02X    L: %02X", h, l);

    ImGui::Spacing();
    ImGui::Text("AF: %04X", cpu.af);
    ImGui::Text("BC: %04X", cpu.bc);
    ImGui::Text("DE: %04X", cpu.de);
    ImGui::Text("HL: %04X", cpu.hl);

    ImGui::Spacing();
    ImGui::Text("PC: %04X", cpu.pc);
    ImGui::Text("SP: %04X", cpu.sp);
    ImGui::Text("IX: %04X", cpu.ix);
    ImGui::Text("IY: %04X", cpu.iy);
    ImGui::Text("I:  %02X", cpu.i);
    ImGui::Text("R:  %02X", cpu.r);

    ImGui::Spacing();
    ImGui::TextUnformatted("Flags");
    ImGui::Separator();
    if (ImGui::BeginTable("cpu-flags", 2, ImGuiTableFlags_SizingStretchSame)) {
        render_flag_row("S", flag_set(f, 0x80U));
        render_flag_row("Z", flag_set(f, 0x40U));
        render_flag_row("H", flag_set(f, 0x10U));
        render_flag_row("P/V", flag_set(f, 0x04U));
        render_flag_row("N", flag_set(f, 0x02U));
        render_flag_row("C", flag_set(f, 0x01U));
        ImGui::EndTable();
    }

    ImGui::Spacing();
    ImGui::TextUnformatted("CPU State");
    ImGui::Separator();
    ImGui::Text("HALT: %s", cpu.halted ? "ON" : "OFF");
    ImGui::Text("INT Line: %s", cpu.int_line ? "ON" : "OFF");
    ImGui::Text("IFF1: %s", cpu.iff1 ? "ON" : "OFF");
    ImGui::Text("IFF2: %s", cpu.iff2 ? "ON" : "OFF");
    ImGui::Text("IM: %u", static_cast<unsigned int>(cpu.interrupt_mode));
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
