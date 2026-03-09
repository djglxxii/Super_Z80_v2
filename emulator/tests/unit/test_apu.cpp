#include "apu.h"
#include "bus.h"
#include "cpu.h"

#include <array>
#include <cstdint>
#include <iostream>

namespace {

bool expect_equal(const char* name, uint8_t actual, uint8_t expected) {
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

    superz80::APU apu;
    ok = expect_equal("reset-tone-a-low-default", apu.read_port(superz80::APU::kToneALowPort), 0x00U) && ok;
    ok = expect_equal("reset-tone-a-high-default", apu.read_port(superz80::APU::kToneAHighPort), 0x00U) && ok;
    ok = expect_equal("reset-noise-default", apu.read_port(superz80::APU::kNoiseControlPort), 0x00U) && ok;
    ok = expect_equal("reset-volume-a-default", apu.read_port(superz80::APU::kVolumeAPort), 0x0FU) && ok;
    ok = expect_equal("reset-volume-noise-default", apu.read_port(superz80::APU::kVolumeNoisePort), 0x0FU) && ok;
    ok = expect_equal("reset-control-default", apu.read_port(superz80::APU::kControlPort), 0x00U) && ok;

    apu.write_port(superz80::APU::kToneALowPort, 0x34U);
    apu.write_port(superz80::APU::kToneAHighPort, 0xF2U);
    ok = expect_equal("tone-a-high-masks-unused-bits", apu.read_port(superz80::APU::kToneAHighPort), 0x02U) && ok;
    ok = expect_equal_u16("tone-a-effective-period-uses-12-bits", apu.tone_period(0U), 0x0234U) && ok;

    apu.write_port(superz80::APU::kNoiseControlPort, 0xFFU);
    ok = expect_equal("noise-control-masks-unused-bits", apu.read_port(superz80::APU::kNoiseControlPort), 0x07U) && ok;

    apu.write_port(superz80::APU::kVolumeAPort, 0xF3U);
    apu.write_port(superz80::APU::kVolumeNoisePort, 0x92U);
    ok = expect_equal("volume-a-masks-unused-bits", apu.read_port(superz80::APU::kVolumeAPort), 0x03U) && ok;
    ok = expect_equal("volume-noise-masks-unused-bits", apu.read_port(superz80::APU::kVolumeNoisePort), 0x02U) && ok;

    apu.write_port(superz80::APU::kControlPort, 0xFFU);
    ok = expect_equal("control-reads-back-enable-mute-only", apu.read_port(superz80::APU::kControlPort), 0x03U) && ok;

    apu.write_port(superz80::APU::kToneBLowPort, 0x56U);
    apu.write_port(superz80::APU::kVolumeBPort, 0x09U);
    apu.reset();
    ok = expect_equal("reset-clears-tone-b-low", apu.read_port(superz80::APU::kToneBLowPort), 0x00U) && ok;
    ok = expect_equal("reset-restores-volume-b-default", apu.read_port(superz80::APU::kVolumeBPort), 0x0FU) && ok;
    ok = expect_equal("reset-clears-control", apu.read_port(superz80::APU::kControlPort), 0x00U) && ok;

    superz80::Bus bus;
    bus.write_port(superz80::Bus::kAudioToneCLowPort, 0x78U);
    bus.write_port(superz80::Bus::kAudioToneCHighPort, 0x0AU);
    bus.write_port(superz80::Bus::kAudioVolumeCPort, 0xE4U);
    bus.write_port(superz80::Bus::kAudioControlPort, 0x05U);
    ok = expect_equal("bus-audio-tone-c-low-routed", bus.read_port(superz80::Bus::kAudioToneCLowPort), 0x78U) && ok;
    ok = expect_equal("bus-audio-tone-c-high-routed", bus.read_port(superz80::Bus::kAudioToneCHighPort), 0x0AU) && ok;
    ok = expect_equal("bus-audio-volume-c-routed", bus.read_port(superz80::Bus::kAudioVolumeCPort), 0x04U) && ok;
    ok = expect_equal("bus-audio-control-self-clears-reset-bit", bus.read_port(superz80::Bus::kAudioControlPort), 0x01U) && ok;
    ok = expect_equal_u16("bus-exposes-effective-tone-c-period", bus.apu().tone_period(2U), 0x0A78U) && ok;

    constexpr std::array<uint8_t, 13> kRom = {
        0x3EU, 0xA5U, // LD A,0xA5
        0xD3U, 0xD6U, // OUT (0xD6),A
        0x3EU, 0xF4U, // LD A,0xF4
        0xD3U, 0xD7U, // OUT (0xD7),A
        0xDBU, 0xD7U, // IN A,(0xD7)
        0x76U         // HALT
    };

    bus.reset();
    bus.load_rom(kRom.data(), kRom.size());
    superz80::CPU cpu(bus);
    cpu.reset();
    cpu.step();
    cpu.step();
    cpu.step();
    cpu.step();
    cpu.step();
    ok = expect_equal("cpu-out-updates-audio-noise-register", bus.read_port(superz80::Bus::kAudioNoiseControlPort), 0x05U) && ok;
    ok = expect_equal("cpu-out-masks-audio-volume-register", bus.read_port(superz80::Bus::kAudioVolumeAPort), 0x04U) && ok;
    ok = expect_equal("cpu-in-reads-masked-audio-volume-register", cpu.get_register_a(), 0x04U) && ok;

    return ok ? 0 : 1;
}
