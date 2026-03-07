#include "vblank.h"

namespace superz80 {

VBlank::VBlank(IRQController& irq)
    : irq_(irq),
      vblank_active_(false) {}

void VBlank::reset() {
    vblank_active_ = false;
}

void VBlank::update_scanline(uint32_t scanline) {
    if (scanline == 0U) {
        vblank_active_ = false;
        return;
    }

    if (scanline == kBeginScanline && !vblank_active_) {
        vblank_active_ = true;
        irq_.request(kIrqBit);
    }
}

bool VBlank::active() const {
    return vblank_active_;
}

} // namespace superz80
