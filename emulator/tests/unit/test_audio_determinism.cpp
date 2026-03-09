#include "emulator_core.h"
#include "../test_audio_sequence_helpers.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <type_traits>
#include <vector>

#if defined(SUPER_Z80_HAS_SDL)
#include <SDL.h>
#endif

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

void program_deterministic_mix(EmulatorCore& core) {
    constexpr uint16_t kTonePeriod = 0x0002U;

    core.bus().write_port(superz80::Bus::kAudioToneALowPort, static_cast<uint8_t>(kTonePeriod & 0x00FFU));
    core.bus().write_port(superz80::Bus::kAudioToneAHighPort, static_cast<uint8_t>((kTonePeriod >> 8U) & 0x0FU));
    core.bus().write_port(superz80::Bus::kAudioVolumeAPort, 0x00U);
    core.bus().write_port(superz80::Bus::kAudioNoiseControlPort, 0x00U);
    core.bus().write_port(superz80::Bus::kAudioVolumeNoisePort, 0x02U);
    core.bus().write_port(superz80::Bus::kAudioControlPort, 0x01U);
}

void program_combined_mix(EmulatorCore& core) {
    superz80::testaudio::program_apu_tone(core.bus(), 0x0002U, 0x00U, 0x01U);
    core.bus().write_port(superz80::Bus::kAudioNoiseControlPort, 0x00U);
    core.bus().write_port(superz80::Bus::kAudioVolumeNoisePort, 0x02U);
    superz80::testaudio::program_deterministic_ym2151_voice(core.bus());
}

std::vector<EmulatorCore::AudioSample> collect_all_audio(EmulatorCore& core) {
    std::vector<EmulatorCore::AudioSample> samples(core.available_audio_samples(), 0);
    const std::size_t copied = core.consume_audio_samples(samples.data(), samples.size());
    samples.resize(copied);
    return samples;
}

std::size_t collect_scanline_samples(EmulatorCore& core) {
    core.step_scanline();
    const std::size_t produced = core.available_audio_samples();
    std::vector<EmulatorCore::AudioSample> scratch(produced, 0);
    core.consume_audio_samples(scratch.data(), scratch.size());
    return produced;
}

std::size_t reference_scanline_samples(uint32_t& remainder) {
    remainder += superz80::APU::kSampleRateHz;
    std::size_t produced = 0U;
    while (remainder >= EmulatorCore::kScanlinesPerSecond) {
        remainder -= EmulatorCore::kScanlinesPerSecond;
        ++produced;
    }
    return produced;
}

#if defined(SUPER_Z80_HAS_SDL)
bool run_sdl_boundary_checks() {
    if (SDL_setenv("SDL_AUDIODRIVER", "dummy", 1) != 0) {
        std::cerr << "[FAIL] sdl-setenv-audiodriver " << SDL_GetError() << std::endl;
        return false;
    }

    if (SDL_Init(SDL_INIT_AUDIO) != 0) {
        std::cerr << "[FAIL] sdl-init-audio " << SDL_GetError() << std::endl;
        return false;
    }

    SDL_AudioSpec desired = {};
    desired.freq = static_cast<int>(superz80::APU::kSampleRateHz);
    desired.format = AUDIO_S16LSB;
    desired.channels = 1;
    desired.samples = 1024U;

    SDL_AudioSpec obtained = {};
    const SDL_AudioDeviceID device = SDL_OpenAudioDevice(nullptr, 0, &desired, &obtained, 0);
    if (device == 0U) {
        std::cerr << "[FAIL] sdl-open-audio-device " << SDL_GetError() << std::endl;
        SDL_Quit();
        return false;
    }

    bool ok = true;
    ok = expect_true("audio-sample-type-is-signed-16-bit",
                     std::is_same<EmulatorCore::AudioSample, int16_t>::value) && ok;
    ok = expect_equal_size("audio-sample-size-is-two-bytes", sizeof(EmulatorCore::AudioSample), 2U) && ok;
    ok = expect_equal_size("dummy-sdl-device-frequency-matches-contract", static_cast<std::size_t>(obtained.freq),
                           static_cast<std::size_t>(desired.freq)) && ok;
    ok = expect_equal_size("dummy-sdl-device-channel-count-is-mono", obtained.channels, desired.channels) && ok;
    ok = expect_equal_size("dummy-sdl-device-format-is-s16le", obtained.format, desired.format) && ok;

    EmulatorCore core;
    core.initialize();
    program_deterministic_mix(core);
    while (core.available_audio_samples() < 32U) {
        core.step_scanline();
    }

    std::vector<EmulatorCore::AudioSample> samples = collect_all_audio(core);
    const Uint32 queue_bytes = static_cast<Uint32>(16U * sizeof(EmulatorCore::AudioSample));
    ok = expect_equal_size("queued-audio-starts-empty", SDL_GetQueuedAudioSize(device), 0U) && ok;
    ok = expect_true("queue-audio-accepts-sample-buffer",
                     SDL_QueueAudio(device, samples.data(), queue_bytes) == 0) && ok;
    ok = expect_equal_size("queued-audio-size-matches-sample-byte-count",
                           SDL_GetQueuedAudioSize(device), queue_bytes) && ok;

    SDL_ClearQueuedAudio(device);
    SDL_CloseAudioDevice(device);
    SDL_Quit();
    return ok;
}
#endif

} // namespace

