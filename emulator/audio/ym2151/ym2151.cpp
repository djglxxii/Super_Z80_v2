#include "ym2151.h"

namespace superz80 {

YM2151::YM2151()
    : selected_register_(0U),
      registers_{},
      channels_{},
      timer_a_{},
      timer_b_{},
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

    for (YM2151Channel& channel : channels_) {
        for (YM2151Operator& op : channel.operators) {
            if (!op.key_on) {
                continue;
            }

            op.phase_counter += cycles * operator_phase_increment(channel, op);
        }
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

uint64_t YM2151::tick_call_count() const {
    return tick_call_count_;
}

uint64_t YM2151::accumulated_cycles() const {
    return accumulated_cycles_;
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
            channel.operators[op_index].key_on =
                ((value >> (4U + static_cast<uint8_t>(op_index))) & 0x01U) != 0U;
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

YM2151Operator& YM2151::operator_for_register_block(uint8_t base, uint8_t reg) {
    const uint8_t operator_index = static_cast<uint8_t>(reg - base);
    const std::size_t channel_index = operator_index % kChannelCount;
    const std::size_t operator_slot = operator_index / kChannelCount;
    return channels_[channel_index].operators[operator_slot];
}

YM2151Operator YM2151::make_default_operator() {
    return {
        0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, 0U, false, 0U,
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

} // namespace superz80
