#include "debug_panel_host.h"

#include "imgui.h"

namespace superz80::frontend {

void DebugPanelHost::initialize() {
    initialized_ = true;
}

void DebugPanelHost::shutdown() {
    initialized_ = false;
}

void DebugPanelHost::begin_frame() {
}

void DebugPanelHost::render(const std::string& runtime_name) {
    if (!initialized_) {
        return;
    }

    ImGui::Begin("Super_Z80 Frontend");
    ImGui::TextUnformatted("Frontend initialized.");
    ImGui::Separator();
    ImGui::Text("Runtime: %s", runtime_name.c_str());
    ImGui::TextUnformatted("Placeholder UI only. Debug panels arrive in later milestones.");
    ImGui::End();
}

void DebugPanelHost::end_frame() {
}

} // namespace superz80::frontend