int main() {
    bool ok = true;

    superz80::APU direct_a;
    superz80::APU direct_b;
    direct_a.write_port(superz80::APU::kToneALowPort, 0x01U);
    direct_b.write_port(superz80::APU::kToneALowPort, 0x01U);
    direct_a.write_port(superz80::APU::kVolumeAPort, 0x00U);
    direct_b.write_port(superz80::APU::kVolumeAPort, 0x00U);
    direct_a.write_port(superz80::APU::kControlPort, 0x01U);
    direct_b.write_port(superz80::APU::kControlPort, 0x01U);
    constexpr std::array<int16_t, 8> kExpectedToneSequence = {
        8191, -8191, 8191, -8191,
        8191, -8191, 8191, -8191
    };
    for (std::size_t index = 0U; index < kExpectedToneSequence.size(); ++index) {
        const int16_t sample_a = direct_a.advance_and_generate_sample(1U);
        const int16_t sample_b = direct_b.advance_and_generate_sample(1U);
        ok = expect_equal_i16("golden-tone-sequence-matches-expected-values", sample_a, kExpectedToneSequence[index]) && ok;
        ok = expect_equal_i16("identical-apu-runs-produce-byte-identical-samples", sample_a, sample_b) && ok;
    }

    EmulatorCore cadence_core;
    cadence_core.initialize();
    program_deterministic_mix(cadence_core);
    uint32_t cadence_remainder = 0U;
    std::size_t frame_total = 0U;
    std::size_t four_sample_scanlines = 0U;
    for (std::size_t scanline = 0U; scanline < superz80::Scheduler::kScanlinesPerFrame; ++scanline) {
        const std::size_t actual = collect_scanline_samples(cadence_core);
        const std::size_t expected = reference_scanline_samples(cadence_remainder);
        ok = expect_equal_size("scanline-sample-cadence-matches-reference-accumulator", actual, expected) && ok;
        frame_total += actual;
        if (actual == 4U) {
            ++four_sample_scanlines;
        }
    }
    ok = expect_equal_size("one-frame-produces-800-samples", frame_total, 800U) && ok;
    ok = expect_equal_size("one-frame-contains-14-four-sample-scanlines", four_sample_scanlines, 14U) && ok;

    EmulatorCore second_core;
    second_core.initialize();
    program_deterministic_mix(second_core);
    uint32_t second_remainder = 0U;
    std::size_t second_total = 0U;
    for (std::size_t scanline = 0U; scanline < EmulatorCore::kScanlinesPerSecond; ++scanline) {
        const std::size_t actual = collect_scanline_samples(second_core);
        const std::size_t expected = reference_scanline_samples(second_remainder);
        ok = expect_equal_size("one-second-cadence-remains-deterministic", actual, expected) && ok;
        second_total += actual;
    }
    ok = expect_equal_size("one-second-produces-exactly-48000-samples", second_total, 48000U) && ok;

    EmulatorCore repeat_a;
    EmulatorCore repeat_b;
    repeat_a.initialize();
    repeat_b.initialize();
    program_deterministic_mix(repeat_a);
    program_deterministic_mix(repeat_b);
    std::vector<EmulatorCore::AudioSample> repeat_a_samples;
    std::vector<EmulatorCore::AudioSample> repeat_b_samples;
    for (std::size_t scanline = 0U; scanline < 128U; ++scanline) {
        repeat_a.step_scanline();
        repeat_b.step_scanline();
    }
    repeat_a_samples = collect_all_audio(repeat_a);
    repeat_b_samples = collect_all_audio(repeat_b);
    ok = expect_true("headless-output-produces-non-empty-sample-block", !repeat_a_samples.empty()) && ok;
    ok = expect_true("repeatable-core-runs-produce-identical-sample-blocks", repeat_a_samples == repeat_b_samples) && ok;
    ok = expect_equal_size("buffer-consume-drains-available-samples", repeat_a.available_audio_samples(), 0U) && ok;

    EmulatorCore ym_only_a;
    EmulatorCore ym_only_b;
    ym_only_a.initialize();
    ym_only_b.initialize();
    superz80::testaudio::program_apu_tone(ym_only_a.bus(), 0x0002U, 0x00U, 0x00U);
    superz80::testaudio::program_apu_tone(ym_only_b.bus(), 0x0002U, 0x00U, 0x00U);
    superz80::testaudio::program_deterministic_ym2151_voice(ym_only_a.bus());
    superz80::testaudio::program_deterministic_ym2151_voice(ym_only_b.bus());
    const std::vector<EmulatorCore::AudioSample> ym_only_a_samples =
        superz80::testaudio::collect_core_audio(ym_only_a, 64U);
    const std::vector<EmulatorCore::AudioSample> ym_only_b_samples =
        superz80::testaudio::collect_core_audio(ym_only_b, 64U);
    ok = expect_true("ym-only-core-runs-produce-identical-sample-blocks",
                     ym_only_a_samples == ym_only_b_samples) && ok;
    ok = expect_true("ym-only-core-output-is-non-silent",
                     !superz80::testaudio::all_zero(ym_only_a_samples)) && ok;

    EmulatorCore combined_repeat_a;
    EmulatorCore combined_repeat_b;
    combined_repeat_a.initialize();
    combined_repeat_b.initialize();
    program_combined_mix(combined_repeat_a);
    program_combined_mix(combined_repeat_b);
    const std::vector<EmulatorCore::AudioSample> combined_repeat_a_samples =
        superz80::testaudio::collect_core_audio(combined_repeat_a, 64U);
    const std::vector<EmulatorCore::AudioSample> combined_repeat_b_samples =
        superz80::testaudio::collect_core_audio(combined_repeat_b, 64U);
    ok = expect_true("combined-core-runs-produce-identical-sample-blocks",
                     combined_repeat_a_samples == combined_repeat_b_samples) && ok;
    ok = expect_true("combined-core-output-differs-from-ym-only-output",
                     combined_repeat_a_samples != ym_only_a_samples) && ok;

    EmulatorCore boundary_core;
    boundary_core.initialize();
    program_deterministic_mix(boundary_core);
    while (boundary_core.available_audio_samples() < EmulatorCore::kAudioBufferCapacitySamples) {
        boundary_core.step_scanline();
    }
    ok = expect_equal_size("audio-ring-buffer-fills-to-fixed-capacity",
                           boundary_core.available_audio_samples(),
                           EmulatorCore::kAudioBufferCapacitySamples) && ok;
    boundary_core.step_scanline();
    ok = expect_equal_size("audio-ring-buffer-does-not-grow-past-capacity",
                           boundary_core.available_audio_samples(),
                           EmulatorCore::kAudioBufferCapacitySamples) && ok;
    ok = expect_true("buffer-overflow-latches-apu-overrun-bit", boundary_core.bus().apu().overrun()) && ok;

#if defined(SUPER_Z80_HAS_SDL)
    ok = run_sdl_boundary_checks() && ok;
#else
    ok = expect_true("sdl-boundary-checks-skipped-when-sdl-is-unavailable", true) && ok;
#endif

    return ok ? 0 : 1;
}
