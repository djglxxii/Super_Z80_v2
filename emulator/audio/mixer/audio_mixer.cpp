#include "audio_mixer.h"

namespace superz80 {

AudioMixer::AudioMixer(const AudioSampleSource& psg, const AudioSampleSource& ym)
    : psg_(psg), ym_(ym), last_sample_(0) {}

void AudioMixer::reset() {
    last_sample_ = 0;
}

void AudioMixer::tick() {
    int32_t mixed = static_cast<int32_t>(psg_.current_sample()) +
                    static_cast<int32_t>(ym_.current_sample());

    if (mixed > 32767) {
        mixed = 32767;
    } else if (mixed < -32768) {
        mixed = -32768;
    }

    last_sample_ = static_cast<int16_t>(mixed);
}

int16_t AudioMixer::current_sample() const {
    return last_sample_;
}

AudioMixer::Snapshot AudioMixer::snapshot() const {
    return {last_sample_};
}

void AudioMixer::restore(const Snapshot& snapshot) {
    last_sample_ = snapshot.current_sample;
}

} // namespace superz80
