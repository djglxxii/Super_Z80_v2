#include "apu.h"

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

bool expect_false(const char* name, bool value) {
    if (value) {
        std::cerr << "[FAIL] " << name << " expected=false actual=true" << std::endl;
        return false;
    }

    std::cout << "[PASS] " << name << " value=false" << std::endl;
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

} // namespace

int main() {
    bool ok = true;

    superz80::APU apu;
    ok = expect_equal_u16("reset-tone-a-counter", apu.tone_runtime_state(0U).divider_counter, 0x0000U) && ok;
    ok = expect_false("reset-tone-a-phase-low", apu.tone_runtime_state(0U).phase_high) && ok;
    ok = expect_equal_u16("reset-noise-counter", apu.noise_runtime_state().divider_counter, 0x0010U) && ok;
    ok = expect_equal_u16("reset-noise-lfsr-seed", apu.noise_lfsr(), 0x4000U) && ok;
    ok = expect_equal_u8("reset-noise-output-bit", apu.noise_output_bit(), 0x00U) && ok;

    apu.write_port(superz80::APU::kToneALowPort, 0x03U);
    ok = expect_equal_u16("tone-a-counter-reloads-from-register-write", apu.tone_runtime_state(0U).divider_counter, 0x0003U) && ok;
    apu.advance_audio_ticks(1U);
    ok = expect_equal_u16("tone-a-counter-after-one-tick", apu.tone_runtime_state(0U).divider_counter, 0x0002U) && ok;
    ok = expect_false("tone-a-phase-still-low-before-expiry", apu.tone_runtime_state(0U).phase_high) && ok;
    apu.advance_audio_ticks(2U);
    ok = expect_equal_u16("tone-a-counter-reloads-after-expiry", apu.tone_runtime_state(0U).divider_counter, 0x0003U) && ok;
    ok = expect_true("tone-a-phase-toggles-at-period-boundary", apu.tone_runtime_state(0U).phase_high) && ok;

    apu.write_port(superz80::APU::kToneBLowPort, 0x01U);
    apu.advance_audio_ticks(1U);
    ok = expect_true("tone-b-period-one-toggles-on-first-tick", apu.tone_runtime_state(1U).phase_high) && ok;
    apu.advance_audio_ticks(1U);
    ok = expect_false("tone-b-period-one-toggles-back-on-second-tick", apu.tone_runtime_state(1U).phase_high) && ok;

    apu.reset();
    apu.write_port(superz80::APU::kNoiseControlPort, 0x00U);
    apu.advance_audio_ticks(16U);
    ok = expect_equal_u16("white-noise-shifts-after-16-ticks", apu.noise_lfsr(), 0x2000U) && ok;
    ok = expect_equal_u16("white-noise-counter-reloads-after-shift", apu.noise_runtime_state().divider_counter, 0x0010U) && ok;

    apu.write_port(superz80::APU::kNoiseControlPort, 0x04U);
    apu.write_port(superz80::APU::kControlPort, 0x04U);
    apu.advance_audio_ticks(15U * 16U);
    ok = expect_equal_u16("periodic-noise-reseeds-at-cycle-boundary", apu.noise_lfsr(), 0x4000U) && ok;

    apu.reset();
    apu.write_port(superz80::APU::kToneCLowPort, 0x02U);
    apu.write_port(superz80::APU::kNoiseControlPort, 0x03U);
    apu.advance_audio_ticks(1U);
    ok = expect_equal_u16("tone-c-clocked-noise-does-not-step-before-divider-event", apu.noise_lfsr(), 0x4000U) && ok;
    apu.advance_audio_ticks(1U);
    ok = expect_equal_u16("tone-c-clocked-noise-steps-on-divider-event", apu.noise_lfsr(), 0x2000U) && ok;

    apu.reset();
    apu.write_port(superz80::APU::kToneALowPort, 0x04U);
    apu.advance_audio_ticks(2U);
    apu.write_port(superz80::APU::kToneALowPort, 0x02U);
    ok = expect_equal_u16("tone-register-update-reloads-divider", apu.tone_runtime_state(0U).divider_counter, 0x0002U) && ok;
    apu.advance_audio_ticks(2U);
    ok = expect_true("tone-register-update-affects-subsequent-ticks", apu.tone_runtime_state(0U).phase_high) && ok;

    superz80::APU repeat_a;
    superz80::APU repeat_b;
    repeat_a.write_port(superz80::APU::kToneALowPort, 0x05U);
    repeat_b.write_port(superz80::APU::kToneALowPort, 0x05U);
    repeat_a.write_port(superz80::APU::kToneCHighPort, 0x01U);
    repeat_b.write_port(superz80::APU::kToneCHighPort, 0x01U);
    repeat_a.write_port(superz80::APU::kNoiseControlPort, 0x02U);
    repeat_b.write_port(superz80::APU::kNoiseControlPort, 0x02U);
    repeat_a.advance_audio_ticks(17U);
    repeat_b.advance_audio_ticks(17U);
    repeat_a.advance_audio_ticks(31U);
    repeat_b.advance_audio_ticks(31U);
    ok = expect_equal_u16("repeatability-tone-a-counter", repeat_a.tone_runtime_state(0U).divider_counter,
                          repeat_b.tone_runtime_state(0U).divider_counter) && ok;
    ok = expect_equal_u8("repeatability-tone-a-phase", static_cast<uint8_t>(repeat_a.tone_runtime_state(0U).phase_high),
                         static_cast<uint8_t>(repeat_b.tone_runtime_state(0U).phase_high)) && ok;
    ok = expect_equal_u16("repeatability-noise-lfsr", repeat_a.noise_lfsr(), repeat_b.noise_lfsr()) && ok;

    apu.reset();
    apu.write_port(superz80::APU::kToneALowPort, 0x00U);
    apu.write_port(superz80::APU::kToneAHighPort, 0x00U);
    apu.advance_audio_ticks(64U);
    ok = expect_false("zero-period-tone-remains-silent", apu.tone_runtime_state(0U).phase_high) && ok;
    ok = expect_equal_u16("zero-period-tone-counter-stays-zero", apu.tone_runtime_state(0U).divider_counter, 0x0000U) && ok;

    apu.write_port(superz80::APU::kToneAHighPort, 0x0FU);
    apu.write_port(superz80::APU::kToneALowPort, 0xFFU);
    ok = expect_equal_u16("max-period-tone-loads-12-bit-boundary", apu.tone_runtime_state(0U).divider_counter, 0x0FFFU) && ok;
    apu.write_port(superz80::APU::kNoiseControlPort, 0x02U);
    ok = expect_equal_u16("noise-divider-select-64-loads-boundary", apu.noise_runtime_state().divider_counter, 0x0040U) && ok;

    return ok ? 0 : 1;
}
