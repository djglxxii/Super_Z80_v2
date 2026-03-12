#ifndef SUPER_Z80_SDL_AUDIO_OUTPUT_H
#define SUPER_Z80_SDL_AUDIO_OUTPUT_H

#include <cstddef>
#include <cstdint>
#include <mutex>
#include <vector>

#include "emulator_core.h"

#if defined(SUPER_Z80_HAS_SDL)
#include <SDL.h>
#endif

namespace superz80 {

class SdlAudioOutput {
public:
    using Sample = EmulatorCore::AudioSample;

    static constexpr std::size_t kDefaultBufferCapacitySamples = 4096U;
    static constexpr uint16_t kDefaultDeviceBufferSamples = 1024U;

    explicit SdlAudioOutput(std::size_t buffer_capacity_samples = kDefaultBufferCapacitySamples);
    ~SdlAudioOutput();

    bool initialize();
    void start();
    void shutdown();
    void clear();

    std::size_t enqueue_samples(const Sample* samples, std::size_t sample_count);
    std::size_t read_samples_or_silence(Sample* destination, std::size_t sample_count);

    std::size_t buffered_samples() const;
    std::size_t dropped_samples() const;
    std::size_t underflow_count() const;

private:
#if defined(SUPER_Z80_HAS_SDL)
    static void audio_callback(void* userdata, Uint8* stream, int length);
#endif

    std::vector<Sample> buffer_;
    std::size_t read_index_;
    std::size_t write_index_;
    std::size_t size_;
    std::size_t dropped_samples_;
    std::size_t underflow_count_;
    mutable std::mutex mutex_;
#if defined(SUPER_Z80_HAS_SDL)
    SDL_AudioDeviceID device_;
    bool initialized_;
#endif
};

} // namespace superz80

#endif
