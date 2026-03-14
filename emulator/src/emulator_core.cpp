#include "emulator_core.h"

#include <algorithm>
#include <iostream>

namespace {

constexpr uint32_t kAudioSampleRateDivisor =
    32U * static_cast<uint32_t>(superz80::APU::kSampleRateHz);

} // namespace

EmulatorCore::EmulatorCore()
    : bus_(),
      cpu_(bus_),
      scheduler_(cpu_, bus_.vdp(), bus_.vblank(), bus_.dma(), bus_.apu(), bus_.ym2151()),
      audio_buffer_{},
      audio_read_index_(0U),
      audio_write_index_(0U),
      audio_buffer_size_(0U),
      scanline_sample_remainder_(0U),
      sample_tick_remainder_(0U) {}

void EmulatorCore::initialize() {
    bus_.reset();
    cpu_.reset();
    scheduler_.reset();
    reset_audio_state();
    std::cout << "Emulator core initialized (audio output ready)." << std::endl;
}

void EmulatorCore::load_rom(const void* data, std::size_t size) {
    bus_.load_rom(data, size);
}

void EmulatorCore::step_scanline() {
    scheduler_.step_scanline();
    produce_scanline_audio();
}

void EmulatorCore::step_scanlines(uint32_t count) {
    for (uint32_t scanline = 0U; scanline < count; ++scanline) {
        step_scanline();
    }
}

superz80::Bus& EmulatorCore::bus() {
    return bus_;
}

const superz80::Bus& EmulatorCore::bus() const {
    return bus_;
}

std::size_t EmulatorCore::available_audio_samples() const {
    return audio_buffer_size_;
}

std::size_t EmulatorCore::consume_audio_samples(AudioSample* destination, std::size_t max_samples) {
    if (destination == nullptr || max_samples == 0U) {
        return 0U;
    }

    const std::size_t samples_to_copy = std::min(max_samples, audio_buffer_size_);
    for (std::size_t index = 0U; index < samples_to_copy; ++index) {
        destination[index] = audio_buffer_[audio_read_index_];
        audio_read_index_ = (audio_read_index_ + 1U) % audio_buffer_.size();
    }

    audio_buffer_size_ -= samples_to_copy;
    return samples_to_copy;
}

EmulatorCore::CpuSnapshot EmulatorCore::cpu_snapshot() const {
    return cpu_.snapshot();
}

EmulatorCore::DmaSnapshot EmulatorCore::dma_snapshot() const {
    return bus_.dma().snapshot();
}

EmulatorCore::InputSnapshot EmulatorCore::input_snapshot() const {
    return bus_.controller_snapshot();
}

EmulatorCore::AudioSnapshot EmulatorCore::audio_snapshot() const {
    return {
        bus_.apu().snapshot(),
        bus_.ym2151().snapshot(),
    };
}

EmulatorCore::TimingSnapshot EmulatorCore::timing_snapshot() const {
    return {
        scheduler_.frame(),
        scheduler_.scanline(),
        superz80::Scheduler::kScanlinesPerFrame,
        superz80::Scheduler::kInstructionsPerScanline,
        superz80::Scheduler::kYm2151CyclesPerScanline,
        bus_.vblank().active(),
        bus_.vdp().frame_ready(),
        available_audio_samples(),
    };
}

EmulatorCore::Snapshot EmulatorCore::capture_snapshot() const {
    return {
        bus_.snapshot(),
        cpu_.snapshot(),
        scheduler_.snapshot(),
        audio_buffer_,
        audio_read_index_,
        audio_write_index_,
        audio_buffer_size_,
        scanline_sample_remainder_,
        sample_tick_remainder_,
    };
}

