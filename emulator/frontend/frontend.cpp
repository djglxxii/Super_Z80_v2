#include "frontend.h"

#if defined(SUPER_Z80_HAS_SDL)
#include <SDL.h>
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"
#include "imgui.h"
#endif

namespace superz80::frontend {

Frontend::Frontend()
    : initialized_(false),
      ui_initialized_(false) {
}

bool Frontend::initialize(const FrontendConfig& config) {
    if (initialized_) {
        return true;
    }

    runtime_name_ = config.runtime_name != nullptr ? config.runtime_name : "";
    debug_panel_host_.initialize();

#if defined(SUPER_Z80_HAS_SDL)
    if (config.window != nullptr && config.renderer != nullptr) {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        renderer_ = config.renderer;

        if (!ImGui_ImplSDL2_InitForSDLRenderer(config.window, config.renderer)) {
            ImGui::DestroyContext();
            renderer_ = nullptr;
            debug_panel_host_.shutdown();
            runtime_name_.clear();
            return false;
        }

        if (!ImGui_ImplSDLRenderer2_Init(config.renderer)) {
            ImGui_ImplSDL2_Shutdown();
            ImGui::DestroyContext();
            renderer_ = nullptr;
            debug_panel_host_.shutdown();
            runtime_name_.clear();
            return false;
        }

        ui_initialized_ = true;
    }
#endif

    initialized_ = true;
    return true;
}

void Frontend::shutdown() {
    if (!initialized_) {
        return;
    }

#if defined(SUPER_Z80_HAS_SDL)
    if (ui_initialized_) {
        ImGui_ImplSDLRenderer2_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
        ui_initialized_ = false;
    }
    renderer_ = nullptr;
#endif

    debug_panel_host_.shutdown();
    runtime_name_.clear();
    initialized_ = false;
}

void Frontend::handle_event(const SDL_Event& event) {
    if (!initialized_) {
        return;
    }

#if defined(SUPER_Z80_HAS_SDL)
    if (ui_initialized_) {
        ImGui_ImplSDL2_ProcessEvent(&event);
    }
#else
    (void)event;
#endif
}

void Frontend::begin_frame() {
    if (!initialized_) {
        return;
    }

#if defined(SUPER_Z80_HAS_SDL)
    if (ui_initialized_) {
        ImGui_ImplSDLRenderer2_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();
    }
#endif

    debug_panel_host_.begin_frame();
}

void Frontend::render() {
    if (!initialized_) {
        return;
    }

#if defined(SUPER_Z80_HAS_SDL)
    if (!ui_initialized_) {
        return;
    }

    debug_panel_host_.render(runtime_name_);
    ImGui::Render();
    ImGui_ImplSDLRenderer2_RenderDrawData(ImGui::GetDrawData(), renderer_);
#endif
}

void Frontend::end_frame() {
    if (!initialized_) {
        return;
    }

    debug_panel_host_.end_frame();
}

void Frontend::set_runtime_control_state(const RuntimeControlState& state) {
    if (!initialized_) {
        return;
    }

    debug_panel_host_.set_runtime_control_state(state);
}

RuntimeControlCommands Frontend::consume_runtime_control_commands() {
    if (!initialized_) {
        return {};
    }

    return debug_panel_host_.consume_runtime_control_commands();
}

bool Frontend::is_initialized() const {
    return initialized_;
}

bool Frontend::is_ui_available() const {
    return ui_initialized_;
}

const std::string& Frontend::runtime_name() const {
    return runtime_name_;
}

} // namespace superz80::frontend
