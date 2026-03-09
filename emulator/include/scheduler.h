#pragma once

#include <cstdint>

#include "audio_mixer.h"
#include "cpu.h"
#include "dma.h"
#include "apu.h"
#include "vdp.h"
#include "vblank.h"
#include "ym2151.h"

namespace superz80 {

class Scheduler {
public:
    static constexpr uint32_t kScanlinesPerFrame = 262U;
    static constexpr uint32_t kInstructionsPerScanline = 10U;
    static constexpr uint32_t kYm2151CyclesPerScanline = 10U;

    Scheduler(CPU& cpu, VDP& vdp, VBlank& vblank, DMA& dma, APU& apu, YM2151& ym2151);

    void reset();
    void step_scanline();
    void step_audio_sample(uint32_t tick_count);

    uint32_t frame() const;
    uint32_t scanline() const;
    int16_t current_audio_sample() const;

private:
    CPU& cpu_;
    VDP& vdp_;
    VBlank& vblank_;
    DMA& dma_;
    APU& apu_;
    YM2151& ym2151_;
    AudioMixer audio_mixer_;
    uint32_t frame_;
    uint32_t scanline_;
};

} // namespace superz80
