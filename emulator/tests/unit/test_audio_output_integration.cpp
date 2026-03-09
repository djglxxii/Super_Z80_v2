#include "emulator_core.h"
#include "../test_audio_sequence_helpers.h"

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

bool expect_equal_i16(const char* name, int16_t actual, int16_t expected) {
    if (actual != expected) {
        std::cerr << "[FAIL] " << name << " expected=" << expected
                  << " actual=" << actual << std::endl;
        return false;
    }

    std::cout << "[PASS] " << name << " value=" << actual << std::endl;
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
    superz80::testaudio::program_apu_tone(enabled_core.bus(), 0x0001U, 0x00U, 0x01U);
    const std::vector<EmulatorCore::AudioSample> enabled_samples =
        superz80::testaudio::collect_core_audio(enabled_core, 32U);
    ok = expect_true("sample-handoff-produces-non-empty-block", !enabled_samples.empty()) && ok;
    ok = expect_true("sample-handoff-produces-audible-samples",
                     !superz80::testaudio::all_zero(enabled_samples)) && ok;
    ok = expect_equal_size("sample-handoff-drains-buffer-after-consume",
                           enabled_core.available_audio_samples(), 0U) && ok;

    EmulatorCore muted_core;
    muted_core.initialize();
    superz80::testaudio::program_apu_tone(muted_core.bus(), 0x0001U, 0x00U, 0x03U);
    const std::vector<EmulatorCore::AudioSample> muted_samples =
        superz80::testaudio::collect_core_audio(muted_core, 32U);
    ok = expect_true("muted-state-produces-silent-payload",
                     superz80::testaudio::all_zero(muted_samples)) && ok;

    EmulatorCore disabled_core;
    disabled_core.initialize();
    superz80::testaudio::program_apu_tone(disabled_core.bus(), 0x0001U, 0x00U, 0x00U);
    const std::vector<EmulatorCore::AudioSample> disabled_samples =
        superz80::testaudio::collect_core_audio(disabled_core, 32U);
    ok = expect_true("disabled-state-produces-silent-payload",
                     superz80::testaudio::all_zero(disabled_samples)) && ok;

    EmulatorCore repeat_a;
    EmulatorCore repeat_b;
    repeat_a.initialize();
    repeat_b.initialize();
    superz80::testaudio::program_apu_tone(repeat_a.bus(), 0x0010U, 0x02U, 0x01U);
    superz80::testaudio::program_apu_tone(repeat_b.bus(), 0x0010U, 0x02U, 0x01U);
    const std::vector<EmulatorCore::AudioSample> repeat_a_samples =
        superz80::testaudio::collect_core_audio(repeat_a, 64U);
    const std::vector<EmulatorCore::AudioSample> repeat_b_samples =
        superz80::testaudio::collect_core_audio(repeat_b, 64U);
    ok = expect_equal_size("repeatability-block-size", repeat_a_samples.size(), repeat_b_samples.size()) && ok;
    ok = expect_true("repeatability-produced-sample-blocks-match", repeat_a_samples == repeat_b_samples) && ok;

    EmulatorCore ym_only_a;
    EmulatorCore ym_only_b;
    ym_only_a.initialize();
    ym_only_b.initialize();
    superz80::testaudio::program_apu_tone(ym_only_a.bus(), 0x0002U, 0x00U, 0x00U);
    superz80::testaudio::program_apu_tone(ym_only_b.bus(), 0x0002U, 0x00U, 0x00U);
    superz80::testaudio::program_deterministic_ym2151_voice(ym_only_a.bus());
    superz80::testaudio::program_deterministic_ym2151_voice(ym_only_b.bus());
    const std::vector<EmulatorCore::AudioSample> ym_only_samples_a =
        superz80::testaudio::collect_core_audio(ym_only_a, 64U);
    const std::vector<EmulatorCore::AudioSample> ym_only_samples_b =
        superz80::testaudio::collect_core_audio(ym_only_b, 64U);
    ok = expect_true("ym-only-core-output-is-repeatable", ym_only_samples_a == ym_only_samples_b) && ok;
    ok = expect_true("ym-only-core-output-is-audible",
                     !superz80::testaudio::all_zero(ym_only_samples_a)) && ok;
    ok = expect_equal_i16("ym-only-apu-source-remains-silent", ym_only_a.bus().apu().current_sample(), 0) && ok;

    EmulatorCore combined_a;
    EmulatorCore combined_b;
    combined_a.initialize();
    combined_b.initialize();
    superz80::testaudio::program_apu_tone(combined_a.bus(), 0x0002U, 0x00U, 0x01U);
    superz80::testaudio::program_apu_tone(combined_b.bus(), 0x0002U, 0x00U, 0x01U);
    superz80::testaudio::program_deterministic_ym2151_voice(combined_a.bus());
    superz80::testaudio::program_deterministic_ym2151_voice(combined_b.bus());
    const std::vector<EmulatorCore::AudioSample> combined_samples_a =
        superz80::testaudio::collect_core_audio(combined_a, 64U);
    const std::vector<EmulatorCore::AudioSample> combined_samples_b =
        superz80::testaudio::collect_core_audio(combined_b, 64U);
    ok = expect_true("combined-core-output-is-repeatable", combined_samples_a == combined_samples_b) && ok;
    ok = expect_true("combined-core-output-differs-from-apu-only-output",
                     combined_samples_a != repeat_a_samples) && ok;
    ok = expect_true("combined-core-output-differs-from-ym-only-output",
                     combined_samples_a != ym_only_samples_a) && ok;

    return ok ? 0 : 1;
}
