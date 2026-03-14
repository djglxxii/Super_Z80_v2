#ifndef SUPER_Z80_FRONTEND_FRONTEND_H
#define SUPER_Z80_FRONTEND_FRONTEND_H

#include <string>

#include "debug_panel_host.h"

struct SDL_Window;
struct SDL_Renderer;
union SDL_Event;

namespace superz80::frontend {

struct DisplayWindowSize {
    int width = 0;
    int height = 0;
};

constexpr unsigned int kDefaultDisplayScale = 2U;

bool is_supported_display_scale(unsigned int scale);
DisplayWindowSize window_size_for_display_scale(unsigned int scale);
unsigned int load_persisted_display_scale();
std::string load_persisted_rom_browser_directory();

struct FrontendConfig {
    const char* runtime_name = "";
    unsigned int initial_display_scale = kDefaultDisplayScale;
#if defined(SUPER_Z80_HAS_SDL)
    SDL_Window* window = nullptr;
    SDL_Renderer* renderer = nullptr;
#endif
};

class Frontend {
public:
    Frontend();

    bool initialize(const FrontendConfig& config);
    void shutdown();
    void handle_event(const SDL_Event& event);
    void begin_frame();
    void render();
    void end_frame();
    void set_runtime_control_state(const RuntimeControlState& state);
    RuntimeControlCommands consume_runtime_control_commands();

    bool is_initialized() const;
    bool is_ui_available() const;
    const std::string& runtime_name() const;
    unsigned int display_scale() const;

private:
    unsigned int sanitized_display_scale(unsigned int requested_scale) const;
    void apply_display_scale(unsigned int scale, bool persist);
    void persist_settings() const;

    bool initialized_;
    bool ui_initialized_;
    std::string runtime_name_;
    std::string imgui_ini_path_;
    std::string frontend_settings_path_;
    std::string persisted_rom_browser_directory_;
    unsigned int display_scale_ = kDefaultDisplayScale;
#if defined(SUPER_Z80_HAS_SDL)
    SDL_Window* window_ = nullptr;
    SDL_Renderer* renderer_ = nullptr;
#endif
    DebugPanelHost debug_panel_host_;
};

} // namespace superz80::frontend

#endif
