#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "apu.h"

namespace superz80 {

enum class YM2151EnvelopeState : uint8_t {
    Off = 0U,
    Attack,
    Decay,
    Sustain,
    Release,
};

struct YM2151Operator {
    uint8_t detune;
    uint8_t multiple;
    uint8_t total_level;
    uint8_t attack_rate;
    uint8_t decay_rate;
    uint8_t sustain_rate;
    uint8_t release_rate;
    uint8_t sustain_level;
    uint8_t key_scale;
    bool key_on;
    uint32_t phase;
    uint32_t phase_increment;
    uint32_t phase_counter;
    uint16_t envelope_level;
    YM2151EnvelopeState envelope_state;
    int16_t last_output;
};

struct YM2151Channel {
    std::array<YM2151Operator, 4> operators;
    uint16_t frequency;
    uint8_t block;
    uint8_t algorithm;
    uint8_t feedback;
};

struct YM2151Timer {
    uint16_t latch;
    uint32_t counter;
    bool enabled;
    bool overflow;
    bool irq_enabled;
};

class YM2151 : public AudioSampleSource {
public:
    static constexpr uint8_t kRegisterSelectPort = 0x70U;
    static constexpr uint8_t kRegisterDataPort = 0x71U;
    static constexpr uint8_t kPortStart = kRegisterSelectPort;
    static constexpr uint8_t kPortEnd = kRegisterDataPort;
    static constexpr std::size_t kRegisterCount = 256U;
    static constexpr std::size_t kChannelCount = 8U;
    static constexpr std::size_t kOperatorsPerChannel = 4U;
    static constexpr uint8_t kTimerAStatusBit = 0x01U;
    static constexpr uint8_t kTimerBStatusBit = 0x02U;

    YM2151();

    void reset();
    uint8_t read_port(uint8_t port) const;
    void write_port(uint8_t port, uint8_t value);
    void tick(uint32_t cycles);

    uint8_t selected_register() const;
    uint8_t register_value(uint8_t reg) const;
    const YM2151Channel& channel(std::size_t index) const;
    const YM2151Timer& timer_a() const;
    const YM2151Timer& timer_b() const;
    uint8_t status() const;
    bool irq_pending() const;
    int16_t current_sample() const override;
    uint64_t tick_call_count() const;
    uint64_t accumulated_cycles() const;

private:
    static constexpr uint32_t kSineTableSize = 1024U;
    static constexpr uint32_t kPhaseFractionBits = 10U;
    static constexpr uint32_t kPhaseIndexMask = kSineTableSize - 1U;
    static constexpr uint32_t kPhaseMask = (kSineTableSize << kPhaseFractionBits) - 1U;
    static constexpr uint16_t kEnvelopeMax = 1024U;

    static constexpr uint8_t kTimerAHighRegister = 0x10U;
    static constexpr uint8_t kTimerALowRegister = 0x11U;
    static constexpr uint8_t kTimerBRegister = 0x12U;
    static constexpr uint8_t kTimerControlRegister = 0x14U;

    void apply_register_write(uint8_t reg, uint8_t value);
    void update_timer_a_latch();
    void update_timer_b_latch();
    void apply_timer_control(uint8_t value);
    void clear_timer_overflow(YM2151Timer& timer);
    void advance_timer(YM2151Timer& timer, uint32_t cycles, uint32_t period);
    void advance_operator(YM2151Channel& channel, YM2151Operator& op);
    void advance_envelope(YM2151Operator& op);
    int32_t render_operator(YM2151Operator& op, int32_t modulation) const;
    int32_t render_channel(YM2151Channel& channel);
    void update_current_sample();
    static YM2151Timer make_default_timer();
    YM2151Operator& operator_for_register_block(uint8_t base, uint8_t reg);
    static YM2151Operator make_default_operator();
    static YM2151Channel make_default_channel();
    static uint32_t operator_phase_increment(const YM2151Channel& channel,
                                             const YM2151Operator& op);
    static uint32_t timer_a_period(uint16_t latch);
    static uint32_t timer_b_period(uint8_t latch);
    static int16_t clamp_to_i16(int32_t value);

    uint8_t selected_register_;
    std::array<uint8_t, kRegisterCount> registers_;
    std::array<YM2151Channel, kChannelCount> channels_;
    YM2151Timer timer_a_;
    YM2151Timer timer_b_;
    int16_t last_sample_;
    uint64_t tick_call_count_;
    uint64_t accumulated_cycles_;
};

} // namespace superz80
