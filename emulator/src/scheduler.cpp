#include "scheduler.h"

namespace superz80 {

Scheduler::Scheduler(CPU& cpu, VDP& vdp, VBlank& vblank, DMA& dma, APU& apu, YM2151& ym2151)
    : cpu_(cpu),
      vdp_(vdp),
      vblank_(vblank),
      dma_(dma),
      apu_(apu),
      ym2151_(ym2151),
      audio_mixer_(apu_, ym2151_),
      frame_(0U),
      scanline_(0U) {}

void Scheduler::reset() {
    frame_ = 0U;
    scanline_ = 0U;
    audio_mixer_.reset();
    vdp_.reset();
    vblank_.reset();
}

void Scheduler::step_scanline() {
    for (uint32_t instruction = 0U; instruction < kInstructionsPerScanline; ++instruction) {
        cpu_.step();
    }

    dma_.step();
    ym2151_.tick(kYm2151CyclesPerScanline);
    audio_mixer_.tick();

    ++scanline_;
    if (scanline_ == kScanlinesPerFrame) {
        scanline_ = 0U;
        ++frame_;
    }

    vblank_.update_scanline(scanline_);
    vdp_.step_scanline(scanline_);
}

void Scheduler::step_audio_sample(uint32_t tick_count) {
    apu_.advance_and_generate_sample(tick_count);
    audio_mixer_.tick();
}

uint32_t Scheduler::frame() const {
    return frame_;
}

uint32_t Scheduler::scanline() const {
    return scanline_;
}

int16_t Scheduler::current_audio_sample() const {
    return audio_mixer_.current_sample();
}

Scheduler::Snapshot Scheduler::snapshot() const {
    return {audio_mixer_.snapshot(), frame_, scanline_};
}

void Scheduler::restore(const Snapshot& snapshot) {
    audio_mixer_.restore(snapshot.audio_mixer);
    frame_ = snapshot.frame;
    scanline_ = snapshot.scanline;
}

} // namespace superz80
