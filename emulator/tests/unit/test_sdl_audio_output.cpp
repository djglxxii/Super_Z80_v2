#include "sdl_audio_output.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>

#include <SDL.h>

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
        std::cerr << "[FAIL] " << name << " expected=" << expected << " actual=" << actual
                  << std::endl;
        return false;
    }

    std::cout << "[PASS] " << name << " value=" << actual << std::endl;
    return true;
}

} // namespace

int main() {
    bool ok = true;

    superz80::SdlAudioOutput fifo(4U);
    constexpr std::array<int16_t, 4> kOrderedSamples = {101, -202, 303, -404};
    ok = expect_equal_size("fifo-accepts-all-ordered-samples",
                           fifo.enqueue_samples(kOrderedSamples.data(), kOrderedSamples.size()),
                           kOrderedSamples.size()) &&
         ok;
    std::array<int16_t, 4> drained = {};
    ok = expect_equal_size("fifo-drains-ordered-samples",
                           fifo.read_samples_or_silence(drained.data(), drained.size()),
                           drained.size()) &&
         ok;
    for (std::size_t index = 0U; index < drained.size(); ++index) {
        ok = expect_equal_i16("fifo-preserves-sample-order", drained[index], kOrderedSamples[index]) && ok;
    }

    superz80::SdlAudioOutput underflow(4U);
    constexpr std::array<int16_t, 2> kPartialSamples = {1111, -2222};
    ok = expect_equal_size("underflow-enqueue-count",
                           underflow.enqueue_samples(kPartialSamples.data(), kPartialSamples.size()),
                           kPartialSamples.size()) &&
         ok;
    std::array<int16_t, 4> underflow_read = {7, 7, 7, 7};
    ok = expect_equal_size("underflow-read-copies-available-samples",
                           underflow.read_samples_or_silence(underflow_read.data(), underflow_read.size()),
                           kPartialSamples.size()) &&
         ok;
    ok = expect_equal_i16("underflow-preserves-first-sample", underflow_read[0], kPartialSamples[0]) && ok;
    ok = expect_equal_i16("underflow-preserves-second-sample", underflow_read[1], kPartialSamples[1]) && ok;
    ok = expect_equal_i16("underflow-fills-third-sample-with-silence", underflow_read[2], 0) && ok;
    ok = expect_equal_i16("underflow-fills-fourth-sample-with-silence", underflow_read[3], 0) && ok;
    ok = expect_equal_size("underflow-counter-increments", underflow.underflow_count(), 1U) && ok;

    superz80::SdlAudioOutput bounded(3U);
    constexpr std::array<int16_t, 4> kOverflowSamples = {1, 2, 3, 4};
    ok = expect_equal_size("bounded-buffer-drops-new-samples-when-full",
                           bounded.enqueue_samples(kOverflowSamples.data(), kOverflowSamples.size()),
                           3U) &&
         ok;
    ok = expect_equal_size("bounded-buffer-records-dropped-samples", bounded.dropped_samples(), 1U) && ok;
    std::array<int16_t, 3> bounded_read = {};
    bounded.read_samples_or_silence(bounded_read.data(), bounded_read.size());
    ok = expect_equal_i16("bounded-buffer-keeps-oldest-sample", bounded_read[0], 1) && ok;
    ok = expect_equal_i16("bounded-buffer-keeps-middle-sample", bounded_read[1], 2) && ok;
    ok = expect_equal_i16("bounded-buffer-keeps-last-fit-sample", bounded_read[2], 3) && ok;

    if (SDL_setenv("SDL_AUDIODRIVER", "dummy", 1) != 0) {
        std::cerr << "[FAIL] sdl-setenv-audiodriver " << SDL_GetError() << std::endl;
        return 1;
    }

    superz80::SdlAudioOutput device_output;
    ok = expect_true("dummy-sdl-device-initializes", device_output.initialize()) && ok;
    device_output.start();
    device_output.shutdown();
    SDL_Quit();

    return ok ? 0 : 1;
}
