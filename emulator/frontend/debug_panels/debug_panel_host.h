#ifndef SUPER_Z80_FRONTEND_DEBUG_PANEL_HOST_H
#define SUPER_Z80_FRONTEND_DEBUG_PANEL_HOST_H

namespace superz80::frontend {

class DebugPanelHost {
public:
    void initialize();
    void shutdown();
    void begin_frame();
    void end_frame();
};

} // namespace superz80::frontend

#endif
