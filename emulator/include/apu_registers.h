#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace superz80 {

struct APUToneRegisterState {
    uint8_t period_low = 0x00U;
    uint8_t period_high = 0x00U;
};

struct APUToneRuntimeState {
    uint16_t divider_counter = 0x0000U;
    bool phase_high = false;
};

struct APUNoiseRuntimeState {
    uint16_t divider_counter = 0x0010U;
    uint16_t lfsr = 0x4000U;
};

struct APUSampleOutputState {
    int16_t current_sample = 0;
};

struct APURegisterState {
    static constexpr std::size_t kToneChannelCount = 3U;
    static constexpr std::size_t kVolumeChannelCount = 4U;
    static constexpr uint8_t kNoiseControlMask = 0x07U;
    static constexpr uint8_t kVolumeMask = 0x0FU;
    static constexpr uint8_t kToneHighMask = 0x0FU;
    static constexpr uint8_t kControlWritableMask = 0x07U;
    static constexpr uint8_t kControlReadableMask = 0x03U;
    static constexpr uint8_t kControlResetStateBit = 0x04U;
    static constexpr uint8_t kControlOverrunBit = 0x80U;

    std::array<APUToneRegisterState, kToneChannelCount> tone = {{
        {},
        {},
        {}
    }};
    uint8_t noise_control = 0x00U;
    std::array<uint8_t, kVolumeChannelCount> volume = {{
        0x0FU,
        0x0FU,
        0x0FU,
        0x0FU
    }};
    uint8_t control = 0x00U;
};

} // namespace superz80
