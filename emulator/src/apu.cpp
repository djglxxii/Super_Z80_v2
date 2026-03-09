#include "apu.h"

#include <array>
#include <algorithm>

namespace superz80 {

namespace {

constexpr std::array<int16_t, 16> kVolumeTable = {
    15, 14, 13, 12,
    11, 10, 9,  8,
    7,  6,  5,  4,
    3,  2,  1,  0
};

constexpr int32_t kMaxMixedLevel = 60;
constexpr int32_t kPcmPositivePeak = 32767;
constexpr int32_t kPcmNegativePeak = -32768;

} // namespace

APU::APU()
    : registers_(),
      overrun_(false),
      tone_runtime_{},
      noise_runtime_{},
      sample_output_state_{} {
    reset();
}

void APU::reset() {
    registers_ = APURegisterState{};
    reset_runtime_state();
}

uint8_t APU::read_port(uint8_t port) const {
    switch (port) {
    case kToneALowPort:
        return read_tone_low(kToneAChannel);
    case kToneAHighPort:
        return read_tone_high(kToneAChannel);
    case kToneBLowPort:
        return read_tone_low(kToneBChannel);
    case kToneBHighPort:
        return read_tone_high(kToneBChannel);
    case kToneCLowPort:
        return read_tone_low(kToneCChannel);
    case kToneCHighPort:
        return read_tone_high(kToneCChannel);
    case kNoiseControlPort:
        return registers_.noise_control;
    case kVolumeAPort:
        return volume(kToneAChannel);
    case kVolumeBPort:
        return volume(kToneBChannel);
    case kVolumeCPort:
        return volume(kToneCChannel);
    case kVolumeNoisePort:
        return volume(kNoiseChannel);
    case kControlPort:
        return control();
    default:
        return 0x00U;
    }
}

void APU::write_port(uint8_t port, uint8_t value) {
    switch (port) {
    case kToneALowPort:
        write_tone_low(kToneAChannel, value);
        break;
    case kToneAHighPort:
        write_tone_high(kToneAChannel, value);
        break;
    case kToneBLowPort:
        write_tone_low(kToneBChannel, value);
        break;
    case kToneBHighPort:
        write_tone_high(kToneBChannel, value);
        break;
    case kToneCLowPort:
        write_tone_low(kToneCChannel, value);
        break;
    case kToneCHighPort:
        write_tone_high(kToneCChannel, value);
        break;
    case kNoiseControlPort:
        registers_.noise_control = static_cast<uint8_t>(value & APURegisterState::kNoiseControlMask);
        reload_noise_divider();
        break;
    case kVolumeAPort:
        write_volume(kToneAChannel, value);
        break;
    case kVolumeBPort:
        write_volume(kToneBChannel, value);
        break;
    case kVolumeCPort:
        write_volume(kToneCChannel, value);
        break;
    case kVolumeNoisePort:
        write_volume(kNoiseChannel, value);
        break;
    case kControlPort: {
        const uint8_t masked = static_cast<uint8_t>(value & APURegisterState::kControlWritableMask);
        registers_.control = static_cast<uint8_t>(masked & APURegisterState::kControlReadableMask);
        if ((masked & APURegisterState::kControlResetStateBit) != 0U) {
            reset_runtime_state();
        }
        break;
    }
    default:
        break;
    }
}

void APU::advance_audio_ticks(uint32_t tick_count) {
    for (uint32_t tick = 0U; tick < tick_count; ++tick) {
        step_tone(kToneAChannel);
        step_tone(kToneBChannel);
        const bool tone_c_event = step_tone(kToneCChannel);
        step_noise(tone_c_event);
    }
}

APU::Sample APU::generate_current_sample() {
    sample_output_state_.current_sample = mix_current_sample();
    return sample_output_state_.current_sample;
}

APU::Sample APU::advance_and_generate_sample(uint32_t tick_count) {
    advance_audio_ticks(tick_count);
    return generate_current_sample();
}

void APU::flag_sample_queue_overrun() {
    overrun_ = true;
}

uint16_t APU::tone_period(std::size_t channel) const {
    return static_cast<uint16_t>(
        (static_cast<uint16_t>(registers_.tone[channel].period_high & APURegisterState::kToneHighMask) << 8U) |
        registers_.tone[channel].period_low);
}

uint8_t APU::volume(std::size_t channel) const {
    return registers_.volume[channel];
}

uint8_t APU::noise_control() const {
    return registers_.noise_control;
}

uint8_t APU::control() const {
    return static_cast<uint8_t>(registers_.control |
                                (overrun_ ? APURegisterState::kControlOverrunBit : 0x00U));
}

bool APU::overrun() const {
    return overrun_;
}

const APUToneRuntimeState& APU::tone_runtime_state(std::size_t channel) const {
    return tone_runtime_[channel];
}

const APUNoiseRuntimeState& APU::noise_runtime_state() const {
    return noise_runtime_;
}

const APUSampleOutputState& APU::sample_output_state() const {
    return sample_output_state_;
}

uint16_t APU::noise_lfsr() const {
    return noise_runtime_.lfsr;
}

uint8_t APU::noise_output_bit() const {
    return static_cast<uint8_t>(noise_runtime_.lfsr & 0x0001U);
}

void APU::reset_runtime_state() {
    overrun_ = false;
    for (std::size_t channel = 0U; channel < APURegisterState::kToneChannelCount; ++channel) {
        tone_runtime_[channel].phase_high = false;
        reload_tone_divider(channel);
    }

    noise_runtime_.lfsr = kNoiseLfsrSeed;
    reload_noise_divider();
    sample_output_state_.current_sample = 0;
}

void APU::reload_tone_divider(std::size_t channel) {
    const uint16_t period = tone_period(channel);
    tone_runtime_[channel].divider_counter = period;
    if (period == 0U) {
        tone_runtime_[channel].phase_high = false;
    }
}

void APU::reload_noise_divider() {
    noise_runtime_.divider_counter = noise_divider_period();
}

bool APU::step_tone(std::size_t channel) {
    const uint16_t period = tone_period(channel);
    if (period == 0U) {
        tone_runtime_[channel].divider_counter = 0U;
        tone_runtime_[channel].phase_high = false;
        return false;
    }

    if (tone_runtime_[channel].divider_counter > 1U) {
        --tone_runtime_[channel].divider_counter;
        return false;
    }

    tone_runtime_[channel].phase_high = !tone_runtime_[channel].phase_high;
    tone_runtime_[channel].divider_counter = period;
    return true;
}

void APU::step_noise(bool tone_c_event) {
    if ((registers_.noise_control & 0x03U) == 0x03U) {
        if (tone_c_event) {
            clock_noise_shift();
        }
        return;
    }

    if (noise_runtime_.divider_counter > 1U) {
        --noise_runtime_.divider_counter;
        return;
    }

    clock_noise_shift();
    reload_noise_divider();
}

void APU::clock_noise_shift() {
    const uint16_t current = noise_runtime_.lfsr;
    if ((registers_.noise_control & 0x04U) == 0U) {
        const uint16_t feedback = static_cast<uint16_t>(((current >> 0U) ^ (current >> 1U)) & 0x0001U);
        noise_runtime_.lfsr = static_cast<uint16_t>((current >> 1U) | (feedback << 14U));
        return;
    }

    // Periodic noise advances by shifting toward zero and re-seeding when the cycle completes.
    const uint16_t shifted = static_cast<uint16_t>(current >> 1U);
    noise_runtime_.lfsr = (shifted == 0U) ? kNoiseLfsrSeed : shifted;
}

uint16_t APU::noise_divider_period() const {
    switch (registers_.noise_control & 0x03U) {
    case 0x00U:
        return 16U;
    case 0x01U:
        return 32U;
    case 0x02U:
        return 64U;
    case 0x03U:
        return 0U;
    default:
        return 16U;
    }
}

bool APU::audio_enabled() const {
    return (registers_.control & 0x01U) != 0U;
}

bool APU::audio_muted() const {
    return (registers_.control & 0x02U) != 0U;
}

int16_t APU::volume_level(std::size_t channel) const {
    return kVolumeTable[registers_.volume[channel] & APURegisterState::kVolumeMask];
}

int16_t APU::tone_contribution(std::size_t channel) const {
    const int16_t level = volume_level(channel);
    if (level == 0 || tone_period(channel) == 0U) {
        return 0;
    }

    return tone_runtime_[channel].phase_high ? level : static_cast<int16_t>(-level);
}

int16_t APU::noise_contribution() const {
    const int16_t level = volume_level(kNoiseChannel);
    if (level == 0) {
        return 0;
    }

    return (noise_output_bit() != 0U) ? level : static_cast<int16_t>(-level);
}

APU::Sample APU::mix_current_sample() const {
    if (!audio_enabled() || audio_muted()) {
        return 0;
    }

    const int32_t sum = static_cast<int32_t>(tone_contribution(kToneAChannel)) +
                        static_cast<int32_t>(tone_contribution(kToneBChannel)) +
                        static_cast<int32_t>(tone_contribution(kToneCChannel)) +
                        static_cast<int32_t>(noise_contribution());
    const int32_t scaled = (sum * kPcmPositivePeak) / kMaxMixedLevel;
    return static_cast<Sample>(std::clamp(scaled, kPcmNegativePeak, kPcmPositivePeak));
}

uint8_t APU::read_tone_low(std::size_t channel) const {
    return registers_.tone[channel].period_low;
}

uint8_t APU::read_tone_high(std::size_t channel) const {
    return static_cast<uint8_t>(registers_.tone[channel].period_high & APURegisterState::kToneHighMask);
}

void APU::write_tone_low(std::size_t channel, uint8_t value) {
    registers_.tone[channel].period_low = value;
    reload_tone_divider(channel);
}

void APU::write_tone_high(std::size_t channel, uint8_t value) {
    registers_.tone[channel].period_high = static_cast<uint8_t>(value & APURegisterState::kToneHighMask);
    reload_tone_divider(channel);
}

void APU::write_volume(std::size_t channel, uint8_t value) {
    registers_.volume[channel] = static_cast<uint8_t>(value & APURegisterState::kVolumeMask);
}

} // namespace superz80
