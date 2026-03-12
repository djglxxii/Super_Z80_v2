#include "sdl_audio_output.h"

#include <algorithm>

namespace superz80 {

SdlAudioOutput::SdlAudioOutput(std::size_t buffer_capacity_samples)
    : buffer_(buffer_capacity_samples, 0),
      read_index_(0U),
      write_index_(0U),
      size_(0U),
      dropped_samples_(0U),
      underflow_count_(0U),
      mutex_()
#if defined(SUPER_Z80_HAS_SDL)
      ,
      device_(0U),
      initialized_(false)
#endif
{
}

SdlAudioOutput::~SdlAudioOutput() {
    shutdown();
}

bool SdlAudioOutput::initialize() {
#if defined(SUPER_Z80_HAS_SDL)
    if (SDL_WasInit(0) == 0U) {
        if (SDL_Init(SDL_INIT_AUDIO) != 0) {
            return false;
        }
    } else if ((SDL_WasInit(SDL_INIT_AUDIO) & SDL_INIT_AUDIO) == 0U) {
        if (SDL_InitSubSystem(SDL_INIT_AUDIO) != 0) {
            return false;
        }
    }

    SDL_AudioSpec desired = {};
    desired.freq = static_cast<int>(superz80::APU::kSampleRateHz);
    desired.format = AUDIO_S16LSB;
    desired.channels = 1;
    desired.samples = kDefaultDeviceBufferSamples;
    desired.callback = &SdlAudioOutput::audio_callback;
    desired.userdata = this;

    SDL_AudioSpec obtained = {};
    device_ = SDL_OpenAudioDevice(nullptr, 0, &desired, &obtained, 0);
    if (device_ == 0U) {
        return false;
    }

    if (obtained.freq != desired.freq || obtained.format != desired.format ||
        obtained.channels != desired.channels) {
        SDL_CloseAudioDevice(device_);
        device_ = 0U;
        return false;
    }

    initialized_ = true;
    return true;
#else
    return false;
#endif
}

void SdlAudioOutput::start() {
#if defined(SUPER_Z80_HAS_SDL)
    if (initialized_) {
        SDL_PauseAudioDevice(device_, 0);
    }
#endif
}

void SdlAudioOutput::shutdown() {
#if defined(SUPER_Z80_HAS_SDL)
    if (device_ != 0U) {
        SDL_CloseAudioDevice(device_);
        device_ = 0U;
    }
    initialized_ = false;
#endif
}

void SdlAudioOutput::clear() {
    std::lock_guard<std::mutex> lock(mutex_);
    read_index_ = 0U;
    write_index_ = 0U;
    size_ = 0U;
}

std::size_t SdlAudioOutput::enqueue_samples(const Sample* samples, std::size_t sample_count) {
    if (samples == nullptr || sample_count == 0U || buffer_.empty()) {
        return 0U;
    }

    std::lock_guard<std::mutex> lock(mutex_);
    std::size_t appended = 0U;
    while (appended < sample_count && size_ < buffer_.size()) {
        buffer_[write_index_] = samples[appended];
        write_index_ = (write_index_ + 1U) % buffer_.size();
        ++size_;
        ++appended;
    }

    dropped_samples_ += sample_count - appended;
    return appended;
}

std::size_t SdlAudioOutput::read_samples_or_silence(Sample* destination, std::size_t sample_count) {
    if (destination == nullptr || sample_count == 0U) {
        return 0U;
    }

    std::fill_n(destination, sample_count, static_cast<Sample>(0));

    std::lock_guard<std::mutex> lock(mutex_);
    const std::size_t copied = std::min(sample_count, size_);
    for (std::size_t index = 0U; index < copied; ++index) {
        destination[index] = buffer_[read_index_];
        read_index_ = (read_index_ + 1U) % buffer_.size();
    }
    size_ -= copied;

    if (copied < sample_count) {
        ++underflow_count_;
    }

    return copied;
}

std::size_t SdlAudioOutput::buffered_samples() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return size_;
}

std::size_t SdlAudioOutput::dropped_samples() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return dropped_samples_;
}

std::size_t SdlAudioOutput::underflow_count() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return underflow_count_;
}

#if defined(SUPER_Z80_HAS_SDL)
void SdlAudioOutput::audio_callback(void* userdata, Uint8* stream, int length) {
    SdlAudioOutput* output = static_cast<SdlAudioOutput*>(userdata);
    if (output == nullptr || stream == nullptr || length <= 0) {
        return;
    }

    const std::size_t sample_count = static_cast<std::size_t>(length) / sizeof(Sample);
    output->read_samples_or_silence(reinterpret_cast<Sample*>(stream), sample_count);
}
#endif

} // namespace superz80
