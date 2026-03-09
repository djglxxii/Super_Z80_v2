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
