#ifndef SUPER_Z80_FRONTEND_DEBUG_PANEL_HOST_H
#define SUPER_Z80_FRONTEND_DEBUG_PANEL_HOST_H

#include <string>

namespace superz80::frontend {

class DebugPanelHost {
public:
    void initialize();
    void shutdown();
    void begin_frame();
    void render(const std::string& runtime_name);
    void end_frame();

private:
    bool initialized_ = false;
};

} // namespace superz80::frontend

#endif
