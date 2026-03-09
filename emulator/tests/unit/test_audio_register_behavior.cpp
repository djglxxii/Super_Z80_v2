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

bool expect_equal_i16(const char* name, int16_t actual, int16_t expected) {
    if (actual != expected) {
        std::cerr << "[FAIL] " << name << " expected=" << expected
                  << " actual=" << actual << std::endl;
        return false;
    }

    std::cout << "[PASS] " << name << " value=" << actual << std::endl;
    return true;
}

void program_tone_a(superz80::APU& apu, uint16_t period, uint8_t volume, uint8_t control) {
    apu.write_port(superz80::APU::kToneALowPort, static_cast<uint8_t>(period & 0x00FFU));
    apu.write_port(superz80::APU::kToneAHighPort, static_cast<uint8_t>((period >> 8U) & 0x0FU));
    apu.write_port(superz80::APU::kVolumeAPort, volume);
    apu.write_port(superz80::APU::kControlPort, control);
}

} // namespace

int main() {
    bool ok = true;

    superz80::APU apu;
    ok = expect_equal_u8("reset-tone-a-high-default", apu.read_port(superz80::APU::kToneAHighPort), 0x00U) && ok;
    ok = expect_equal_u8("reset-noise-default", apu.read_port(superz80::APU::kNoiseControlPort), 0x00U) && ok;
    ok = expect_equal_u8("reset-volume-c-default", apu.read_port(superz80::APU::kVolumeCPort), 0x0FU) && ok;
    ok = expect_equal_u8("reset-control-default", apu.read_port(superz80::APU::kControlPort), 0x00U) && ok;

    apu.write_port(superz80::APU::kToneAHighPort, 0xFFU);
    apu.write_port(superz80::APU::kNoiseControlPort, 0xFFU);
    apu.write_port(superz80::APU::kVolumeAPort, 0xFFU);
    apu.write_port(superz80::APU::kControlPort, 0xFFU);
    ok = expect_equal_u8("tone-high-masks-undefined-bits", apu.read_port(superz80::APU::kToneAHighPort), 0x0FU) && ok;
    ok = expect_equal_u8("noise-control-masks-undefined-bits", apu.read_port(superz80::APU::kNoiseControlPort), 0x07U) && ok;
    ok = expect_equal_u8("volume-masks-undefined-bits", apu.read_port(superz80::APU::kVolumeAPort), 0x0FU) && ok;
    ok = expect_equal_u8("control-readback-masks-reset-and-overrun-bits", apu.read_port(superz80::APU::kControlPort), 0x03U) && ok;

    apu.write_port(superz80::APU::kToneBLowPort, 0xFFU);
    apu.write_port(superz80::APU::kToneBHighPort, 0xFFU);
    ok = expect_equal_u16("max-tone-period-combines-to-12-bit-boundary", apu.tone_period(1U), 0x0FFFU) && ok;
    apu.write_port(superz80::APU::kToneBLowPort, 0x00U);
    apu.write_port(superz80::APU::kToneBHighPort, 0x00U);
    ok = expect_equal_u16("min-tone-period-combines-to-zero", apu.tone_period(1U), 0x0000U) && ok;

    program_tone_a(apu, 0x0001U, 0x00U, 0x00U);
    apu.advance_audio_ticks(1U);
    ok = expect_equal_i16("disabled-audio-forces-silence", apu.generate_current_sample(), 0) && ok;
    apu.write_port(superz80::APU::kControlPort, 0x01U);
    ok = expect_equal_i16("enabled-audio-restores-current-mix", apu.generate_current_sample(), 8191) && ok;
    apu.write_port(superz80::APU::kControlPort, 0x03U);
    ok = expect_equal_i16("mute-bit-forces-silence", apu.generate_current_sample(), 0) && ok;

    apu.flag_sample_queue_overrun();
    ok = expect_equal_u8("overrun-bit-latches-into-control-readback", apu.read_port(superz80::APU::kControlPort), 0x83U) && ok;
    apu.write_port(superz80::APU::kControlPort, 0x05U);
    ok = expect_equal_u8("reset-bit-self-clears-from-control-readback", apu.read_port(superz80::APU::kControlPort), 0x01U) && ok;
    ok = expect_equal_u16("reset-runtime-reloads-tone-divider-from-registers", apu.tone_runtime_state(0U).divider_counter, 0x0001U) && ok;
    ok = expect_equal_i16("reset-runtime-clears-latched-sample", apu.sample_output_state().current_sample, 0) && ok;
    ok = expect_equal_u8("reset-runtime-clears-overrun-bit", apu.read_port(superz80::APU::kControlPort), 0x01U) && ok;

    return ok ? 0 : 1;
}
