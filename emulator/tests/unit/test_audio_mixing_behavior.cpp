#include "apu.h"

#include <array>
#include <cstdint>
#include <iostream>

namespace {

constexpr std::array<int16_t, 16> kVolumeTable = {
    15, 14, 13, 12,
    11, 10, 9,  8,
    7,  6,  5,  4,
    3,  2,  1,  0
};

bool expect_equal_i16(const char* name, int16_t actual, int16_t expected) {
    if (actual != expected) {
        std::cerr << "[FAIL] " << name << " expected=" << expected
                  << " actual=" << actual << std::endl;
        return false;
    }

    std::cout << "[PASS] " << name << " value=" << actual << std::endl;
    return true;
}

int16_t expected_pcm(int32_t sum) {
    return static_cast<int16_t>((sum * 32767) / 60);
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

    for (uint8_t register_value = 0U; register_value <= 0x0FU; ++register_value) {
        superz80::APU apu;
        enable_audio(apu);
        set_tone(apu, superz80::APU::kToneALowPort, superz80::APU::kToneAHighPort,
                 superz80::APU::kVolumeAPort, 0x0001U, register_value);
        apu.advance_audio_ticks(1U);
        ok = expect_equal_i16("volume-table-maps-register-to-expected-pcm",
                              apu.generate_current_sample(),
                              expected_pcm(kVolumeTable[register_value])) && ok;
    }

    superz80::APU mixed;
    enable_audio(mixed);
    set_tone(mixed, superz80::APU::kToneALowPort, superz80::APU::kToneAHighPort, superz80::APU::kVolumeAPort, 0x0001U, 0x00U);
    set_tone(mixed, superz80::APU::kToneBLowPort, superz80::APU::kToneBHighPort, superz80::APU::kVolumeBPort, 0x0001U, 0x01U);
    set_tone(mixed, superz80::APU::kToneCLowPort, superz80::APU::kToneCHighPort, superz80::APU::kVolumeCPort, 0x0001U, 0x02U);
    mixed.write_port(superz80::APU::kNoiseControlPort, 0x00U);
    mixed.write_port(superz80::APU::kVolumeNoisePort, 0x03U);
    mixed.advance_audio_ticks(1U);
    ok = expect_equal_i16("multiple-channels-mix-with-equal-weighting",
                          mixed.generate_current_sample(),
                          expected_pcm(15 + 14 + 13 - 12)) && ok;

    superz80::APU negative_peak;
    enable_audio(negative_peak);
    set_tone(negative_peak, superz80::APU::kToneALowPort, superz80::APU::kToneAHighPort, superz80::APU::kVolumeAPort, 0x0001U, 0x00U);
    set_tone(negative_peak, superz80::APU::kToneBLowPort, superz80::APU::kToneBHighPort, superz80::APU::kVolumeBPort, 0x0001U, 0x00U);
    set_tone(negative_peak, superz80::APU::kToneCLowPort, superz80::APU::kToneCHighPort, superz80::APU::kVolumeCPort, 0x0001U, 0x00U);
    negative_peak.write_port(superz80::APU::kNoiseControlPort, 0x00U);
    negative_peak.write_port(superz80::APU::kVolumeNoisePort, 0x00U);
    ok = expect_equal_i16("all-negative-contributions-scale-to-full-negative-range",
                          negative_peak.generate_current_sample(),
                          expected_pcm(-60)) && ok;
    negative_peak.advance_audio_ticks(1U);
    ok = expect_equal_i16("all-positive-contributions-scale-to-full-positive-range",
                          negative_peak.generate_current_sample(),
                          expected_pcm(30)) && ok;

    superz80::APU noise_only;
    enable_audio(noise_only);
    noise_only.write_port(superz80::APU::kNoiseControlPort, 0x00U);
    noise_only.write_port(superz80::APU::kVolumeNoisePort, 0x02U);
    ok = expect_equal_i16("noise-channel-contributes-using-current-output-bit",
                          noise_only.generate_current_sample(),
                          expected_pcm(-13)) && ok;
    noise_only.advance_audio_ticks(16U);
    ok = expect_equal_i16("noise-channel-shift-updates-signed-contribution",
                          noise_only.generate_current_sample(),
                          expected_pcm(-13)) && ok;

    superz80::APU silent;
    silent.write_port(superz80::APU::kControlPort, 0x00U);
    ok = expect_equal_i16("all-channels-disabled-remain-silent", silent.generate_current_sample(), 0) && ok;
    silent.write_port(superz80::APU::kControlPort, 0x01U);
    silent.write_port(superz80::APU::kVolumeAPort, 0x0FU);
    silent.write_port(superz80::APU::kVolumeBPort, 0x0FU);
    silent.write_port(superz80::APU::kVolumeCPort, 0x0FU);
    silent.write_port(superz80::APU::kVolumeNoisePort, 0x0FU);
    ok = expect_equal_i16("all-channels-at-silent-volume-remain-silent", silent.generate_current_sample(), 0) && ok;
    silent.write_port(superz80::APU::kControlPort, 0x03U);
    ok = expect_equal_i16("mute-overrides-mixer-output", silent.generate_current_sample(), 0) && ok;

    return ok ? 0 : 1;
}
