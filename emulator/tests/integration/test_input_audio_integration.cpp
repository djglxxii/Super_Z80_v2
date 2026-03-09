#include "emulator_core.h"
#include "io.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>

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

bool expect_true(const char* name, bool value) {
    if (!value) {
        std::cerr << "[FAIL] " << name << " expected=true actual=false" << std::endl;
        return false;
    }

    std::cout << "[PASS] " << name << " value=true" << std::endl;
    return true;
}

std::array<uint8_t, 16U> make_rom() {
    std::array<uint8_t, 16U> rom{};

    rom[0x00U] = 0xDBU;
    rom[0x01U] = 0xDCU;
    rom[0x02U] = 0x32U;
    rom[0x03U] = 0x00U;
    rom[0x04U] = 0xC3U;
    rom[0x05U] = 0xDBU;
    rom[0x06U] = 0xDDU;
    rom[0x07U] = 0x32U;
    rom[0x08U] = 0x01U;
    rom[0x09U] = 0xC3U;
    rom[0x0AU] = 0xC3U;
    rom[0x0BU] = 0x00U;
    rom[0x0CU] = 0x00U;

    return rom;
}

void program_audio(EmulatorCore& core) {
    core.bus().write_port(superz80::Bus::kAudioToneALowPort, 0x02U);
    core.bus().write_port(superz80::Bus::kAudioToneAHighPort, 0x00U);
    core.bus().write_port(superz80::Bus::kAudioVolumeAPort, 0x00U);
    core.bus().write_port(superz80::Bus::kAudioNoiseControlPort, 0x00U);
    core.bus().write_port(superz80::Bus::kAudioVolumeNoisePort, 0x03U);
    core.bus().write_port(superz80::Bus::kAudioControlPort, 0x01U);
}

void set_controller_state(EmulatorCore& core) {
    core.bus().set_controller_button(superz80::IO::Button::Right, true);
    core.bus().set_controller_button(superz80::IO::Button::B, true);
    core.bus().set_controller_button(superz80::IO::Button::Start, true);
}

std::vector<EmulatorCore::AudioSample> collect_audio(EmulatorCore& core) {
    std::vector<EmulatorCore::AudioSample> samples(core.available_audio_samples(), 0);
    const std::size_t copied = core.consume_audio_samples(samples.data(), samples.size());
    samples.resize(copied);
    return samples;
}

struct RunResult {
    uint8_t pad1;
    uint8_t pad1_sys;
    uint8_t cpu_pad1;
    uint8_t cpu_pad1_sys;
    std::vector<EmulatorCore::AudioSample> samples;
};

RunResult run_sequence() {
    EmulatorCore core;
    const auto rom = make_rom();

    core.initialize();
    core.bus().load_rom(rom.data(), rom.size());
    program_audio(core);
    set_controller_state(core);

    for (std::size_t scanline = 0U; scanline < 64U; ++scanline) {
        core.step_scanline();
    }

    RunResult result{};
    result.pad1 = core.bus().read_port(superz80::Bus::kPad1Port);
    result.pad1_sys = core.bus().read_port(superz80::Bus::kPad1SysPort);
    result.cpu_pad1 = core.bus().read(0xC300U);
    result.cpu_pad1_sys = core.bus().read(0xC301U);
    result.samples = collect_audio(core);
    return result;
}

} // namespace

int main() {
    bool ok = true;

    const RunResult first = run_sequence();
    const RunResult second = run_sequence();

    ok = expect_equal_u8("pad1-bus-read-reflects-controller-state", first.pad1, 0xD7U) && ok;
    ok = expect_equal_u8("pad1-sys-bus-read-reflects-controller-state", first.pad1_sys, 0xFEU) && ok;
    ok = expect_equal_u8("cpu-pad1-read-remains-deterministic", first.cpu_pad1, 0xD7U) && ok;
    ok = expect_equal_u8("cpu-pad1-sys-read-remains-deterministic", first.cpu_pad1_sys, 0xFEU) && ok;
    ok = expect_true("audio-samples-are-produced-under-platform-stepping", !first.samples.empty()) && ok;
    ok = expect_true("audio-output-is-repeatable-for-identical-input-sequences", first.samples == second.samples) && ok;
    ok = expect_equal_u8("repeated-pad1-read-matches", second.cpu_pad1, first.cpu_pad1) && ok;
    ok = expect_equal_u8("repeated-pad1-sys-read-matches", second.cpu_pad1_sys, first.cpu_pad1_sys) && ok;

    return ok ? 0 : 1;
}