void EmulatorCore::restore_snapshot(const Snapshot& snapshot) {
    bus_.restore(snapshot.bus);
    cpu_.restore(snapshot.cpu);
    scheduler_.restore(snapshot.scheduler);
    audio_buffer_ = snapshot.audio_buffer;
    audio_read_index_ = snapshot.audio_read_index;
    audio_write_index_ = snapshot.audio_write_index;
    audio_buffer_size_ = snapshot.audio_buffer_size;
    scanline_sample_remainder_ = snapshot.scanline_sample_remainder;
    sample_tick_remainder_ = snapshot.sample_tick_remainder;
}

EmulatorCore::RomSnapshot EmulatorCore::rom_snapshot() const {
    RomSnapshot snapshot = {};
    for (std::size_t offset = 0U; offset < snapshot.size(); ++offset) {
        snapshot[offset] = bus_.read(static_cast<uint16_t>(superz80::Bus::kRomStart + offset));
    }
    return snapshot;
}

EmulatorCore::RamSnapshot EmulatorCore::ram_snapshot() const {
    RamSnapshot snapshot = {};
    for (std::size_t offset = 0U; offset < snapshot.size(); ++offset) {
        snapshot[offset] = bus_.read(static_cast<uint16_t>(superz80::Bus::kRamStart + offset));
    }
    return snapshot;
}

EmulatorCore::VramSnapshot EmulatorCore::vram_snapshot() const {
    VramSnapshot snapshot = {};
    for (std::size_t offset = 0U; offset < snapshot.size(); ++offset) {
        snapshot[offset] = bus_.vdp().vram(static_cast<uint16_t>(offset));
    }
    return snapshot;
}

EmulatorCore::SpriteTableSnapshot EmulatorCore::sprite_table_snapshot() const {
    SpriteTableSnapshot snapshot = {};
    for (std::size_t sprite_index = 0U; sprite_index < snapshot.size(); ++sprite_index) {
        const uint16_t sprite_base = static_cast<uint16_t>(
            superz80::VDP::kSpriteAttributeTableBase + (sprite_index * superz80::VDP::kSpriteSizeBytes));
        snapshot[sprite_index] = {
            bus_.vdp().vram(static_cast<uint16_t>(sprite_base + 1U)),
            bus_.vdp().vram(sprite_base),
            bus_.vdp().vram(static_cast<uint16_t>(sprite_base + 2U)),
            bus_.vdp().vram(static_cast<uint16_t>(sprite_base + 3U)),
        };
    }
    return snapshot;
}

uint32_t EmulatorCore::frame() const {
    return scheduler_.frame();
}

uint32_t EmulatorCore::scanline() const {
    return scheduler_.scanline();
}

void EmulatorCore::reset_audio_state() {
    audio_read_index_ = 0U;
    audio_write_index_ = 0U;
    audio_buffer_size_ = 0U;
    scanline_sample_remainder_ = 0U;
    sample_tick_remainder_ = 0U;
}

void EmulatorCore::produce_scanline_audio() {
    scanline_sample_remainder_ += superz80::APU::kSampleRateHz;
    while (scanline_sample_remainder_ >= kScanlinesPerSecond) {
        scanline_sample_remainder_ -= kScanlinesPerSecond;
        scheduler_.step_audio_sample(next_audio_tick_count());
        const AudioSample sample = scheduler_.current_audio_sample();
        push_audio_sample(sample);
    }
}

uint32_t EmulatorCore::next_audio_tick_count() {
    sample_tick_remainder_ += kAudioMasterClockHz;
    const uint32_t tick_count = sample_tick_remainder_ / kAudioSampleRateDivisor;
    sample_tick_remainder_ %= kAudioSampleRateDivisor;
    return tick_count;
}

void EmulatorCore::push_audio_sample(AudioSample sample) {
    if (audio_buffer_size_ == audio_buffer_.size()) {
        bus_.apu().flag_sample_queue_overrun();
        return;
    }

    audio_buffer_[audio_write_index_] = sample;
    audio_write_index_ = (audio_write_index_ + 1U) % audio_buffer_.size();
    ++audio_buffer_size_;
}
