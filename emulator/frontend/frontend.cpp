#include "frontend.h"

#include <algorithm>
#include <array>
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
constexpr std::size_t kFramebufferUploadSize =
    static_cast<std::size_t>(kNativeFramebufferWidth) *
    static_cast<std::size_t>(kNativeFramebufferHeight) * 4U;

struct FrontendSettings {
    unsigned int display_scale = kDefaultDisplayScale;
    std::string rom_browser_directory;
};

FrontendSettings load_frontend_settings() {
#if defined(SUPER_Z80_HAS_SDL)
    std::ifstream settings_file(determine_frontend_settings_path());
    if (!settings_file.is_open()) {
        return {};
    }

    FrontendSettings settings = {};
    std::string line;
    while (std::getline(settings_file, line)) {
        if (line.rfind("display_scale=", 0U) == 0U) {
            std::istringstream value_stream(line.substr(14U));
            unsigned int persisted_scale = 0U;
            value_stream >> persisted_scale;
            if (!value_stream.fail() && is_supported_display_scale(persisted_scale)) {
                settings.display_scale = persisted_scale;
            }
            continue;
        }

        if (line.rfind("rom_browser_directory=", 0U) == 0U) {
            settings.rom_browser_directory = line.substr(22U);
        }
    }

    return settings;
#else
    return {};
#endif
}

#if defined(SUPER_Z80_HAS_SDL)
SDL_Rect destination_rect_for_window(int window_width, int window_height) {
    if (window_width <= 0 || window_height <= 0) {
        return {0, 0, 0, 0};
    }

    const int integer_scale = std::min(window_width / kNativeFramebufferWidth,
                                       window_height / kNativeFramebufferHeight);
    int destination_width = 0;
    int destination_height = 0;
    if (integer_scale >= 1) {
        destination_width = kNativeFramebufferWidth * integer_scale;
        destination_height = kNativeFramebufferHeight * integer_scale;
    } else {
        const float scale = std::min(static_cast<float>(window_width) /
                                         static_cast<float>(kNativeFramebufferWidth),
                                     static_cast<float>(window_height) /
                                         static_cast<float>(kNativeFramebufferHeight));
        destination_width = std::max(1, static_cast<int>(kNativeFramebufferWidth * scale));
        destination_height = std::max(1, static_cast<int>(kNativeFramebufferHeight * scale));
    }

    return {
        (window_width - destination_width) / 2,
        (window_height - destination_height) / 2,
        destination_width,
        destination_height,
    };
}
#endif
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
    return load_frontend_settings().display_scale;
}

std::string load_persisted_rom_browser_directory() {
    return load_frontend_settings().rom_browser_directory;
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
    persisted_rom_browser_directory_ = load_persisted_rom_browser_directory();
    debug_panel_host_.set_persisted_rom_browser_directory(persisted_rom_browser_directory_);

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
            persisted_rom_browser_directory_.clear();
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
            persisted_rom_browser_directory_.clear();
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
    if (framebuffer_texture_ != nullptr) {
        SDL_DestroyTexture(framebuffer_texture_);
        framebuffer_texture_ = nullptr;
    }
    window_ = nullptr;
    renderer_ = nullptr;
#endif

    debug_panel_host_.shutdown();
    imgui_ini_path_.clear();
    frontend_settings_path_.clear();
    persisted_rom_browser_directory_.clear();
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

void Frontend::compose_ui() {
    if (!initialized_) {
        return;
    }

#if defined(SUPER_Z80_HAS_SDL)
    if (!ui_initialized_) {
        return;
    }

    debug_panel_host_.render(runtime_name_);
    ImGui::Render();
#endif
}

void Frontend::update_framebuffer(const uint32_t* rgba_pixels, std::size_t pixel_count) {
    if (!initialized_ || rgba_pixels == nullptr) {
        return;
    }

#if defined(SUPER_Z80_HAS_SDL)
    if (!ui_initialized_ || !ensure_framebuffer_texture()) {
        return;
    }

    if (pixel_count != static_cast<std::size_t>(kNativeFramebufferWidth) *
                           static_cast<std::size_t>(kNativeFramebufferHeight)) {
        return;
    }

    std::array<uint8_t, kFramebufferUploadSize> upload_buffer = {};
    for (std::size_t pixel_index = 0U; pixel_index < pixel_count; ++pixel_index) {
        const uint32_t rgba = rgba_pixels[pixel_index];
        const std::size_t byte_index = pixel_index * 4U;
        upload_buffer[byte_index] = static_cast<uint8_t>((rgba >> 24U) & 0xFFU);
        upload_buffer[byte_index + 1U] = static_cast<uint8_t>((rgba >> 16U) & 0xFFU);
        upload_buffer[byte_index + 2U] = static_cast<uint8_t>((rgba >> 8U) & 0xFFU);
        upload_buffer[byte_index + 3U] = static_cast<uint8_t>(rgba & 0xFFU);
    }

    SDL_UpdateTexture(framebuffer_texture_, nullptr, upload_buffer.data(), kNativeFramebufferWidth * 4);
#else
    (void)pixel_count;
#endif
}

void Frontend::render_framebuffer() {
    if (!initialized_) {
        return;
    }

#if defined(SUPER_Z80_HAS_SDL)
    if (!ui_initialized_ || renderer_ == nullptr || framebuffer_texture_ == nullptr) {
        return;
    }

    int window_width = 0;
    int window_height = 0;
    SDL_GetWindowSize(window_, &window_width, &window_height);
    const SDL_Rect destination = destination_rect_for_window(window_width, window_height);
    if (destination.w <= 0 || destination.h <= 0) {
        return;
    }

    SDL_RenderCopy(renderer_, framebuffer_texture_, nullptr, &destination);
#endif
}

void Frontend::render_overlay() {
    if (!initialized_) {
        return;
    }

#if defined(SUPER_Z80_HAS_SDL)
    if (!ui_initialized_) {
        return;
    }

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
    if (commands.rom_browser_directory_changed) {
        persisted_rom_browser_directory_ = commands.rom_browser_directory;
        debug_panel_host_.set_persisted_rom_browser_directory(persisted_rom_browser_directory_);
        persist_settings();
        commands.rom_browser_directory_changed = false;
        commands.rom_browser_directory.clear();
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
        persist_settings();
    }
}

void Frontend::persist_settings() const {
    if (frontend_settings_path_.empty()) {
        return;
    }

    std::ofstream settings_file(frontend_settings_path_, std::ios::trunc);
    if (!settings_file.is_open()) {
        return;
    }

    settings_file << "display_scale=" << display_scale_ << '\n';
    settings_file << "rom_browser_directory=" << persisted_rom_browser_directory_ << '\n';
}

bool Frontend::ensure_framebuffer_texture() {
#if defined(SUPER_Z80_HAS_SDL)
    if (renderer_ == nullptr) {
        return false;
    }

    if (framebuffer_texture_ != nullptr) {
        return true;
    }

    framebuffer_texture_ = SDL_CreateTexture(renderer_,
                                             SDL_PIXELFORMAT_RGBA32,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             kNativeFramebufferWidth,
                                             kNativeFramebufferHeight);
    if (framebuffer_texture_ == nullptr) {
        return false;
    }

    SDL_SetTextureBlendMode(framebuffer_texture_, SDL_BLENDMODE_NONE);
    return true;
#else
    return false;
#endif
}

} // namespace superz80::frontend
