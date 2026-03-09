#include "apu.h"

#include <array>
#include <cstdint>
#include <iostream>

namespace {

bool expect_equal_u8(const char* name, uint8_t actual, uint8_t expected) {
    if (actual != expected) {
        std::cerr << "[FAIL] " << name << " expected=0x" << std::hex << std::uppercase
                  << static_cast<unsigned int>(expected) << " actual=0x"
                  << static_cast<unsigned int>(actual) << std::dec << std::nouppercase
                  << std::endl;
        return false;
    }

    std::cout << "[PASS] " << name << " value=0x" << std::hex << std::uppercase
              << static_cast<unsigned int>(actual) << std::dec << std::nouppercase
              << std::endl;
    return true;
}

bool expect_equal_u16(const char* name, uint16_t actual, uint16_t expected) {
    if (actual != expected) {
        std::cerr << "[FAIL] " << name << " expected=0x" << std::hex << std::uppercase
                  << static_cast<unsigned int>(expected) << " actual=0x"
                  << static_cast<unsigned int>(actual) << std::dec << std::nouppercase
                  << std::endl;
        return false;
    }

    std::cout << "[PASS] " << name << " value=0x" << std::hex << std::uppercase
              << static_cast<unsigned int>(actual) << std::dec << std::nouppercase
              << std::endl;
    return true;
}

} // namespace

int main() {
    bool ok = true;

    superz80::APU white_noise;
    white_noise.write_port(superz80::APU::kNoiseControlPort, 0x00U);
    constexpr std::array<uint16_t, 8> kExpectedWhiteNoiseStates = {
        0x2000U, 0x1000U, 0x0800U, 0x0400U,
        0x0200U, 0x0100U, 0x0080U, 0x0040U
    };
    for (std::size_t index = 0U; index < kExpectedWhiteNoiseStates.size(); ++index) {
        white_noise.advance_audio_ticks(16U);
        ok = expect_equal_u16("white-noise-lfsr-advances-deterministically",
                              white_noise.noise_lfsr(), kExpectedWhiteNoiseStates[index]) && ok;
    }

    superz80::APU periodic_noise;
    periodic_noise.write_port(superz80::APU::kNoiseControlPort, 0x04U);
    periodic_noise.advance_audio_ticks(14U * 16U);
    ok = expect_equal_u16("periodic-noise-reaches-lsb-before-reseed", periodic_noise.noise_lfsr(), 0x0001U) && ok;
    periodic_noise.advance_audio_ticks(16U);
    ok = expect_equal_u16("periodic-noise-reseeds-when-shift-register-would-zero",
                          periodic_noise.noise_lfsr(), 0x4000U) && ok;

    superz80::APU tone_clocked_noise;
    tone_clocked_noise.write_port(superz80::APU::kToneCLowPort, 0x02U);
    tone_clocked_noise.write_port(superz80::APU::kToneCHighPort, 0x00U);
    tone_clocked_noise.write_port(superz80::APU::kNoiseControlPort, 0x03U);
    tone_clocked_noise.advance_audio_ticks(1U);
    ok = expect_equal_u16("tone-c-clocked-noise-does-not-shift-before-divider-expires",
                          tone_clocked_noise.noise_lfsr(), 0x4000U) && ok;
    tone_clocked_noise.advance_audio_ticks(1U);
    ok = expect_equal_u16("tone-c-clocked-noise-shifts-on-tone-c-event",
                          tone_clocked_noise.noise_lfsr(), 0x2000U) && ok;
    tone_clocked_noise.advance_audio_ticks(2U);
    ok = expect_equal_u16("tone-c-clocked-noise-remains-cadenced-by-tone-c-period",
                          tone_clocked_noise.noise_lfsr(), 0x1000U) && ok;

    superz80::APU repeat_a;
    superz80::APU repeat_b;
    repeat_a.write_port(superz80::APU::kNoiseControlPort, 0x06U);
    repeat_b.write_port(superz80::APU::kNoiseControlPort, 0x06U);
    constexpr std::array<uint32_t, 5> kTickSteps = {64U, 17U, 31U, 64U, 64U};
    for (std::size_t index = 0U; index < kTickSteps.size(); ++index) {
        repeat_a.advance_audio_ticks(kTickSteps[index]);
        repeat_b.advance_audio_ticks(kTickSteps[index]);
        ok = expect_equal_u16("noise-repeatability-lfsr-state", repeat_a.noise_lfsr(), repeat_b.noise_lfsr()) && ok;
        ok = expect_equal_u16("noise-repeatability-divider-state",
                              repeat_a.noise_runtime_state().divider_counter,
                              repeat_b.noise_runtime_state().divider_counter) && ok;
        ok = expect_equal_u8("noise-repeatability-output-bit",
                             repeat_a.noise_output_bit(),
                             repeat_b.noise_output_bit()) && ok;
    }

    return ok ? 0 : 1;
}
