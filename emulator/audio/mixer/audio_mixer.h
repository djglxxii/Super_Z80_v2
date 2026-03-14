#pragma once

#include <cstdint>

#include "apu.h"

namespace superz80 {

class AudioMixer {
public:
    struct Snapshot {
        int16_t current_sample = 0;
    };

    AudioMixer(const AudioSampleSource& psg, const AudioSampleSource& ym);

    void reset();
    void tick();

    int16_t current_sample() const;
    Snapshot snapshot() const;
    void restore(const Snapshot& snapshot);

private:
    const AudioSampleSource& psg_;
    const AudioSampleSource& ym_;

    int16_t last_sample_;
};

} // namespace superz80
