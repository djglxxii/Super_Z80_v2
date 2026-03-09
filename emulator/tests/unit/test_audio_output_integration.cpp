#include "emulator_core.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

bool expect_true(const char* name, bool value) {
    if (!value) {
        std::cerr << "[FAIL] " << name << " expected=true actual=false" << std::endl;
        return false;
    }

    std::cout << "[PASS] " << name << " value=true" << std::endl;
    return true;
}

bool expect_equal_size(const char* name, std::size_t actual, std::size_t expected) {
    if (actual != expected) {
        std::cerr << "[FAIL] " << name << " expected=" << expected << " actual=" << actual
                  << std::endl;
        return false;
    }

    std::cout << "[PASS] " << name << " value=" << actual << std::endl;
    return true;
}

void program_tone_a(EmulatorCore& core, uint16_t period, uint8_t volume, uint8_t control) {
    core.bus().write_port(superz80::Bus::kAudioToneALowPort, static_cast<uint8_t>(period & 0x00FFU));
    core.bus().write_port(superz80::Bus::kAudioToneAHighPort, static_cast<uint8_t>((period >> 8U) & 0x0FU));
    core.bus().write_port(superz80::Bus::kAudioVolumeAPort, volume);
    core.bus().write_port(superz80::Bus::kAudioControlPort, control);
}

std::vector<EmulatorCore::AudioSample> collect_audio(EmulatorCore& core, std::size_t scanlines) {
    for (std::size_t index = 0U; index < scanlines; ++index) {
        core.step_scanline();
    }

    std::vector<EmulatorCore::AudioSample> samples(core.available_audio_samples(), 0);
    const std::size_t copied = core.consume_audio_samples(samples.data(), samples.size());
    samples.resize(copied);
    return samples;
}

bool all_zero(const std::vector<EmulatorCore::AudioSample>& samples) {
    for (const auto sample : samples) {
        if (sample != 0) {
            return false;
        }
    }

    return true;
}

} // namespace

int main() {
    bool ok = true;

    EmulatorCore init_core;
    init_core.initialize();
    ok = expect_equal_size("initialization-path-starts-with-empty-audio-buffer",
                           init_core.available_audio_samples(), 0U) && ok;
    init_core.step_scanline();
    ok = expect_true("headless-scanline-step-without-sdl-remains-functional", true) && ok;

    EmulatorCore enabled_core;
    enabled_core.initialize();
    program_tone_a(enabled_core, 0x0001U, 0x00U, 0x01U);
    const std::vector<EmulatorCore::AudioSample> enabled_samples = collect_audio(enabled_core, 32U);
    ok = expect_true("sample-handoff-produces-non-empty-block", !enabled_samples.empty()) && ok;
    ok = expect_true("sample-handoff-produces-audible-samples", !all_zero(enabled_samples)) && ok;
    ok = expect_equal_size("sample-handoff-drains-buffer-after-consume",
                           enabled_core.available_audio_samples(), 0U) && ok;

    EmulatorCore muted_core;
    muted_core.initialize();
    program_tone_a(muted_core, 0x0001U, 0x00U, 0x03U);
    const std::vector<EmulatorCore::AudioSample> muted_samples = collect_audio(muted_core, 32U);
    ok = expect_true("muted-state-produces-silent-payload", all_zero(muted_samples)) && ok;

    EmulatorCore disabled_core;
    disabled_core.initialize();
    program_tone_a(disabled_core, 0x0001U, 0x00U, 0x00U);
    const std::vector<EmulatorCore::AudioSample> disabled_samples = collect_audio(disabled_core, 32U);
    ok = expect_true("disabled-state-produces-silent-payload", all_zero(disabled_samples)) && ok;

    EmulatorCore repeat_a;
    EmulatorCore repeat_b;
    repeat_a.initialize();
    repeat_b.initialize();
    program_tone_a(repeat_a, 0x0010U, 0x02U, 0x01U);
    program_tone_a(repeat_b, 0x0010U, 0x02U, 0x01U);
    const std::vector<EmulatorCore::AudioSample> repeat_a_samples = collect_audio(repeat_a, 64U);
    const std::vector<EmulatorCore::AudioSample> repeat_b_samples = collect_audio(repeat_b, 64U);
    ok = expect_equal_size("repeatability-block-size", repeat_a_samples.size(), repeat_b_samples.size()) && ok;
    ok = expect_true("repeatability-produced-sample-blocks-match", repeat_a_samples == repeat_b_samples) && ok;

    return ok ? 0 : 1;
}
