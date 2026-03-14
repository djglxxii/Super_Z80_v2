#include "debug_panel_host.h"

#include <algorithm>
#include <cstdint>

#include "imgui.h"

namespace superz80::frontend {

namespace {

constexpr uint16_t kMemoryRowsPerPage = 16U;
constexpr uint16_t kMemoryBytesPerRow = 16U;
constexpr uint16_t kMemoryPageSize = kMemoryRowsPerPage * kMemoryBytesPerRow;
constexpr uint16_t kSpriteAttributeTableBase = 0x6000U;
constexpr uint16_t kSpriteAttributeSizeBytes = 4U;
constexpr float kWindowMargin = 12.0f;

constexpr ImGuiCond kInitialWindowLayoutCondition = ImGuiCond_FirstUseEver;

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

void render_panel_toggle(const char* label, bool& visible) {
    ImGui::MenuItem(label, nullptr, &visible);
}

void set_initial_window_layout(const ImVec2 desired_position, const ImVec2 desired_size) {
    const ImGuiViewport* viewport = ImGui::GetMainViewport();
    const ImVec2 work_position = viewport != nullptr ? viewport->WorkPos : ImVec2(0.0f, 0.0f);
    const ImVec2 work_size = viewport != nullptr ? viewport->WorkSize : ImVec2(1280.0f, 720.0f);

    const float max_width = work_size.x > (kWindowMargin * 2.0f)
                                ? work_size.x - (kWindowMargin * 2.0f)
                                : work_size.x;
    const float max_height = work_size.y > (kWindowMargin * 2.0f)
                                 ? work_size.y - (kWindowMargin * 2.0f)
                                 : work_size.y;

    const ImVec2 clamped_size(
        std::min(desired_size.x, std::max(max_width, 160.0f)),
        std::min(desired_size.y, std::max(max_height, 120.0f)));

    const float min_x = work_position.x + kWindowMargin;
    const float min_y = work_position.y + kWindowMargin;
    const float max_x = work_position.x + work_size.x - clamped_size.x - kWindowMargin;
    const float max_y = work_position.y + work_size.y - clamped_size.y - kWindowMargin;

    const ImVec2 clamped_position(
        std::clamp(desired_position.x, min_x, std::max(min_x, max_x)),
        std::clamp(desired_position.y, min_y, std::max(min_y, max_y)));

    ImGui::SetNextWindowPos(clamped_position, kInitialWindowLayoutCondition);
    ImGui::SetNextWindowSize(clamped_size, kInitialWindowLayoutCondition);
}

} // namespace

void DebugPanelHost::initialize() {
    initialized_ = true;
    runtime_control_state_ = {};
    pending_runtime_control_commands_ = {};
    load_rom_popup_open_ = false;
    panel_visibility_ = {};
    memory_region_ = MemoryRegion::Rom;
    memory_view_start_address_ = 0x0000U;
    vram_view_start_address_ = 0x0000U;
    display_scale_ = 2U;
    rom_path_input_.fill('\0');
    rom_path_input_cache_.clear();
}

void DebugPanelHost::shutdown() {
    initialized_ = false;
    runtime_control_state_ = {};
    pending_runtime_control_commands_ = {};
    load_rom_popup_open_ = false;
    panel_visibility_ = {};
    memory_region_ = MemoryRegion::Rom;
    memory_view_start_address_ = 0x0000U;
    vram_view_start_address_ = 0x0000U;
    display_scale_ = 2U;
    rom_path_input_.fill('\0');
    rom_path_input_cache_.clear();
}

void DebugPanelHost::begin_frame() {
}

void DebugPanelHost::set_display_scale(unsigned int scale) {
    display_scale_ = scale;
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

    render_menu_bar();

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

    render_debug_overview_panel(runtime_name);
    render_emulator_control_panel(runtime_name);
    render_cpu_debug_panel();
    render_memory_viewer();
    render_vram_viewer();
    render_sprite_debug_panel();
    render_dma_debug_panel();
    render_input_visualization_panel();
    render_audio_debug_panel();
    render_frame_timing_scheduler_panel();
}

void DebugPanelHost::end_frame() {
}

void DebugPanelHost::render_menu_bar() {
    if (!ImGui::BeginMainMenuBar()) {
        return;
    }

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

    if (ImGui::BeginMenu("Debug")) {
        render_panel_toggle("Debug Overview", panel_visibility_.debug_overview);
        render_panel_toggle("Emulator Control", panel_visibility_.emulator_control);
        render_panel_toggle("CPU Debug", panel_visibility_.cpu_debug);
        render_panel_toggle("Memory Viewer", panel_visibility_.memory_viewer);
        render_panel_toggle("VRAM Viewer", panel_visibility_.vram_viewer);
        render_panel_toggle("Sprite Debug", panel_visibility_.sprite_debug);
        render_panel_toggle("DMA Debug", panel_visibility_.dma_debug);
        render_panel_toggle("Audio Debug", panel_visibility_.audio_debug);
        render_panel_toggle("Input Visualization", panel_visibility_.input_visualization);
        render_panel_toggle("Frame Timing & Scheduler", panel_visibility_.frame_timing_scheduler);
        ImGui::Separator();
        if (ImGui::MenuItem("Show Core Panels")) {
            panel_visibility_.debug_overview = true;
            panel_visibility_.emulator_control = true;
            panel_visibility_.cpu_debug = true;
            panel_visibility_.frame_timing_scheduler = true;
        }
        if (ImGui::MenuItem("Show All Panels")) {
            panel_visibility_ = {true, true, true, true, true, true, true, true, true, true};
        }
        if (ImGui::MenuItem("Hide Inspectors")) {
            panel_visibility_.memory_viewer = false;
            panel_visibility_.vram_viewer = false;
            panel_visibility_.sprite_debug = false;
            panel_visibility_.dma_debug = false;
            panel_visibility_.audio_debug = false;
            panel_visibility_.input_visualization = false;
        }
        ImGui::EndMenu();
    }

    if (ImGui::BeginMenu("View")) {
        if (ImGui::MenuItem("Display Scale 2x", nullptr, display_scale_ == 2U)) {
            request_display_scale(2U);
        }
        if (ImGui::MenuItem("Display Scale 4x", nullptr, display_scale_ == 4U)) {
            request_display_scale(4U);
        }
        if (ImGui::MenuItem("Display Scale 6x", nullptr, display_scale_ == 6U)) {
            request_display_scale(6U);
        }
        ImGui::EndMenu();
    }

    ImGui::EndMainMenuBar();
}

void DebugPanelHost::render_debug_overview_panel(const std::string& runtime_name) {
    if (!panel_visibility_.debug_overview) {
        return;
    }

    set_initial_window_layout(ImVec2(12.0f, 32.0f), ImVec2(360.0f, 180.0f));

    if (!ImGui::Begin("Debug Overview", &panel_visibility_.debug_overview)) {
        ImGui::End();
        return;
    }

    ImGui::Text("Runtime: %s", runtime_name.c_str());
    ImGui::Text("Execution: %s", runtime_control_state_.running ? "Running" : "Paused");
    ImGui::Text("Frame: %u", runtime_control_state_.frame_counter);
    ImGui::Text("Display Scale: %ux", display_scale_);
    ImGui::Text("ROM: %s",
                runtime_control_state_.current_rom_path.empty()
                    ? "<none>"
                    : runtime_control_state_.current_rom_path.c_str());
    ImGui::Text("Snapshot Slot: %s",
                runtime_control_state_.snapshot_available ? "Available" : "Empty");

    ImGui::Separator();
    ImGui::TextUnformatted("Visible Panels");
    ImGui::Text("%d active",
                static_cast<int>(panel_visibility_.debug_overview) +
                    static_cast<int>(panel_visibility_.emulator_control) +
                    static_cast<int>(panel_visibility_.cpu_debug) +
                    static_cast<int>(panel_visibility_.memory_viewer) +
                    static_cast<int>(panel_visibility_.vram_viewer) +
                    static_cast<int>(panel_visibility_.sprite_debug) +
                    static_cast<int>(panel_visibility_.dma_debug) +
                    static_cast<int>(panel_visibility_.audio_debug) +
                    static_cast<int>(panel_visibility_.input_visualization) +
                    static_cast<int>(panel_visibility_.frame_timing_scheduler));
    ImGui::TextUnformatted("Use the Debug menu to toggle panels and keep the workspace focused.");

    ImGui::End();
}

void DebugPanelHost::render_emulator_control_panel(const std::string& runtime_name) {
    if (!panel_visibility_.emulator_control) {
        return;
    }

    set_initial_window_layout(ImVec2(12.0f, 220.0f), ImVec2(420.0f, 260.0f));

    if (!ImGui::Begin("Emulator Control", &panel_visibility_.emulator_control)) {
        ImGui::End();
        return;
    }

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

    ImGui::SameLine();
    if (ImGui::Button("Save Snapshot")) {
        pending_runtime_control_commands_.save_snapshot_requested = true;
    }

    ImGui::SameLine();
    if (!runtime_control_state_.snapshot_available) {
        ImGui::BeginDisabled();
    }
    if (ImGui::Button("Restore Snapshot")) {
        pending_runtime_control_commands_.restore_snapshot_requested = true;
    }
    if (!runtime_control_state_.snapshot_available) {
        ImGui::EndDisabled();
    }

    ImGui::Separator();
    ImGui::Text("Execution State: %s", runtime_control_state_.running ? "Running" : "Paused");
    ImGui::Text("Frame Counter: %u", runtime_control_state_.frame_counter);
    ImGui::Text("Display Scale: %ux", display_scale_);
    ImGui::Text("Snapshot: %s", runtime_control_state_.snapshot_available ? "Available" : "Empty");
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
    if (!runtime_control_state_.snapshot_status_message.empty()) {
        const ImVec4 status_color = runtime_control_state_.snapshot_status_ok
                                        ? ImVec4(0.39f, 0.83f, 0.47f, 1.0f)
                                        : ImVec4(0.93f, 0.33f, 0.30f, 1.0f);
        ImGui::TextColored(status_color,
                           "%s",
                           runtime_control_state_.snapshot_status_message.c_str());
    }
    ImGui::Text("Runtime: %s", runtime_name.c_str());

    ImGui::End();
}

void DebugPanelHost::request_display_scale(unsigned int scale) {
    pending_runtime_control_commands_.display_scale_change_requested = true;
    pending_runtime_control_commands_.requested_display_scale = scale;
}

void DebugPanelHost::render_cpu_debug_panel() {
    if (!panel_visibility_.cpu_debug) {
        return;
    }

    set_initial_window_layout(ImVec2(444.0f, 32.0f), ImVec2(300.0f, 448.0f));

    if (!ImGui::Begin("CPU Debug", &panel_visibility_.cpu_debug)) {
        ImGui::End();
        return;
    }

    if (!runtime_control_state_.cpu_debug_state.available) {
        ImGui::TextUnformatted("CPU state is unavailable.");
    } else {
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
    }

    ImGui::End();
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

void DebugPanelHost::render_memory_viewer() {
    if (!panel_visibility_.memory_viewer) {
        return;
    }

    set_initial_window_layout(ImVec2(756.0f, 32.0f), ImVec2(520.0f, 320.0f));

    if (!ImGui::Begin("Memory Viewer", &panel_visibility_.memory_viewer)) {
        ImGui::End();
        return;
    }

    if (!runtime_control_state_.memory_viewer_state.available) {
        ImGui::TextUnformatted("Memory viewer state is unavailable.");
        ImGui::End();
        return;
    }

    int selected_region = memory_region_ == MemoryRegion::Rom ? 0 : 1;
    if (ImGui::RadioButton("ROM", selected_region == 0)) {
        selected_region = 0;
    }
    ImGui::SameLine();
    if (ImGui::RadioButton("RAM", selected_region == 1)) {
        selected_region = 1;
    }

    const MemoryRegion previous_region = memory_region_;
    memory_region_ = selected_region == 0 ? MemoryRegion::Rom : MemoryRegion::Ram;
    if (memory_region_ != previous_region) {
        memory_view_start_address_ =
            memory_region_ == MemoryRegion::Rom ? 0x0000U : 0xC000U;
    }

    clamp_memory_view_start();

    uint32_t start_address = memory_view_start_address_;
    ImGui::SetNextItemWidth(120.0f);
    if (ImGui::InputScalar("Start", ImGuiDataType_U32, &start_address, nullptr, nullptr, "%04X",
                           ImGuiInputTextFlags_CharsHexadecimal)) {
        memory_view_start_address_ = static_cast<uint16_t>(start_address & 0xFFFFU);
        clamp_memory_view_start();
    }

    if (ImGui::Button("Prev Page")) {
        memory_view_start_address_ = static_cast<uint16_t>(
            memory_view_start_address_ >= kMemoryPageSize
                ? memory_view_start_address_ - kMemoryPageSize
                : 0U);
        clamp_memory_view_start();
    }

    ImGui::SameLine();
    if (ImGui::Button("Next Page")) {
        memory_view_start_address_ =
            static_cast<uint16_t>(memory_view_start_address_ + kMemoryPageSize);
        clamp_memory_view_start();
    }

    ImGui::Separator();

    const uint16_t region_start = memory_region_ == MemoryRegion::Rom ? 0x0000U : 0xC000U;
    const uint16_t region_end = memory_region_ == MemoryRegion::Rom ? 0x7FFFU : 0xFFFFU;
    const uint8_t* bytes = memory_region_ == MemoryRegion::Rom
                               ? runtime_control_state_.memory_viewer_state.rom.data()
                               : runtime_control_state_.memory_viewer_state.ram.data();
    const std::size_t bytes_size = memory_region_ == MemoryRegion::Rom
                                       ? runtime_control_state_.memory_viewer_state.rom.size()
                                       : runtime_control_state_.memory_viewer_state.ram.size();

    ImGui::Text("Region: %s  Range: %04X-%04X",
                memory_region_ == MemoryRegion::Rom ? "ROM" : "RAM",
                static_cast<unsigned int>(region_start),
                static_cast<unsigned int>(region_end));

    if (ImGui::BeginChild("memory-viewer-scroll", ImVec2(0.0f, 0.0f), true,
                          ImGuiWindowFlags_HorizontalScrollbar)) {
        for (uint16_t row = 0U; row < kMemoryRowsPerPage; ++row) {
            const uint32_t address = static_cast<uint32_t>(memory_view_start_address_) +
                                     (static_cast<uint32_t>(row) * kMemoryBytesPerRow);
            if (address > region_end) {
                break;
            }

            const std::size_t base_index = static_cast<std::size_t>(address - region_start);
            ImGui::Text("%04X:", static_cast<unsigned int>(address));
            ImGui::SameLine();

            for (uint16_t column = 0U; column < kMemoryBytesPerRow; ++column) {
                const std::size_t index = base_index + column;
                if (index >= bytes_size) {
                    break;
                }

                ImGui::SameLine();
                ImGui::Text("%02X", static_cast<unsigned int>(bytes[index]));
            }
        }
    }
    ImGui::EndChild();
    ImGui::End();
}

void DebugPanelHost::clamp_memory_view_start() {
    const uint16_t region_start = memory_region_ == MemoryRegion::Rom ? 0x0000U : 0xC000U;
    const uint16_t region_end = memory_region_ == MemoryRegion::Rom ? 0x7FFFU : 0xFFFFU;
    const uint16_t max_start =
        region_end >= (kMemoryPageSize - 1U) ? static_cast<uint16_t>(region_end - (kMemoryPageSize - 1U))
                                             : region_start;

    if (memory_view_start_address_ < region_start) {
        memory_view_start_address_ = region_start;
    }
    if (memory_view_start_address_ > max_start) {
        memory_view_start_address_ = max_start;
    }
}

void DebugPanelHost::render_vram_viewer() {
    if (!panel_visibility_.vram_viewer) {
        return;
    }

    set_initial_window_layout(ImVec2(756.0f, 364.0f), ImVec2(520.0f, 320.0f));

    if (!ImGui::Begin("VRAM Viewer", &panel_visibility_.vram_viewer)) {
        ImGui::End();
        return;
    }

    if (!runtime_control_state_.vram_viewer_state.available) {
        ImGui::TextUnformatted("VRAM viewer state is unavailable.");
        ImGui::End();
        return;
    }

    clamp_vram_view_start();

    uint32_t start_address = vram_view_start_address_;
    ImGui::SetNextItemWidth(120.0f);
    if (ImGui::InputScalar("Start", ImGuiDataType_U32, &start_address, nullptr, nullptr, "%04X",
                           ImGuiInputTextFlags_CharsHexadecimal)) {
        vram_view_start_address_ = start_address & 0xFFFFU;
        clamp_vram_view_start();
    }

    if (ImGui::Button("Prev Page")) {
        vram_view_start_address_ =
            vram_view_start_address_ >= kMemoryPageSize
                ? vram_view_start_address_ - kMemoryPageSize
                : 0U;
        clamp_vram_view_start();
    }

    ImGui::SameLine();
    if (ImGui::Button("Next Page")) {
        vram_view_start_address_ += kMemoryPageSize;
        clamp_vram_view_start();
    }

    ImGui::Separator();
    ImGui::Text("Range: %04X-%04X  Size: %u bytes",
                0U,
                0xFFFFU,
                static_cast<unsigned int>(runtime_control_state_.vram_viewer_state.vram.size()));

    if (ImGui::BeginChild("vram-viewer-scroll", ImVec2(0.0f, 0.0f), true,
                          ImGuiWindowFlags_HorizontalScrollbar)) {
        for (uint16_t row = 0U; row < kMemoryRowsPerPage; ++row) {
            const uint32_t address = vram_view_start_address_ +
                                     (static_cast<uint32_t>(row) * kMemoryBytesPerRow);
            if (address >= runtime_control_state_.vram_viewer_state.vram.size()) {
                break;
            }

            ImGui::Text("%04X:", static_cast<unsigned int>(address));
            ImGui::SameLine();

            for (uint16_t column = 0U; column < kMemoryBytesPerRow; ++column) {
                const std::size_t index = static_cast<std::size_t>(address) + column;
                if (index >= runtime_control_state_.vram_viewer_state.vram.size()) {
                    break;
                }

                ImGui::SameLine();
                ImGui::Text("%02X",
                            static_cast<unsigned int>(runtime_control_state_.vram_viewer_state.vram[index]));
            }
        }
    }
    ImGui::EndChild();
    ImGui::End();
}

void DebugPanelHost::render_sprite_debug_panel() {
    if (!panel_visibility_.sprite_debug) {
        return;
    }

    set_initial_window_layout(ImVec2(12.0f, 492.0f), ImVec2(420.0f, 280.0f));

    if (!ImGui::Begin("Sprite Debug", &panel_visibility_.sprite_debug)) {
        ImGui::End();
        return;
    }

    if (!runtime_control_state_.sprite_debug_state.available) {
        ImGui::TextUnformatted("Sprite debug state is unavailable.");
        ImGui::End();
        return;
    }

    ImGui::Text("SAT Base: %04X  Entries: %u  Entry Size: %u bytes",
                static_cast<unsigned int>(kSpriteAttributeTableBase),
                static_cast<unsigned int>(runtime_control_state_.sprite_debug_state.sprites.size()),
                static_cast<unsigned int>(kSpriteAttributeSizeBytes));
    ImGui::Separator();

    if (ImGui::BeginTable("sprite-debug-table",
                          5,
                          ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                              ImGuiTableFlags_ScrollY | ImGuiTableFlags_SizingFixedFit,
                          ImVec2(0.0f, 320.0f))) {
        ImGui::TableSetupColumn("Index");
        ImGui::TableSetupColumn("X");
        ImGui::TableSetupColumn("Y");
        ImGui::TableSetupColumn("Tile");
        ImGui::TableSetupColumn("Attr");
        ImGui::TableHeadersRow();

        for (std::size_t sprite_index = 0U;
             sprite_index < runtime_control_state_.sprite_debug_state.sprites.size();
             ++sprite_index) {
            const SpriteDebugEntry& sprite = runtime_control_state_.sprite_debug_state.sprites[sprite_index];
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            ImGui::Text("%02u", static_cast<unsigned int>(sprite_index));
            ImGui::TableNextColumn();
            ImGui::Text("%02X", static_cast<unsigned int>(sprite.x));
            ImGui::TableNextColumn();
            ImGui::Text("%02X", static_cast<unsigned int>(sprite.y));
            ImGui::TableNextColumn();
            ImGui::Text("%02X", static_cast<unsigned int>(sprite.tile_index));
            ImGui::TableNextColumn();
            ImGui::Text("%02X", static_cast<unsigned int>(sprite.attributes));
        }

        ImGui::EndTable();
    }

    ImGui::End();
}

void DebugPanelHost::render_dma_debug_panel() {
    if (!panel_visibility_.dma_debug) {
        return;
    }

    set_initial_window_layout(ImVec2(444.0f, 492.0f), ImVec2(300.0f, 180.0f));

    if (!ImGui::Begin("DMA Debug", &panel_visibility_.dma_debug)) {
        ImGui::End();
        return;
    }

    if (!runtime_control_state_.dma_debug_state.available) {
        ImGui::TextUnformatted("DMA debug state is unavailable.");
        ImGui::End();
        return;
    }

    const DmaDebugState& dma = runtime_control_state_.dma_debug_state;
    ImGui::Text("Source:      %04X", static_cast<unsigned int>(dma.source_address));
    ImGui::Text("Destination: %04X", static_cast<unsigned int>(dma.destination_address));
    ImGui::Text("Length:      %02X", static_cast<unsigned int>(dma.transfer_length));
    ImGui::Text("Control:     %02X", static_cast<unsigned int>(dma.control));
    ImGui::Separator();
    ImGui::Text("Status:      %s", dma.active ? "ACTIVE" : "IDLE");
    ImGui::Text("Busy Bit:    %s",
                (dma.control & 0x80U) != 0U ? "SET" : "CLEAR");
    ImGui::End();
}

void DebugPanelHost::render_audio_debug_panel() {
    if (!panel_visibility_.audio_debug) {
        return;
    }

    set_initial_window_layout(ImVec2(12.0f, 32.0f), ImVec2(560.0f, 420.0f));

    if (!ImGui::Begin("Audio Debug", &panel_visibility_.audio_debug)) {
        ImGui::End();
        return;
    }

    if (!runtime_control_state_.audio_debug_state.available) {
        ImGui::TextUnformatted("Audio debug state is unavailable.");
        ImGui::End();
        return;
    }

    const AudioDebugPsgState& psg = runtime_control_state_.audio_debug_state.psg;
    const AudioDebugYmState& ym = runtime_control_state_.audio_debug_state.ym2151;

    if (ImGui::CollapsingHeader("PSG", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("Enabled: %s", psg.enabled ? "YES" : "NO");
        ImGui::Text("Muted: %s", psg.muted ? "YES" : "NO");
        ImGui::Text("Overrun: %s", psg.overrun ? "YES" : "NO");
        ImGui::Text("Control: %02X", static_cast<unsigned int>(psg.control));
        ImGui::Text("Current Sample: %d", static_cast<int>(psg.current_sample));
        ImGui::Separator();

        if (ImGui::BeginTable("audio-debug-psg-channels",
                              5,
                              ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                                  ImGuiTableFlags_SizingFixedFit)) {
            ImGui::TableSetupColumn("Channel");
            ImGui::TableSetupColumn("Period");
            ImGui::TableSetupColumn("Volume");
            ImGui::TableSetupColumn("Divider");
            ImGui::TableSetupColumn("Phase");
            ImGui::TableHeadersRow();

            for (std::size_t channel = 0U; channel < psg.tone_channels.size(); ++channel) {
                const AudioDebugPsgChannelState& state = psg.tone_channels[channel];
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("Tone %c", static_cast<char>('A' + channel));
                ImGui::TableNextColumn();
                ImGui::Text("%03X", static_cast<unsigned int>(state.period));
                ImGui::TableNextColumn();
                ImGui::Text("%X", static_cast<unsigned int>(state.volume));
                ImGui::TableNextColumn();
                ImGui::Text("%03X", static_cast<unsigned int>(state.divider_counter));
                ImGui::TableNextColumn();
                ImGui::TextUnformatted(state.phase_high ? "HIGH" : "LOW");
            }

            ImGui::EndTable();
        }

        ImGui::Spacing();
        ImGui::Text("Noise Ctrl: %02X  Volume: %X  Divider: %03X  LFSR: %04X  Out: %u",
                    static_cast<unsigned int>(psg.noise.control),
                    static_cast<unsigned int>(psg.noise.volume),
                    static_cast<unsigned int>(psg.noise.divider_counter),
                    static_cast<unsigned int>(psg.noise.lfsr),
                    static_cast<unsigned int>(psg.noise.output_bit));
        ImGui::Separator();
        ImGui::TextUnformatted("Registers");
        if (ImGui::BeginChild("audio-debug-psg-registers", ImVec2(0.0f, 120.0f), true)) {
            for (std::size_t index = 0U; index < psg.registers.size(); ++index) {
                const uint8_t port = static_cast<uint8_t>(0xD0U + index);
                ImGui::Text("Port %02X: %02X",
                            static_cast<unsigned int>(port),
                            static_cast<unsigned int>(psg.registers[index]));
            }
        }
        ImGui::EndChild();
    }

    if (ImGui::CollapsingHeader("YM2151", ImGuiTreeNodeFlags_DefaultOpen)) {
        ImGui::Text("Selected Register: %02X", static_cast<unsigned int>(ym.selected_register));
        ImGui::Text("Status: %02X", static_cast<unsigned int>(ym.status));
        ImGui::Text("IRQ Pending: %s", ym.irq_pending ? "YES" : "NO");
        ImGui::Text("Current Sample: %d", static_cast<int>(ym.current_sample));
        ImGui::Text("Tick Calls: %llu", static_cast<unsigned long long>(ym.tick_call_count));
        ImGui::Text("Accumulated Cycles: %llu",
                    static_cast<unsigned long long>(ym.accumulated_cycles));
        ImGui::Separator();
        ImGui::Text("Timer A  Latch: %03X  Counter: %u  Enabled: %s  Overflow: %s  IRQ: %s",
                    static_cast<unsigned int>(ym.timer_a.latch),
                    static_cast<unsigned int>(ym.timer_a.counter),
                    ym.timer_a.enabled ? "YES" : "NO",
                    ym.timer_a.overflow ? "YES" : "NO",
                    ym.timer_a.irq_enabled ? "YES" : "NO");
        ImGui::Text("Timer B  Latch: %02X  Counter: %u  Enabled: %s  Overflow: %s  IRQ: %s",
                    static_cast<unsigned int>(ym.timer_b.latch),
                    static_cast<unsigned int>(ym.timer_b.counter),
                    ym.timer_b.enabled ? "YES" : "NO",
                    ym.timer_b.overflow ? "YES" : "NO",
                    ym.timer_b.irq_enabled ? "YES" : "NO");
        ImGui::Separator();

        if (ImGui::BeginTable("audio-debug-ym-channels",
                              6,
                              ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg |
                                  ImGuiTableFlags_SizingFixedFit,
                              ImVec2(0.0f, 180.0f))) {
            ImGui::TableSetupColumn("Ch");
            ImGui::TableSetupColumn("Freq");
            ImGui::TableSetupColumn("Block");
            ImGui::TableSetupColumn("Alg");
            ImGui::TableSetupColumn("Fb");
            ImGui::TableSetupColumn("KeyOn");
            ImGui::TableHeadersRow();

            for (std::size_t channel = 0U; channel < ym.channels.size(); ++channel) {
                const AudioDebugYmChannelState& state = ym.channels[channel];
                ImGui::TableNextRow();
                ImGui::TableNextColumn();
                ImGui::Text("%u", static_cast<unsigned int>(channel));
                ImGui::TableNextColumn();
                ImGui::Text("%03X", static_cast<unsigned int>(state.frequency));
                ImGui::TableNextColumn();
                ImGui::Text("%u", static_cast<unsigned int>(state.block));
                ImGui::TableNextColumn();
                ImGui::Text("%u", static_cast<unsigned int>(state.algorithm));
                ImGui::TableNextColumn();
                ImGui::Text("%u", static_cast<unsigned int>(state.feedback));
                ImGui::TableNextColumn();
                ImGui::Text("%X", static_cast<unsigned int>(state.key_on_mask));
            }

            ImGui::EndTable();
        }

        ImGui::Spacing();
        ImGui::TextUnformatted("Registers");
        if (ImGui::BeginChild("audio-debug-ym-registers", ImVec2(0.0f, 220.0f), true,
                              ImGuiWindowFlags_HorizontalScrollbar)) {
            for (std::size_t row = 0U; row < ym.registers.size(); row += 16U) {
                ImGui::Text("%02X:", static_cast<unsigned int>(row));
                for (std::size_t column = 0U; column < 16U; ++column) {
                    const std::size_t index = row + column;
                    ImGui::SameLine();
                    ImGui::Text("%02X", static_cast<unsigned int>(ym.registers[index]));
                }
            }
        }
        ImGui::EndChild();
    }

    ImGui::End();
}

void DebugPanelHost::render_input_visualization_panel() {
    if (!panel_visibility_.input_visualization) {
        return;
    }

    set_initial_window_layout(ImVec2(444.0f, 684.0f), ImVec2(300.0f, 180.0f));

    if (!ImGui::Begin("Input Visualization", &panel_visibility_.input_visualization)) {
        ImGui::End();
        return;
    }

    if (!runtime_control_state_.input_visualization_state.available) {
        ImGui::TextUnformatted("Input visualization state is unavailable.");
        ImGui::End();
        return;
    }

    const InputVisualizationState& input = runtime_control_state_.input_visualization_state;

    ImGui::TextUnformatted("Buttons");
    ImGui::Separator();
    if (ImGui::BeginTable("input-buttons", 2, ImGuiTableFlags_SizingStretchSame)) {
        render_flag_row("Up", input.up);
        render_flag_row("Down", input.down);
        render_flag_row("Left", input.left);
        render_flag_row("Right", input.right);
        render_flag_row("A", input.a);
        render_flag_row("B", input.b);
        render_flag_row("Start", input.start);
        ImGui::EndTable();
    }

    ImGui::Spacing();
    ImGui::TextUnformatted("Raw Ports");
    ImGui::Separator();
    ImGui::Text("PAD1:     0x%02X", input.pad1);
    ImGui::Text("PAD1_SYS: 0x%02X", input.pad1_sys);
    ImGui::TextUnformatted("Active-low input bits clear to 0 when pressed.");
    ImGui::End();
}

void DebugPanelHost::render_frame_timing_scheduler_panel() {
    if (!panel_visibility_.frame_timing_scheduler) {
        return;
    }

    set_initial_window_layout(ImVec2(756.0f, 32.0f), ImVec2(360.0f, 220.0f));

    if (!ImGui::Begin("Frame Timing & Scheduler", &panel_visibility_.frame_timing_scheduler)) {
        ImGui::End();
        return;
    }

    if (!runtime_control_state_.frame_timing_state.available) {
        ImGui::TextUnformatted("Frame timing state is unavailable.");
        ImGui::End();
        return;
    }

    const FrameTimingState& timing = runtime_control_state_.frame_timing_state;

    ImGui::Text("Frame Counter:            %u",
                static_cast<unsigned int>(timing.frame_counter));
    ImGui::Text("Scanline Counter:         %u",
                static_cast<unsigned int>(timing.scanline_counter));
    ImGui::Text("Scanlines/Frame:          %u",
                static_cast<unsigned int>(timing.scanlines_per_frame));
    ImGui::Text("Instructions/Scanline:    %u",
                static_cast<unsigned int>(timing.instructions_per_scanline));
    ImGui::Text("YM2151 Cycles/Scanline:   %u",
                static_cast<unsigned int>(timing.ym2151_cycles_per_scanline));
    ImGui::Separator();
    ImGui::Text("VBlank:                   %s", timing.vblank_active ? "ON" : "OFF");
    ImGui::Text("Frame Ready:              %s", timing.frame_ready ? "ON" : "OFF");
    ImGui::Text("Buffered Audio Samples:   %u",
                static_cast<unsigned int>(timing.buffered_audio_samples));
    ImGui::End();
}

void DebugPanelHost::clamp_vram_view_start() {
    const uint32_t max_start = static_cast<uint32_t>(
        runtime_control_state_.vram_viewer_state.vram.size() > kMemoryPageSize
            ? runtime_control_state_.vram_viewer_state.vram.size() - kMemoryPageSize
            : 0U);

    if (vram_view_start_address_ > max_start) {
        vram_view_start_address_ = max_start;
    }
}

} // namespace superz80::frontend
