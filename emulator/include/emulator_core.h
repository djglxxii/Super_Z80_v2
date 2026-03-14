#ifndef SUPER_Z80_EMULATOR_CORE_H
#define SUPER_Z80_EMULATOR_CORE_H

#include <array>
#include <cstddef>
#include <cstdint>

#include "apu.h"
#include "bus.h"
#include "cpu.h"
#include "dma.h"
#include "scheduler.h"

class EmulatorCore {
public:
    using AudioSample = superz80::APU::Sample;
    using ApuSnapshot = superz80::APU::Snapshot;
    using BusSnapshot = superz80::Bus::Snapshot;
    using CpuSnapshot = superz80::CPU::RegisterSnapshot;
    using DmaSnapshot = superz80::DMA::Snapshot;
    using InputSnapshot = superz80::IO::Snapshot;
    using RomSnapshot = std::array<uint8_t, superz80::Bus::kRomSize>;
    using RamSnapshot = std::array<uint8_t, superz80::Bus::kRamWindowSize>;
    using VramSnapshot = std::array<uint8_t, superz80::VDP::kVramSize>;
    using FramebufferSnapshot = std::array<uint32_t, superz80::VDP::kFramebufferSize>;
    using Ym2151Snapshot = superz80::YM2151::Snapshot;
    struct SpriteSnapshot {
        uint8_t x = 0U;
        uint8_t y = 0U;
        uint8_t tile_index = 0U;
        uint8_t attributes = 0U;
    };
    using SpriteTableSnapshot = std::array<SpriteSnapshot, superz80::VDP::kMaxSprites>;
    struct AudioSnapshot {
        ApuSnapshot apu = {};
        Ym2151Snapshot ym2151 = {};
    };
    struct TimingSnapshot {
        uint32_t frame_counter = 0U;
        uint32_t scanline_counter = 0U;
        uint32_t scanlines_per_frame = 0U;
        uint32_t instructions_per_scanline = 0U;
        uint32_t ym2151_cycles_per_scanline = 0U;
        bool vblank_active = false;
        bool frame_ready = false;
        std::size_t buffered_audio_samples = 0U;
    };

    static constexpr uint32_t kAudioMasterClockHz = 3579545U;
    static constexpr uint32_t kVideoFramesPerSecond = 60U;
    static constexpr uint32_t kScanlinesPerSecond =
        superz80::Scheduler::kScanlinesPerFrame * kVideoFramesPerSecond;
    static constexpr std::size_t kAudioBufferCapacitySamples = 4096U;
    struct Snapshot {
        BusSnapshot bus = {};
        CpuSnapshot cpu = {};
        superz80::Scheduler::Snapshot scheduler = {};
        std::array<AudioSample, kAudioBufferCapacitySamples> audio_buffer = {};
        std::size_t audio_read_index = 0U;
        std::size_t audio_write_index = 0U;
        std::size_t audio_buffer_size = 0U;
        uint32_t scanline_sample_remainder = 0U;
        uint32_t sample_tick_remainder = 0U;
    };

    EmulatorCore();

    void initialize();
    void load_rom(const void* data, std::size_t size);
    void step_scanline();
    void step_scanlines(uint32_t count);
    superz80::Bus& bus();
    const superz80::Bus& bus() const;
    std::size_t available_audio_samples() const;
    std::size_t consume_audio_samples(AudioSample* destination, std::size_t max_samples);
    CpuSnapshot cpu_snapshot() const;
    DmaSnapshot dma_snapshot() const;
    InputSnapshot input_snapshot() const;
    AudioSnapshot audio_snapshot() const;
    TimingSnapshot timing_snapshot() const;
    Snapshot capture_snapshot() const;
    void restore_snapshot(const Snapshot& snapshot);
    RomSnapshot rom_snapshot() const;
    RamSnapshot ram_snapshot() const;
    VramSnapshot vram_snapshot() const;
    FramebufferSnapshot framebuffer_snapshot() const;
    SpriteTableSnapshot sprite_table_snapshot() const;
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
