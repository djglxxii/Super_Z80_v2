#include "apu.h"

#include <array>
#include <cstdint>
#include <iostream>

namespace {

constexpr int32_t kMixerScale = 32767;
constexpr int32_t kMixerDenominator = 60;

bool expect_equal_i16(const char* name, int16_t actual, int16_t expected) {
    if (actual != expected) {
        std::cerr << "[FAIL] " << name << " expected=" << expected
                  << " actual=" << actual << std::endl;
        return false;
    }

    std::cout << "[PASS] " << name << " value=" << actual << std::endl;
    return true;
}

bool expect_true(const char* name, bool value) {
    if (!value) {
        std::cerr << "[FAIL] " << name << " expected=true actual=false" << std::endl;
        return false;
    }

    std::cout << "[PASS] " << name << " value=true" << std::endl;
    return true;
}

int16_t expected_pcm(int32_t sum) {
    int32_t value = (sum * kMixerScale) / kMixerDenominator;
    if (value > 32767) {
        value = 32767;
    } else if (value < -32768) {
        value = -32768;
    }

    return static_cast<int16_t>(value);
}

void enable_audio(superz80::APU& apu) {
    apu.write_port(superz80::APU::kControlPort, 0x01U);
}

void set_tone(superz80::APU& apu, uint8_t low_port, uint8_t high_port, uint8_t volume_port,
              uint16_t period, uint8_t volume) {
    apu.write_port(low_port, static_cast<uint8_t>(period & 0x00FFU));
    apu.write_port(high_port, static_cast<uint8_t>((period >> 8U) & 0x0FU));
    apu.write_port(volume_port, volume);
}

} // namespace

