#include "frontend.h"

#include <fstream>
#include <sstream>
#include <utility>

#if defined(SUPER_Z80_HAS_SDL)
#include <SDL.h>
#include "backends/imgui_impl_sdl2.h"
#include "backends/imgui_impl_sdlrenderer2.h"
#include "imgui.h"
#endif

namespace superz80::frontend {

namespace {

#if defined(SUPER_Z80_HAS_SDL)
std::string determine_imgui_ini_path() {
    constexpr const char* kFallbackPath = "super_z80_frontend_layout.ini";

    char* pref_path = SDL_GetPrefPath("super_z80", "frontend");
    if (pref_path == nullptr) {
        return kFallbackPath;
    }

    std::string ini_path = pref_path;
    SDL_free(pref_path);
    ini_path += "super_z80_frontend_layout.ini";
    return ini_path;
}

std::string determine_frontend_settings_path() {
    constexpr const char* kFallbackPath = "super_z80_frontend_settings.cfg";

    char* pref_path = SDL_GetPrefPath("super_z80", "frontend");
    if (pref_path == nullptr) {
        return kFallbackPath;
    }

    std::string settings_path = pref_path;
    SDL_free(pref_path);
    settings_path += "super_z80_frontend_settings.cfg";
    return settings_path;
}
#endif

constexpr int kNativeFramebufferWidth = 256;
constexpr int kNativeFramebufferHeight = 192;
} // namespace

Frontend::Frontend()
    : initialized_(false),
      ui_initialized_(false) {
}

bool is_supported_display_scale(unsigned int scale) {
    return scale == 2U || scale == 4U || scale == 6U;
}

DisplayWindowSize window_size_for_display_scale(unsigned int scale) {
    const unsigned int sanitized_scale =
        is_supported_display_scale(scale) ? scale : kDefaultDisplayScale;
    return {kNativeFramebufferWidth * static_cast<int>(sanitized_scale),
            kNativeFramebufferHeight * static_cast<int>(sanitized_scale)};
}

unsigned int load_persisted_display_scale() {
#if defined(SUPER_Z80_HAS_SDL)
    std::ifstream settings_file(determine_frontend_settings_path());
    if (!settings_file.is_open()) {
        return kDefaultDisplayScale;
    }

    std::string line;
    while (std::getline(settings_file, line)) {
        if (line.rfind("display_scale=", 0U) != 0U) {
            continue;
        }

        std::istringstream value_stream(line.substr(14U));
        unsigned int persisted_scale = 0U;
        value_stream >> persisted_scale;
        if (!value_stream.fail() && is_supported_display_scale(persisted_scale)) {
            return persisted_scale;
        }
    }
#endif

    return kDefaultDisplayScale;
}

bool Frontend::initialize(const FrontendConfig& config) {
    if (initialized_) {
        return true;
    }

    runtime_name_ = config.runtime_name != nullptr ? config.runtime_name : "";
    display_scale_ = sanitized_display_scale(config.initial_display_scale);
    debug_panel_host_.initialize();
    debug_panel_host_.set_display_scale(display_scale_);

#if defined(SUPER_Z80_HAS_SDL)
    frontend_settings_path_ = determine_frontend_settings_path();

    if (config.window != nullptr && config.renderer != nullptr) {
        window_ = config.window;
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui::StyleColorsDark();
        ImGuiIO& io = ImGui::GetIO();
        imgui_ini_path_ = determine_imgui_ini_path();
        io.IniFilename = imgui_ini_path_.c_str();
        ImGui::LoadIniSettingsFromDisk(io.IniFilename);
        renderer_ = config.renderer;
        apply_display_scale(display_scale_, false);

        if (!ImGui_ImplSDL2_InitForSDLRenderer(config.window, config.renderer)) {
            ImGui::DestroyContext();
            window_ = nullptr;
            renderer_ = nullptr;
            imgui_ini_path_.clear();
            frontend_settings_path_.clear();
            debug_panel_host_.shutdown();
            runtime_name_.clear();
            return false;
        }

        if (!ImGui_ImplSDLRenderer2_Init(config.renderer)) {
            ImGui_ImplSDL2_Shutdown();
            ImGui::DestroyContext();
            window_ = nullptr;
            renderer_ = nullptr;
            imgui_ini_path_.clear();
            frontend_settings_path_.clear();
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
        ImGuiIO& io = ImGui::GetIO();
        if (io.IniFilename != nullptr) {
            ImGui::SaveIniSettingsToDisk(io.IniFilename);
        }
        ImGui_ImplSDLRenderer2_Shutdown();
        ImGui_ImplSDL2_Shutdown();
        ImGui::DestroyContext();
        ui_initialized_ = false;
    }
    window_ = nullptr;
    renderer_ = nullptr;
#endif

    debug_panel_host_.shutdown();
    imgui_ini_path_.clear();
    frontend_settings_path_.clear();
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

    RuntimeControlCommands commands = debug_panel_host_.consume_runtime_control_commands();
    if (commands.display_scale_change_requested) {
        apply_display_scale(commands.requested_display_scale, true);
        debug_panel_host_.set_display_scale(display_scale_);
        commands.display_scale_change_requested = false;
        commands.requested_display_scale = 0U;
    }
    return commands;
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

unsigned int Frontend::display_scale() const {
    return display_scale_;
}

unsigned int Frontend::sanitized_display_scale(unsigned int requested_scale) const {
    return is_supported_display_scale(requested_scale) ? requested_scale : kDefaultDisplayScale;
}

void Frontend::apply_display_scale(unsigned int scale, bool persist) {
    display_scale_ = sanitized_display_scale(scale);

#if defined(SUPER_Z80_HAS_SDL)
    if (window_ != nullptr) {
        const DisplayWindowSize size = window_size_for_display_scale(display_scale_);
        SDL_SetWindowSize(window_, size.width, size.height);
    }
#endif

    if (persist) {
        persist_display_scale();
    }
}

void Frontend::persist_display_scale() const {
    if (frontend_settings_path_.empty()) {
        return;
    }

    std::ofstream settings_file(frontend_settings_path_, std::ios::trunc);
    if (!settings_file.is_open()) {
        return;
    }

    settings_file << "display_scale=" << display_scale_ << '\n';
}

} // namespace superz80::frontend
