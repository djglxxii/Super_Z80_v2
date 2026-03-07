#pragma once

#include <cstdint>

#include "irq_controller.h"

namespace superz80 {

class VBlank {
public:
    static constexpr uint32_t kBeginScanline = 192U;
    static constexpr uint8_t kIrqBit = 0x01U;

    explicit VBlank(IRQController& irq);

    void reset();
    void update_scanline(uint32_t scanline);

    bool active() const;

private:
    IRQController& irq_;
    bool vblank_active_;
};

} // namespace superz80