int main() {
    bool ok = true;

    superz80::APU apu;
    ok = expect_equal_i16("reset-generate-current-sample-silence", apu.generate_current_sample(), 0) && ok;
    ok = expect_equal_i16("reset-sample-state-cleared", apu.sample_output_state().current_sample, 0) && ok;

    apu.reset();
    enable_audio(apu);
    set_tone(apu, superz80::APU::kToneALowPort, superz80::APU::kToneAHighPort, superz80::APU::kVolumeAPort, 0x0001U, 0x00U);
    ok = expect_equal_i16("single-tone-advance-and-generate-sample",
                          apu.advance_and_generate_sample(1U), expected_pcm(15)) && ok;

    apu.reset();
    enable_audio(apu);
    set_tone(apu, superz80::APU::kToneALowPort, superz80::APU::kToneAHighPort, superz80::APU::kVolumeAPort, 0x0001U, 0x00U);
    apu.advance_audio_ticks(1U);
    const int16_t loud_sample = apu.generate_current_sample();
    apu.write_port(superz80::APU::kVolumeAPort, 0x0EU);
    const int16_t quiet_sample = apu.generate_current_sample();
    ok = expect_equal_i16("volume-max-level-applies-lookup", loud_sample, expected_pcm(15)) && ok;
    ok = expect_equal_i16("volume-low-level-applies-lookup", quiet_sample, expected_pcm(1)) && ok;
    ok = expect_true("volume-change-reduces-amplitude", quiet_sample < loud_sample) && ok;

    apu.reset();
    enable_audio(apu);
    apu.write_port(superz80::APU::kNoiseControlPort, 0x04U);
    apu.write_port(superz80::APU::kVolumeNoisePort, 0x00U);
    ok = expect_equal_i16("noise-initial-negative-contribution", apu.generate_current_sample(), expected_pcm(-15)) && ok;
    apu.advance_audio_ticks(14U * 16U);
    ok = expect_equal_i16("noise-shifted-positive-contribution", apu.generate_current_sample(), expected_pcm(15)) && ok;

    apu.reset();
    enable_audio(apu);
    set_tone(apu, superz80::APU::kToneALowPort, superz80::APU::kToneAHighPort, superz80::APU::kVolumeAPort, 0x0001U, 0x00U);
    set_tone(apu, superz80::APU::kToneBLowPort, superz80::APU::kToneBHighPort, superz80::APU::kVolumeBPort, 0x0001U, 0x05U);
    apu.write_port(superz80::APU::kNoiseControlPort, 0x04U);
    apu.write_port(superz80::APU::kVolumeNoisePort, 0x0EU);
    apu.advance_audio_ticks(225U);
    ok = expect_equal_i16("channel-combination-mixes-summed-levels",
                          apu.generate_current_sample(), expected_pcm(15 + 10 + 1)) && ok;

    apu.reset();
    enable_audio(apu);
    set_tone(apu, superz80::APU::kToneALowPort, superz80::APU::kToneAHighPort, superz80::APU::kVolumeAPort, 0x0001U, 0x00U);
    set_tone(apu, superz80::APU::kToneBLowPort, superz80::APU::kToneBHighPort, superz80::APU::kVolumeBPort, 0x0001U, 0x00U);
    set_tone(apu, superz80::APU::kToneCLowPort, superz80::APU::kToneCHighPort, superz80::APU::kVolumeCPort, 0x0001U, 0x00U);
    apu.write_port(superz80::APU::kNoiseControlPort, 0x04U);
    apu.write_port(superz80::APU::kVolumeNoisePort, 0x00U);
    apu.advance_audio_ticks(225U);
    ok = expect_equal_i16("maximum-positive-mix-reaches-full-scale",
                          apu.generate_current_sample(), 32767) && ok;

    superz80::APU repeat_a;
    superz80::APU repeat_b;
    enable_audio(repeat_a);
    enable_audio(repeat_b);
    set_tone(repeat_a, superz80::APU::kToneALowPort, superz80::APU::kToneAHighPort, superz80::APU::kVolumeAPort, 0x0002U, 0x00U);
    set_tone(repeat_b, superz80::APU::kToneALowPort, superz80::APU::kToneAHighPort, superz80::APU::kVolumeAPort, 0x0002U, 0x00U);
    repeat_a.write_port(superz80::APU::kNoiseControlPort, 0x00U);
    repeat_b.write_port(superz80::APU::kNoiseControlPort, 0x00U);
    repeat_a.write_port(superz80::APU::kVolumeNoisePort, 0x0DU);
    repeat_b.write_port(superz80::APU::kVolumeNoisePort, 0x0DU);
    const std::array<uint32_t, 5> kSteps = {1U, 3U, 16U, 2U, 32U};
    for (std::size_t index = 0; index < kSteps.size(); ++index) {
        const int16_t sample_a = repeat_a.advance_and_generate_sample(kSteps[index]);
        const int16_t sample_b = repeat_b.advance_and_generate_sample(kSteps[index]);
        ok = expect_equal_i16("repeatability-sample-sequence", sample_a, sample_b) && ok;
    }

    apu.reset();
    enable_audio(apu);
    set_tone(apu, superz80::APU::kToneALowPort, superz80::APU::kToneAHighPort, superz80::APU::kVolumeAPort, 0x0001U, 0x00U);
    apu.advance_audio_ticks(1U);
    ok = expect_equal_i16("mute-precondition-non-silent", apu.generate_current_sample(), expected_pcm(15)) && ok;
    apu.write_port(superz80::APU::kControlPort, 0x03U);
    ok = expect_equal_i16("mute-forces-silence", apu.generate_current_sample(), 0) && ok;
    apu.write_port(superz80::APU::kControlPort, 0x01U);
    ok = expect_equal_i16("unmute-restores-current-mix", apu.generate_current_sample(), expected_pcm(15)) && ok;
    apu.write_port(superz80::APU::kControlPort, 0x04U);
    ok = expect_equal_i16("reset-state-clears-sample-output", apu.sample_output_state().current_sample, 0) && ok;

    return ok ? 0 : 1;
}
