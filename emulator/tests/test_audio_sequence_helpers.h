#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "emulator_core.h"

namespace superz80::testaudio {

inline void write_ym2151_register(Bus& bus, uint8_t reg, uint8_t value) {
    bus.write_port(Bus::kYm2151RegisterSelectPort, reg);
    bus.write_port(Bus::kYm2151RegisterDataPort, value);
}

inline void program_apu_tone(Bus& bus, uint16_t period, uint8_t volume, uint8_t control) {
    bus.write_port(Bus::kAudioToneALowPort, static_cast<uint8_t>(period & 0x00FFU));
    bus.write_port(Bus::kAudioToneAHighPort, static_cast<uint8_t>((period >> 8U) & 0x0FU));
    bus.write_port(Bus::kAudioVolumeAPort, volume);
    bus.write_port(Bus::kAudioControlPort, control);
}

inline void program_deterministic_ym2151_voice(Bus& bus) {
    write_ym2151_register(bus, 0x20U, 0x07U);
    write_ym2151_register(bus, 0x30U, 0xAAU);
    write_ym2151_register(bus, 0x38U, 0x0DU);
    write_ym2151_register(bus, 0x40U, 0x01U);
    write_ym2151_register(bus, 0x48U, 0x01U);
    write_ym2151_register(bus, 0x50U, 0x01U);
    write_ym2151_register(bus, 0x58U, 0x01U);
    write_ym2151_register(bus, 0x80U, 0xDFU);
    write_ym2151_register(bus, 0x88U, 0xDFU);
    write_ym2151_register(bus, 0x90U, 0xDFU);
    write_ym2151_register(bus, 0x98U, 0xDFU);
    write_ym2151_register(bus, 0xE0U, 0x0FU);
    write_ym2151_register(bus, 0xE8U, 0x0FU);
    write_ym2151_register(bus, 0xF0U, 0x0FU);
    write_ym2151_register(bus, 0xF8U, 0x0FU);
    write_ym2151_register(bus, 0x28U, 0xF0U);
}

inline std::vector<int16_t> collect_scheduler_audio_script(Scheduler& scheduler,
                                                           const std::vector<uint32_t>& sample_steps) {
    std::vector<int16_t> samples;
    samples.reserve(sample_steps.size());
    for (const uint32_t step : sample_steps) {
        scheduler.step_scanline();
        scheduler.step_audio_sample(step);
        samples.push_back(scheduler.current_audio_sample());
    }

    return samples;
}

inline std::vector<EmulatorCore::AudioSample> collect_core_audio(EmulatorCore& core,
                                                                 std::size_t scanlines) {
    for (std::size_t index = 0U; index < scanlines; ++index) {
        core.step_scanline();
    }

    std::vector<EmulatorCore::AudioSample> samples(core.available_audio_samples(), 0);
    const std::size_t copied = core.consume_audio_samples(samples.data(), samples.size());
    samples.resize(copied);
    return samples;
}

template <typename SampleType>
inline bool all_zero(const std::vector<SampleType>& samples) {
    for (const SampleType sample : samples) {
        if (sample != 0) {
            return false;
        }
    }

    return true;
}

} // namespace superz80::testaudio
