#include "frontend.h"

namespace superz80::frontend {

Frontend::Frontend()
    : initialized_(false) {
}

bool Frontend::initialize(const FrontendConfig& config) {
    if (initialized_) {
        return true;
    }

    runtime_name_ = config.runtime_name != nullptr ? config.runtime_name : "";
    debug_panel_host_.initialize();
    initialized_ = true;
    return true;
}

void Frontend::shutdown() {
    if (!initialized_) {
        return;
    }

    debug_panel_host_.shutdown();
    runtime_name_.clear();
    initialized_ = false;
}

void Frontend::begin_frame() {
    if (!initialized_) {
        return;
    }

    debug_panel_host_.begin_frame();
}

void Frontend::end_frame() {
    if (!initialized_) {
        return;
    }

    debug_panel_host_.end_frame();
}

bool Frontend::is_initialized() const {
    return initialized_;
}

const std::string& Frontend::runtime_name() const {
    return runtime_name_;
}

} // namespace superz80::frontend
