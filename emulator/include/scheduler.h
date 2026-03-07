#pragma once

#include <cstdint>

#include "cpu.h"
#include "dma.h"
#include "vdp.h"
#include "vblank.h"

namespace superz80 {

class Scheduler {
public:
    static constexpr uint32_t kScanlinesPerFrame = 262U;
    static constexpr uint32_t kInstructionsPerScanline = 10U;

    Scheduler(CPU& cpu, VDP& vdp, VBlank& vblank, DMA& dma);

    void reset();
    void step_scanline();

    uint32_t frame() const;
    uint32_t scanline() const;

private:
    CPU& cpu_;
    VDP& vdp_;
    VBlank& vblank_;
    DMA& dma_;
    uint32_t frame_;
    uint32_t scanline_;
};

} // namespace superz80
