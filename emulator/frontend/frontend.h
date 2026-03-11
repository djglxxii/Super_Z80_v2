#ifndef SUPER_Z80_FRONTEND_FRONTEND_H
#define SUPER_Z80_FRONTEND_FRONTEND_H

#include <string>

#include "debug_panel_host.h"

namespace superz80::frontend {

struct FrontendConfig {
    const char* runtime_name = "";
};

class Frontend {
public:
    Frontend();

    bool initialize(const FrontendConfig& config);
    void shutdown();
    void begin_frame();
    void end_frame();

    bool is_initialized() const;
    const std::string& runtime_name() const;

private:
    bool initialized_;
    std::string runtime_name_;
    DebugPanelHost debug_panel_host_;
};

} // namespace superz80::frontend

#endif
