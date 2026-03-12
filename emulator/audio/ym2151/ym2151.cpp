#include "ym2151.h"

#include <array>
#include <cmath>

namespace superz80 {

namespace {

constexpr double kPi = 3.14159265358979323846;
constexpr uint32_t kSineTableSize = 1024U;
constexpr uint16_t kEnvelopeMax = 1024U;
constexpr uint32_t kPhaseIndexMask = kSineTableSize - 1U;

std::array<int16_t, kSineTableSize> build_sine_table() {
    std::array<int16_t, kSineTableSize> table{};
    for (std::size_t index = 0; index < table.size(); ++index) {
        const double radians =
            (static_cast<double>(index) * 2.0 * kPi) / static_cast<double>(table.size());
        table[index] = static_cast<int16_t>(std::lround(std::sin(radians) * 32767.0));
    }
    return table;
}

const std::array<int16_t, kSineTableSize>& sine_table() {
    static const std::array<int16_t, kSineTableSize> kTable = build_sine_table();
    return kTable;
}

uint16_t sustain_target_level(uint8_t sustain_level) {
    return static_cast<uint16_t>(((15U - static_cast<uint16_t>(sustain_level & 0x0FU)) *
                                  kEnvelopeMax) /
                                 15U);
}

uint16_t attack_step(const YM2151Operator& op) {
    return static_cast<uint16_t>(8U + (static_cast<uint16_t>(op.attack_rate) * 4U) +
                                 (static_cast<uint16_t>(op.key_scale) * 8U));
}

uint16_t decay_step(const YM2151Operator& op) {
    return static_cast<uint16_t>(1U + static_cast<uint16_t>(op.decay_rate));
}

uint16_t release_step(const YM2151Operator& op) {
    return static_cast<uint16_t>(1U + (static_cast<uint16_t>(op.release_rate) * 2U));
}

uint32_t feedback_modulation(const YM2151Channel& channel, const YM2151Operator& op) {
    if (channel.feedback == 0U) {
        return 0U;
    }

    const int32_t scaled = (static_cast<int32_t>(op.last_output) * static_cast<int32_t>(channel.feedback)) >> 8U;
    return static_cast<uint32_t>(scaled & static_cast<int32_t>(kPhaseIndexMask));
}

} // namespace

YM2151::YM2151()
    : selected_register_(0U),
      registers_{},
      channels_{},
      timer_a_{},
      timer_b_{},
      last_sample_(0),
      tick_call_count_(0U),
      accumulated_cycles_(0U) {
    reset();
}

void YM2151::reset() {
    selected_register_ = 0U;
    registers_.fill(0U);
    for (YM2151Channel& channel : channels_) {
        channel = make_default_channel();
    }
    timer_a_ = make_default_timer();
    timer_b_ = make_default_timer();
    last_sample_ = 0;
    tick_call_count_ = 0U;
    accumulated_cycles_ = 0U;
}

uint8_t YM2151::read_port(uint8_t port) const {
    if (port == kRegisterSelectPort) {
        return status();
    }

    if (port == kRegisterDataPort) {
        return registers_[selected_register_];
    }

    return 0xFFU;
}

void YM2151::write_port(uint8_t port, uint8_t value) {
    if (port == kRegisterSelectPort) {
        selected_register_ = value;
        return;
    }

    if (port == kRegisterDataPort) {
        registers_[selected_register_] = value;
        apply_register_write(selected_register_, value);
    }
}

void YM2151::tick(uint32_t cycles) {
    ++tick_call_count_;
    accumulated_cycles_ += cycles;

    advance_timer(timer_a_, cycles, timer_a_period(timer_a_.latch));
    advance_timer(timer_b_, cycles, timer_b_period(static_cast<uint8_t>(timer_b_.latch & 0x00FFU)));

    for (uint32_t cycle = 0U; cycle < cycles; ++cycle) {
        for (YM2151Channel& channel : channels_) {
            for (YM2151Operator& op : channel.operators) {
                advance_operator(channel, op);
            }
        }

        update_current_sample();
    }
}

uint8_t YM2151::selected_register() const {
    return selected_register_;
}

uint8_t YM2151::register_value(uint8_t reg) const {
    return registers_[reg];
}

const YM2151Channel& YM2151::channel(std::size_t index) const {
    return channels_[index];
}

const YM2151Timer& YM2151::timer_a() const {
    return timer_a_;
}

const YM2151Timer& YM2151::timer_b() const {
    return timer_b_;
}

uint8_t YM2151::status() const {
    uint8_t value = 0U;
    if (timer_a_.overflow) {
        value = static_cast<uint8_t>(value | kTimerAStatusBit);
    }
    if (timer_b_.overflow) {
        value = static_cast<uint8_t>(value | kTimerBStatusBit);
    }
    return value;
}

bool YM2151::irq_pending() const {
    return (timer_a_.overflow && timer_a_.irq_enabled) ||
           (timer_b_.overflow && timer_b_.irq_enabled);
}

int16_t YM2151::current_sample() const {
    return last_sample_;
}

uint64_t YM2151::tick_call_count() const {
    return tick_call_count_;
}

uint64_t YM2151::accumulated_cycles() const {
    return accumulated_cycles_;
}

YM2151::Snapshot YM2151::snapshot() const {
    Snapshot snapshot = {};
    snapshot.selected_register = selected_register_;
    snapshot.registers = registers_;
    for (std::size_t channel_index = 0U; channel_index < channels_.size(); ++channel_index) {
        uint8_t key_on_mask = 0U;
        for (std::size_t operator_index = 0U;
             operator_index < channels_[channel_index].operators.size();
             ++operator_index) {
            if (channels_[channel_index].operators[operator_index].key_on) {
                key_on_mask = static_cast<uint8_t>(key_on_mask | (1U << operator_index));
            }
        }

        snapshot.channels[channel_index] = {
            channels_[channel_index].frequency,
            channels_[channel_index].block,
            channels_[channel_index].algorithm,
            channels_[channel_index].feedback,
            key_on_mask,
        };
    }

    snapshot.timer_a = {
        timer_a_.latch,
        timer_a_.counter,
        timer_a_.enabled,
        timer_a_.overflow,
        timer_a_.irq_enabled,
    };
    snapshot.timer_b = {
        timer_b_.latch,
        timer_b_.counter,
        timer_b_.enabled,
        timer_b_.overflow,
        timer_b_.irq_enabled,
    };
    snapshot.status = status();
    snapshot.irq_pending = irq_pending();
    snapshot.current_sample = last_sample_;
    snapshot.tick_call_count = tick_call_count_;
    snapshot.accumulated_cycles = accumulated_cycles_;
    return snapshot;
}

void YM2151::apply_register_write(uint8_t reg, uint8_t value) {
    if (reg == kTimerAHighRegister) {
        update_timer_a_latch();
        return;
    }

    if (reg == kTimerALowRegister) {
        update_timer_a_latch();
        return;
    }

    if (reg == kTimerBRegister) {
        update_timer_b_latch();
        return;
    }

    if (reg == kTimerControlRegister) {
        apply_timer_control(value);
        return;
    }

    if (reg >= 0x20U && reg <= 0x27U) {
        YM2151Channel& channel = channels_[reg - 0x20U];
        channel.algorithm = value & 0x07U;
        channel.feedback = (value >> 3U) & 0x07U;
        return;
    }

    if (reg >= 0x28U && reg <= 0x2FU) {
        YM2151Channel& channel = channels_[reg - 0x28U];
        for (std::size_t op_index = 0; op_index < kOperatorsPerChannel; ++op_index) {
            YM2151Operator& op = channel.operators[op_index];
            const bool key_on = ((value >> (4U + static_cast<uint8_t>(op_index))) & 0x01U) != 0U;
            if (key_on && !op.key_on) {
                op.phase = 0U;
                op.phase_counter = 0U;
                op.phase_increment = operator_phase_increment(channel, op);
                op.envelope_level = 0U;
                op.envelope_state = YM2151EnvelopeState::Attack;
                op.last_output = 0;
            } else if (!key_on && op.key_on) {
                op.envelope_state = YM2151EnvelopeState::Release;
            }

            op.key_on = key_on;
        }
        return;
    }

    if (reg >= 0x30U && reg <= 0x37U) {
        YM2151Channel& channel = channels_[reg - 0x30U];
        channel.frequency = static_cast<uint16_t>((channel.frequency & 0x0300U) | value);
        return;
    }

    if (reg >= 0x38U && reg <= 0x3FU) {
        YM2151Channel& channel = channels_[reg - 0x38U];
        channel.frequency =
            static_cast<uint16_t>((channel.frequency & 0x00FFU) | ((value & 0x03U) << 8U));
        channel.block = static_cast<uint8_t>((value >> 2U) & 0x07U);
        return;
    }

    if (reg >= 0x40U && reg <= 0x5FU) {
        YM2151Operator& op = operator_for_register_block(0x40U, reg);
        op.detune = static_cast<uint8_t>((value >> 4U) & 0x07U);
        op.multiple = value & 0x0FU;
        return;
    }

    if (reg >= 0x60U && reg <= 0x7FU) {
        operator_for_register_block(0x60U, reg).total_level = value & 0x7FU;
        return;
    }

    if (reg >= 0x80U && reg <= 0x9FU) {
        YM2151Operator& op = operator_for_register_block(0x80U, reg);
        op.key_scale = static_cast<uint8_t>((value >> 6U) & 0x03U);
        op.attack_rate = value & 0x1FU;
        return;
    }

    if (reg >= 0xA0U && reg <= 0xBFU) {
        operator_for_register_block(0xA0U, reg).decay_rate = value & 0x1FU;
        return;
    }

    if (reg >= 0xC0U && reg <= 0xDFU) {
        operator_for_register_block(0xC0U, reg).sustain_rate = value & 0x1FU;
        return;
    }

    if (reg >= 0xE0U && reg <= 0xFFU) {
        YM2151Operator& op = operator_for_register_block(0xE0U, reg);
        op.sustain_level = static_cast<uint8_t>((value >> 4U) & 0x0FU);
        op.release_rate = value & 0x0FU;
    }
}

void YM2151::update_timer_a_latch() {
    timer_a_.latch = static_cast<uint16_t>(
        (static_cast<uint16_t>(registers_[kTimerAHighRegister]) << 2U) |
        static_cast<uint16_t>(registers_[kTimerALowRegister] & 0x03U));
}

void YM2151::update_timer_b_latch() {
    timer_b_.latch = registers_[kTimerBRegister];
}

void YM2151::apply_timer_control(uint8_t value) {
    if ((value & 0x10U) != 0U) {
        clear_timer_overflow(timer_a_);
    }

    if ((value & 0x20U) != 0U) {
        clear_timer_overflow(timer_b_);
    }

    timer_a_.irq_enabled = (value & 0x04U) != 0U;
    timer_b_.irq_enabled = (value & 0x08U) != 0U;

    timer_a_.enabled = (value & 0x01U) != 0U;
    timer_b_.enabled = (value & 0x02U) != 0U;

    if (timer_a_.enabled) {
        timer_a_.counter = 0U;
    }

    if (!timer_a_.enabled) {
        timer_a_.counter = 0U;
    }

    if (timer_b_.enabled) {
        timer_b_.counter = 0U;
    }

    if (!timer_b_.enabled) {
        timer_b_.counter = 0U;
    }
}

void YM2151::clear_timer_overflow(YM2151Timer& timer) {
    timer.overflow = false;
}

void YM2151::advance_timer(YM2151Timer& timer, uint32_t cycles, uint32_t period) {
    if (!timer.enabled || cycles == 0U) {
        return;
    }

    timer.counter += cycles;
    while (timer.counter >= period) {
        timer.counter -= period;
        timer.overflow = true;
    }
}

YM2151Timer YM2151::make_default_timer() {
    return {0U, 0U, false, false, false};
}

void YM2151::advance_operator(YM2151Channel& channel, YM2151Operator& op) {
    op.phase_increment = operator_phase_increment(channel, op);
    advance_envelope(op);

    if (!op.key_on && op.envelope_state == YM2151EnvelopeState::Off) {
        op.phase_increment = 0U;
        op.last_output = 0;
        return;
    }

    op.phase = (op.phase + op.phase_increment) & kPhaseMask;
    op.phase_counter += op.phase_increment;
}

void YM2151::advance_envelope(YM2151Operator& op) {
    switch (op.envelope_state) {
    case YM2151EnvelopeState::Off:
        op.envelope_level = 0U;
        break;
    case YM2151EnvelopeState::Attack: {
        const uint32_t next_level = static_cast<uint32_t>(op.envelope_level) + attack_step(op);
        if (next_level >= kEnvelopeMax) {
            op.envelope_level = kEnvelopeMax;
            op.envelope_state = YM2151EnvelopeState::Decay;
        } else {
            op.envelope_level = static_cast<uint16_t>(next_level);
        }
        break;
    }
    case YM2151EnvelopeState::Decay: {
        const uint16_t target = sustain_target_level(op.sustain_level);
        if (op.envelope_level <= target) {
            op.envelope_level = target;
            op.envelope_state = YM2151EnvelopeState::Sustain;
            break;
        }

        const uint16_t step = decay_step(op);
        op.envelope_level = (op.envelope_level > step) ? static_cast<uint16_t>(op.envelope_level - step)
                                                       : target;
        if (op.envelope_level <= target) {
            op.envelope_level = target;
            op.envelope_state = YM2151EnvelopeState::Sustain;
        }
        break;
    }
    case YM2151EnvelopeState::Sustain:
        op.envelope_level = sustain_target_level(op.sustain_level);
        break;
    case YM2151EnvelopeState::Release: {
        const uint16_t step = release_step(op);
        if (op.envelope_level <= step) {
            op.envelope_level = 0U;
            op.envelope_state = YM2151EnvelopeState::Off;
            op.last_output = 0;
        } else {
            op.envelope_level = static_cast<uint16_t>(op.envelope_level - step);
        }
        break;
    }
    }
}

int32_t YM2151::render_operator(YM2151Operator& op, int32_t modulation) const {
    if (op.envelope_state == YM2151EnvelopeState::Off || op.envelope_level == 0U) {
        op.last_output = 0;
        return 0;
    }

    const uint32_t base_phase = (op.phase >> kPhaseFractionBits) & kPhaseIndexMask;
    const uint32_t phase_index =
        static_cast<uint32_t>((static_cast<int32_t>(base_phase) + modulation) &
                              static_cast<int32_t>(kPhaseIndexMask));
    int32_t sample = sine_table()[phase_index];
    sample = (sample * static_cast<int32_t>(op.envelope_level)) / static_cast<int32_t>(kEnvelopeMax);

    const int32_t total_level_scale = 127 - static_cast<int32_t>(op.total_level & 0x7FU);
    sample = (sample * total_level_scale) / 127;
    op.last_output = clamp_to_i16(sample);
    return op.last_output;
}

int32_t YM2151::render_channel(YM2151Channel& channel) {
    YM2151Operator& op1 = channel.operators[0];
    YM2151Operator& op2 = channel.operators[1];
    YM2151Operator& op3 = channel.operators[2];
    YM2151Operator& op4 = channel.operators[3];

    const int32_t op1_out =
        render_operator(op1, static_cast<int32_t>(feedback_modulation(channel, op1)));
    const int32_t op2_from_op1 = render_operator(op2, op1_out >> 6U);
    const int32_t op3_from_op2 = render_operator(op3, op2_from_op1 >> 6U);
    const int32_t op4_from_op3 = render_operator(op4, op3_from_op2 >> 6U);
    const int32_t op3_from_op1 = render_operator(op3, op1_out >> 6U);
    const int32_t op4_from_mix = render_operator(op4, (op2_from_op1 + op3_from_op1) >> 6U);
    const int32_t op2_carrier = render_operator(op2, 0);
    const int32_t op3_carrier = render_operator(op3, 0);
    const int32_t op4_carrier = render_operator(op4, 0);

    switch (channel.algorithm & 0x07U) {
    case 0U:
        return op4_from_op3;
    case 1U:
        return op4_from_mix;
    case 2U:
        return render_operator(op4, (op2_from_op1 + op3_from_op2) >> 6U);
    case 3U:
        return render_operator(op4, (op2_from_op1 + op3_carrier) >> 6U);
    case 4U:
        return op3_from_op2 + op4_carrier;
    case 5U:
        return op2_from_op1 + op4_from_op3;
    case 6U:
        return op2_from_op1 + op3_carrier + op4_carrier;
    case 7U:
    default:
        return op1_out + op2_carrier + op3_carrier + op4_carrier;
    }
}

void YM2151::update_current_sample() {
    int32_t mixed_sample = 0;
    for (YM2151Channel& channel : channels_) {
        mixed_sample += render_channel(channel);
    }

    last_sample_ = clamp_to_i16(mixed_sample);
}

YM2151Operator& YM2151::operator_for_register_block(uint8_t base, uint8_t reg) {
    const uint8_t operator_index = static_cast<uint8_t>(reg - base);
    const std::size_t channel_index = operator_index % kChannelCount;
    const std::size_t operator_slot = operator_index / kChannelCount;
    return channels_[channel_index].operators[operator_slot];
}

YM2151Operator YM2151::make_default_operator() {
    return {
        0U,
        0U,
        0U,
        0U,
        0U,
        0U,
        0U,
        0U,
        0U,
        false,
        0U,
        0U,
        0U,
        0U,
        YM2151EnvelopeState::Off,
        0,
    };
}

YM2151Channel YM2151::make_default_channel() {
    YM2151Channel channel{};
    for (YM2151Operator& op : channel.operators) {
        op = make_default_operator();
    }
    channel.frequency = 0U;
    channel.block = 0U;
    channel.algorithm = 0U;
    channel.feedback = 0U;
    return channel;
}

uint32_t YM2151::operator_phase_increment(const YM2151Channel& channel,
                                          const YM2151Operator& op) {
    const uint32_t multiple = (op.multiple == 0U) ? 1U : op.multiple;
    const uint32_t pitch = (static_cast<uint32_t>(channel.block) << 10U) |
                           static_cast<uint32_t>(channel.frequency);
    return (pitch + 1U) * multiple;
}

uint32_t YM2151::timer_a_period(uint16_t latch) {
    const uint32_t countdown = 1024U - static_cast<uint32_t>(latch & 0x03FFU);
    return (countdown == 0U) ? 1024U : countdown;
}

uint32_t YM2151::timer_b_period(uint8_t latch) {
    const uint32_t countdown = 256U - static_cast<uint32_t>(latch);
    return ((countdown == 0U) ? 256U : countdown) * 16U;
}

int16_t YM2151::clamp_to_i16(int32_t value) {
    if (value > 32767) {
        return 32767;
    }
    if (value < -32768) {
        return -32768;
    }
    return static_cast<int16_t>(value);
}

} // namespace superz80
