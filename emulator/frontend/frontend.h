#ifndef SUPER_Z80_FRONTEND_FRONTEND_H
#define SUPER_Z80_FRONTEND_FRONTEND_H

#include <string>

#include "debug_panel_host.h"

struct SDL_Window;
struct SDL_Renderer;
union SDL_Event;

namespace superz80::frontend {

struct FrontendConfig {
    const char* runtime_name = "";
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

    bool is_initialized() const;
    bool is_ui_available() const;
    const std::string& runtime_name() const;

private:
    bool initialized_;
    bool ui_initialized_;
    std::string runtime_name_;
#if defined(SUPER_Z80_HAS_SDL)
    SDL_Renderer* renderer_ = nullptr;
#endif
    DebugPanelHost debug_panel_host_;
};

} // namespace superz80::frontend

#endif
