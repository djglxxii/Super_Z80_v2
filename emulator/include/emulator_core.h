#ifndef SUPER_Z80_EMULATOR_CORE_H
#define SUPER_Z80_EMULATOR_CORE_H

#include <array>
#include <cstddef>
#include <cstdint>

#include "apu.h"
#include "bus.h"
#include "cpu.h"
#include "scheduler.h"

class EmulatorCore {
public:
    using AudioSample = superz80::APU::Sample;

    static constexpr uint32_t kAudioMasterClockHz = 3579545U;
    static constexpr uint32_t kVideoFramesPerSecond = 60U;
    static constexpr uint32_t kScanlinesPerSecond =
        superz80::Scheduler::kScanlinesPerFrame * kVideoFramesPerSecond;
    static constexpr std::size_t kAudioBufferCapacitySamples = 4096U;

    EmulatorCore();

    void initialize();
    void load_rom(const void* data, std::size_t size);
    void step_scanline();
    void step_scanlines(uint32_t count);
    superz80::Bus& bus();
    const superz80::Bus& bus() const;
    std::size_t available_audio_samples() const;
    std::size_t consume_audio_samples(AudioSample* destination, std::size_t max_samples);
    uint32_t frame() const;
    uint32_t scanline() const;

private:
    void reset_audio_state();
    void produce_scanline_audio();
    uint32_t next_audio_tick_count();
    void push_audio_sample(AudioSample sample);

    superz80::Bus bus_;
    superz80::CPU cpu_;
    superz80::Scheduler scheduler_;
    std::array<AudioSample, kAudioBufferCapacitySamples> audio_buffer_;
    std::size_t audio_read_index_;
    std::size_t audio_write_index_;
    std::size_t audio_buffer_size_;
    uint32_t scanline_sample_remainder_;
    uint32_t sample_tick_remainder_;
};

#endif
