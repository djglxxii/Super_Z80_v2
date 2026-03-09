#include "scheduler.h"

namespace superz80 {

Scheduler::Scheduler(CPU& cpu, VDP& vdp, VBlank& vblank, DMA& dma, YM2151& ym2151)
    : cpu_(cpu),
      vdp_(vdp),
      vblank_(vblank),
      dma_(dma),
      ym2151_(ym2151),
      frame_(0U),
      scanline_(0U) {}

void Scheduler::reset() {
    frame_ = 0U;
    scanline_ = 0U;
    vdp_.reset();
    vblank_.reset();
}

void Scheduler::step_scanline() {
    for (uint32_t instruction = 0U; instruction < kInstructionsPerScanline; ++instruction) {
        cpu_.step();
    }

    dma_.step();
    ym2151_.tick(kYm2151CyclesPerScanline);

    ++scanline_;
    if (scanline_ == kScanlinesPerFrame) {
        scanline_ = 0U;
        ++frame_;
    }

    vblank_.update_scanline(scanline_);
    vdp_.step_scanline(scanline_);
}

uint32_t Scheduler::frame() const {
    return frame_;
}

uint32_t Scheduler::scanline() const {
    return scanline_;
}

} // namespace superz80
