#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "apu_registers.h"

namespace superz80 {

class AudioSampleSource {
public:
    virtual ~AudioSampleSource() = default;
    virtual int16_t current_sample() const = 0;
};

class APU : public AudioSampleSource {
public:
    using Sample = int16_t;
    struct ToneChannelSnapshot {
        uint16_t period = 0U;
        uint8_t volume = 0U;
        uint16_t divider_counter = 0U;
        bool phase_high = false;
    };
    struct NoiseChannelSnapshot {
        uint8_t control = 0U;
        uint8_t volume = 0U;
        uint16_t divider_counter = 0U;
        uint16_t lfsr = 0U;
        uint8_t output_bit = 0U;
    };
    struct Snapshot {
        std::array<uint8_t, 12U> registers = {};
        std::array<ToneChannelSnapshot, 3U> tone_channels = {};
        NoiseChannelSnapshot noise = {};
        uint8_t control = 0U;
        bool enabled = false;
        bool muted = false;
        bool overrun = false;
        int16_t current_sample = 0;
    };

    static constexpr uint8_t kToneALowPort = 0xD0U;
    static constexpr uint8_t kToneAHighPort = 0xD1U;
    static constexpr uint8_t kToneBLowPort = 0xD2U;
    static constexpr uint8_t kToneBHighPort = 0xD3U;
    static constexpr uint8_t kToneCLowPort = 0xD4U;
    static constexpr uint8_t kToneCHighPort = 0xD5U;
    static constexpr uint8_t kNoiseControlPort = 0xD6U;
    static constexpr uint8_t kVolumeAPort = 0xD7U;
    static constexpr uint8_t kVolumeBPort = 0xD8U;
    static constexpr uint8_t kVolumeCPort = 0xD9U;
    static constexpr uint8_t kVolumeNoisePort = 0xDAU;
    static constexpr uint8_t kControlPort = 0xDBU;
    static constexpr uint8_t kPortStart = kToneALowPort;
    static constexpr uint8_t kPortEnd = kControlPort;
    static constexpr uint32_t kSampleRateHz = 48000U;
    static constexpr uint8_t kChannelCount = 4U;

    APU();

    void reset();

    uint8_t read_port(uint8_t port) const;
    void write_port(uint8_t port, uint8_t value);
    void advance_audio_ticks(uint32_t tick_count);
    Sample generate_current_sample();
    Sample advance_and_generate_sample(uint32_t tick_count);
    void flag_sample_queue_overrun();
    int16_t current_sample() const override;

    uint16_t tone_period(std::size_t channel) const;
    uint8_t volume(std::size_t channel) const;
    uint8_t noise_control() const;
    uint8_t control() const;
    bool overrun() const;
    const APUToneRuntimeState& tone_runtime_state(std::size_t channel) const;
    const APUNoiseRuntimeState& noise_runtime_state() const;
    const APUSampleOutputState& sample_output_state() const;
    uint16_t noise_lfsr() const;
    uint8_t noise_output_bit() const;
    Snapshot snapshot() const;
    void restore(const Snapshot& snapshot);

private:
    static constexpr std::size_t kToneAChannel = 0U;
    static constexpr std::size_t kToneBChannel = 1U;
    static constexpr std::size_t kToneCChannel = 2U;
    static constexpr std::size_t kNoiseChannel = 3U;
    static constexpr uint16_t kNoiseLfsrSeed = 0x4000U;

    void reset_runtime_state();
    void reload_tone_divider(std::size_t channel);
    void reload_noise_divider();
    bool step_tone(std::size_t channel);
    void step_noise(bool tone_c_event);
    void clock_noise_shift();
    uint16_t noise_divider_period() const;
    bool audio_enabled() const;
    bool audio_muted() const;
    int16_t volume_level(std::size_t channel) const;
    int16_t tone_contribution(std::size_t channel) const;
    int16_t noise_contribution() const;
    Sample mix_current_sample() const;
    uint8_t read_tone_low(std::size_t channel) const;
    uint8_t read_tone_high(std::size_t channel) const;
    void write_tone_low(std::size_t channel, uint8_t value);
    void write_tone_high(std::size_t channel, uint8_t value);
    void write_volume(std::size_t channel, uint8_t value);

    APURegisterState registers_;
    bool overrun_;
    APUToneRuntimeState tone_runtime_[APURegisterState::kToneChannelCount];
    APUNoiseRuntimeState noise_runtime_;
    APUSampleOutputState sample_output_state_;
};

} // namespace superz80
