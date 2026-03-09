#include "bus.h"
#include "cpu.h"
#include "scheduler.h"
#include "ym2151.h"

#include <array>
#include <cstdint>
#include <iostream>

namespace {

bool expect_equal_u8(const char* name, uint8_t actual, uint8_t expected) {
    if (actual != expected) {
        std::cerr << "[FAIL] " << name << " expected=0x" << std::hex << std::uppercase
                  << static_cast<unsigned int>(expected) << " actual=0x"
                  << static_cast<unsigned int>(actual) << std::dec << std::nouppercase
                  << std::endl;
        return false;
    }

    std::cout << "[PASS] " << name << " value=0x" << std::hex << std::uppercase
              << static_cast<unsigned int>(actual) << std::dec << std::nouppercase
              << std::endl;
    return true;
}

bool expect_equal_u64(const char* name, uint64_t actual, uint64_t expected) {
    if (actual != expected) {
        std::cerr << "[FAIL] " << name << " expected=" << expected << " actual=" << actual
                  << std::endl;
        return false;
    }

    std::cout << "[PASS] " << name << " value=" << actual << std::endl;
    return true;
}

bool expect_equal_u32(const char* name, uint32_t actual, uint32_t expected) {
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

bool expect_true(const char* name, bool value) {
    if (!value) {
        std::cerr << "[FAIL] " << name << " expected=true actual=false" << std::endl;
        return false;
    }

    std::cout << "[PASS] " << name << " value=true" << std::endl;
    return true;
}

bool expect_false(const char* name, bool value) {
    if (value) {
        std::cerr << "[FAIL] " << name << " expected=false actual=true" << std::endl;
        return false;
    }

    std::cout << "[PASS] " << name << " value=false" << std::endl;
    return true;
}

void write_register(superz80::YM2151& ym2151, uint8_t reg, uint8_t value) {
    ym2151.write_port(superz80::YM2151::kRegisterSelectPort, reg);
    ym2151.write_port(superz80::YM2151::kRegisterDataPort, value);
}

} // namespace

int main() {
    bool ok = true;

    superz80::YM2151 ym2151;
    ok = expect_equal_u8("reset-selected-register-default", ym2151.selected_register(), 0x00U) && ok;
    ok = expect_equal_u8("reset-register-storage-default", ym2151.register_value(0x20U), 0x00U) && ok;
    ok = expect_equal_u8("reset-channel-algorithm-default", ym2151.channel(0).algorithm, 0x00U) && ok;
    ok = expect_false("reset-operator-key-on-default", ym2151.channel(0).operators[0].key_on) && ok;
    ok = expect_equal_u64("reset-operator-phase-default",
                          ym2151.channel(0).operators[0].phase_counter,
                          0U) && ok;
    ok = expect_equal_u32("reset-timer-a-latch-default", ym2151.timer_a().latch, 0U) && ok;
    ok = expect_equal_u32("reset-timer-b-latch-default", ym2151.timer_b().latch, 0U) && ok;
    ok = expect_equal_u8("reset-ym2151-status-default", ym2151.status(), 0x00U) && ok;
    ok = expect_false("reset-ym2151-irq-pending-default", ym2151.irq_pending()) && ok;
    ok = expect_equal_i16("reset-current-sample-default", ym2151.current_sample(), 0) && ok;

    ym2151.write_port(superz80::YM2151::kRegisterSelectPort, 0x20U);
    ok = expect_equal_u8("port-0x70-selects-register", ym2151.selected_register(), 0x20U) && ok;
    ym2151.write_port(superz80::YM2151::kRegisterDataPort, 0x7FU);
    ok = expect_equal_u8("port-0x71-writes-selected-register", ym2151.register_value(0x20U), 0x7FU) && ok;
    ok = expect_equal_u8("channel-algorithm-updates-from-register-write",
                         ym2151.channel(0).algorithm,
                         0x07U) && ok;
    ok = expect_equal_u8("channel-feedback-updates-from-register-write",
                         ym2151.channel(0).feedback,
                         0x07U) && ok;

    ym2151.write_port(superz80::YM2151::kRegisterSelectPort, 0x21U);
    ym2151.write_port(superz80::YM2151::kRegisterDataPort, 0x55U);
    ok = expect_equal_u8("register-storage-keeps-separate-values", ym2151.register_value(0x20U), 0x7FU) && ok;
    ok = expect_equal_u8("new-selected-register-updates-separately", ym2151.register_value(0x21U), 0x55U) && ok;
    ok = expect_equal_u8("channel-1-algorithm-decodes-low-bits", ym2151.channel(1).algorithm, 0x05U) && ok;
    ok = expect_equal_u8("channel-1-feedback-decodes-next-bits", ym2151.channel(1).feedback, 0x02U) && ok;

    ym2151.write_port(superz80::YM2151::kRegisterSelectPort, 0x28U);
    ym2151.write_port(superz80::YM2151::kRegisterDataPort, 0x50U);
    ok = expect_true("key-on-bit-4-enables-operator-0", ym2151.channel(0).operators[0].key_on) && ok;
    ok = expect_false("key-on-bit-5-clears-operator-1", ym2151.channel(0).operators[1].key_on) && ok;
    ok = expect_true("key-on-bit-6-enables-operator-2", ym2151.channel(0).operators[2].key_on) && ok;
    ok = expect_false("key-on-bit-7-clears-operator-3", ym2151.channel(0).operators[3].key_on) && ok;

    ym2151.write_port(superz80::YM2151::kRegisterSelectPort, 0x30U);
    ym2151.write_port(superz80::YM2151::kRegisterDataPort, 0xAAU);
    ym2151.write_port(superz80::YM2151::kRegisterSelectPort, 0x38U);
    ym2151.write_port(superz80::YM2151::kRegisterDataPort, 0x0DU);
    ok = expect_equal_u64("frequency-register-pair-updates-channel-frequency",
                          ym2151.channel(0).frequency,
                          0x01AAU) && ok;
    ok = expect_equal_u8("frequency-register-pair-updates-channel-block",
                         ym2151.channel(0).block,
                         0x03U) && ok;

    ym2151.write_port(superz80::YM2151::kRegisterSelectPort, 0x40U);
    ym2151.write_port(superz80::YM2151::kRegisterDataPort, 0xB2U);
    ym2151.write_port(superz80::YM2151::kRegisterSelectPort, 0x60U);
    ym2151.write_port(superz80::YM2151::kRegisterDataPort, 0x7FU);
    ym2151.write_port(superz80::YM2151::kRegisterSelectPort, 0x80U);
    ym2151.write_port(superz80::YM2151::kRegisterDataPort, 0xDFU);
    ym2151.write_port(superz80::YM2151::kRegisterSelectPort, 0xA0U);
    ym2151.write_port(superz80::YM2151::kRegisterDataPort, 0x1BU);
    ym2151.write_port(superz80::YM2151::kRegisterSelectPort, 0xC0U);
    ym2151.write_port(superz80::YM2151::kRegisterDataPort, 0x17U);
    ym2151.write_port(superz80::YM2151::kRegisterSelectPort, 0xE0U);
    ym2151.write_port(superz80::YM2151::kRegisterDataPort, 0xA9U);
    ok = expect_equal_u8("operator-register-write-updates-detune",
                         ym2151.channel(0).operators[0].detune,
                         0x03U) && ok;
    ok = expect_equal_u8("operator-register-write-updates-multiple",
                         ym2151.channel(0).operators[0].multiple,
                         0x02U) && ok;
    ok = expect_equal_u8("operator-register-write-updates-total-level",
                         ym2151.channel(0).operators[0].total_level,
                         0x7FU) && ok;
    ok = expect_equal_u8("operator-register-write-updates-key-scale",
                         ym2151.channel(0).operators[0].key_scale,
                         0x03U) && ok;
    ok = expect_equal_u8("operator-register-write-updates-attack-rate",
                         ym2151.channel(0).operators[0].attack_rate,
                         0x1FU) && ok;
    ok = expect_equal_u8("operator-register-write-updates-decay-rate",
                         ym2151.channel(0).operators[0].decay_rate,
                         0x1BU) && ok;
    ok = expect_equal_u8("operator-register-write-updates-sustain-rate",
                         ym2151.channel(0).operators[0].sustain_rate,
                         0x17U) && ok;
    ok = expect_equal_u8("operator-register-write-updates-sustain-level",
                         ym2151.channel(0).operators[0].sustain_level,
                         0x0AU) && ok;
    ok = expect_equal_u8("operator-register-write-updates-release-rate",
                         ym2151.channel(0).operators[0].release_rate,
                         0x09U) && ok;

    write_register(ym2151, 0x10U, 0x12U);
    write_register(ym2151, 0x11U, 0x03U);
    write_register(ym2151, 0x12U, 0x34U);
    ok = expect_equal_u32("timer-a-latch-register-pair-updates-state",
                          ym2151.timer_a().latch,
                          0x004BU) && ok;
    ok = expect_equal_u32("timer-b-latch-register-updates-state",
                          ym2151.timer_b().latch,
                          0x34U) && ok;
    ok = expect_equal_u8("timer-register-storage-kept-raw-high",
                         ym2151.register_value(0x10U),
                         0x12U) && ok;
    ok = expect_equal_u8("timer-register-storage-kept-raw-low",
                         ym2151.register_value(0x11U),
                         0x03U) && ok;
    ok = expect_equal_u8("timer-register-storage-kept-raw-b",
                         ym2151.register_value(0x12U),
                         0x34U) && ok;

    ym2151.tick(4U);
    ok = expect_equal_u32("tick-stores-phase-increment-for-keyed-operator-0",
                          ym2151.channel(0).operators[0].phase_increment,
                          6998U) && ok;
    ok = expect_equal_u32("tick-stores-phase-increment-for-keyed-operator-2",
                          ym2151.channel(0).operators[2].phase_increment,
                          3499U) && ok;
    ok = expect_equal_u64("tick-advances-keyed-operators-only",
                          ym2151.channel(0).operators[0].phase_counter,
                          27992U) && ok;
    ok = expect_equal_u64("tick-leaves-cleared-operators-stationary",
                          ym2151.channel(0).operators[1].phase_counter,
                          0U) && ok;
    ok = expect_equal_u64("tick-advances-all-keyed-operators",
                          ym2151.channel(0).operators[2].phase_counter,
                          13996U) && ok;
    ok = expect_true("tick-generates-nonzero-internal-fm-sample",
                     ym2151.current_sample() != 0) && ok;

    superz80::YM2151 timer_disabled;
    write_register(timer_disabled, 0x10U, 0xFFU);
    write_register(timer_disabled, 0x11U, 0x03U);
    timer_disabled.tick(32U);
    ok = expect_equal_u32("disabled-timer-a-does-not-advance-counter",
                          timer_disabled.timer_a().counter,
                          0U) && ok;
    ok = expect_equal_u8("disabled-timer-a-does-not-set-status",
                         timer_disabled.status(),
                         0x00U) && ok;

    superz80::YM2151 timer_enabled;
    write_register(timer_enabled, 0x10U, 0xFFU);
    write_register(timer_enabled, 0x11U, 0x02U);
    write_register(timer_enabled, 0x14U, 0x01U);
    timer_enabled.tick(1U);
    ok = expect_true("timer-a-enable-bit-latches-enabled", timer_enabled.timer_a().enabled) && ok;
    ok = expect_equal_u32("enabled-timer-a-advances-counter",
                          timer_enabled.timer_a().counter,
                          1U) && ok;

    superz80::YM2151 timer_overflow;
    write_register(timer_overflow, 0x10U, 0xFFU);
    write_register(timer_overflow, 0x11U, 0x03U);
    write_register(timer_overflow, 0x14U, 0x01U);
    timer_overflow.tick(2U);
    ok = expect_equal_u8("timer-a-overflow-sets-status-bit",
                         timer_overflow.status(),
                         superz80::YM2151::kTimerAStatusBit) && ok;
    ok = expect_equal_u8("status-read-via-port-0x70-returns-overflow-bits",
                         timer_overflow.read_port(superz80::YM2151::kRegisterSelectPort),
                         superz80::YM2151::kTimerAStatusBit) && ok;
    ok = expect_equal_u32("timer-a-overflow-wraps-counter",
                          timer_overflow.timer_a().counter,
                          0U) && ok;

    superz80::YM2151 timer_irq_disabled;
    write_register(timer_irq_disabled, 0x10U, 0xFFU);
    write_register(timer_irq_disabled, 0x11U, 0x03U);
    write_register(timer_irq_disabled, 0x14U, 0x01U);
    timer_irq_disabled.tick(2U);
    ok = expect_false("timer-a-overflow-with-irq-disabled-has-no-pending-irq",
                      timer_irq_disabled.irq_pending()) && ok;

    superz80::YM2151 timer_irq_enabled;
    write_register(timer_irq_enabled, 0x10U, 0xFFU);
    write_register(timer_irq_enabled, 0x11U, 0x03U);
    write_register(timer_irq_enabled, 0x14U, 0x05U);
    timer_irq_enabled.tick(2U);
    ok = expect_true("timer-a-overflow-with-irq-enabled-sets-pending-irq",
                     timer_irq_enabled.irq_pending()) && ok;
    write_register(timer_irq_enabled, 0x14U, 0x14U);
    ok = expect_equal_u8("timer-a-clear-control-clears-status",
                         timer_irq_enabled.status(),
                         0x00U) && ok;
    ok = expect_false("timer-a-clear-control-clears-pending-irq",
                      timer_irq_enabled.irq_pending()) && ok;
    ok = expect_false("timer-a-clear-control-disables-timer-when-start-bit-cleared",
                      timer_irq_enabled.timer_a().enabled) && ok;

    superz80::YM2151 timer_b_irq_enabled;
    write_register(timer_b_irq_enabled, 0x12U, 0xFFU);
    write_register(timer_b_irq_enabled, 0x14U, 0x0AU);
    timer_b_irq_enabled.tick(16U);
    ok = expect_equal_u8("timer-b-overflow-sets-status-bit",
                         timer_b_irq_enabled.status(),
                         superz80::YM2151::kTimerBStatusBit) && ok;
    ok = expect_true("timer-b-overflow-with-irq-enabled-sets-pending-irq",
                     timer_b_irq_enabled.irq_pending()) && ok;
    write_register(timer_b_irq_enabled, 0x14U, 0x20U);
    ok = expect_equal_u8("timer-b-clear-control-clears-status",
                         timer_b_irq_enabled.status(),
                         0x00U) && ok;
    ok = expect_false("timer-b-clear-control-clears-pending-irq",
                      timer_b_irq_enabled.irq_pending()) && ok;

    superz80::YM2151 repeat_a;
    superz80::YM2151 repeat_b;
    write_register(repeat_a, 0x10U, 0xFFU);
    write_register(repeat_a, 0x11U, 0x03U);
    write_register(repeat_a, 0x14U, 0x05U);
    write_register(repeat_b, 0x10U, 0xFFU);
    write_register(repeat_b, 0x11U, 0x03U);
    write_register(repeat_b, 0x14U, 0x05U);
    repeat_a.tick(1U);
    repeat_a.tick(1U);
    repeat_b.tick(2U);
    ok = expect_equal_u32("repeatability-matches-timer-a-counter",
                          repeat_a.timer_a().counter,
                          repeat_b.timer_a().counter) && ok;
    ok = expect_equal_u8("repeatability-matches-status",
                         repeat_a.status(),
                         repeat_b.status()) && ok;
    ok = expect_equal_u8("repeatability-matches-irq-pending",
                         repeat_a.irq_pending() ? 1U : 0U,
                         repeat_b.irq_pending() ? 1U : 0U) && ok;

    superz80::YM2151 sample_repeat_a;
    superz80::YM2151 sample_repeat_b;
    for (superz80::YM2151* device : {&sample_repeat_a, &sample_repeat_b}) {
        write_register(*device, 0x20U, 0x07U);
        write_register(*device, 0x30U, 0xAAU);
        write_register(*device, 0x38U, 0x0DU);
        write_register(*device, 0x40U, 0x01U);
        write_register(*device, 0x48U, 0x01U);
        write_register(*device, 0x50U, 0x01U);
        write_register(*device, 0x58U, 0x01U);
        write_register(*device, 0x80U, 0xDFU);
        write_register(*device, 0x88U, 0xDFU);
        write_register(*device, 0x90U, 0xDFU);
        write_register(*device, 0x98U, 0xDFU);
        write_register(*device, 0xE0U, 0x0FU);
        write_register(*device, 0xE8U, 0x0FU);
        write_register(*device, 0xF0U, 0x0FU);
        write_register(*device, 0xF8U, 0x0FU);
        write_register(*device, 0x28U, 0xF0U);
    }
    constexpr std::array<uint32_t, 5> kSampleSteps = {1U, 2U, 5U, 3U, 7U};
    for (uint32_t step : kSampleSteps) {
        sample_repeat_a.tick(step);
        sample_repeat_b.tick(step);
        ok = expect_equal_i16("repeatability-matches-current-sample",
                              sample_repeat_a.current_sample(),
                              sample_repeat_b.current_sample()) && ok;
        ok = expect_equal_u64("repeatability-matches-phase-counter",
                              sample_repeat_a.channel(0).operators[0].phase_counter,
                              sample_repeat_b.channel(0).operators[0].phase_counter) && ok;
    }

    superz80::YM2151 key_off_release;
    write_register(key_off_release, 0x20U, 0x07U);
    write_register(key_off_release, 0x30U, 0x80U);
    write_register(key_off_release, 0x38U, 0x08U);
    write_register(key_off_release, 0x40U, 0x01U);
    write_register(key_off_release, 0x80U, 0xDFU);
    write_register(key_off_release, 0xE0U, 0x0FU);
    write_register(key_off_release, 0x28U, 0x10U);
    key_off_release.tick(40U);
    ok = expect_true("key-on-enters-envelope-attack-path",
                     key_off_release.channel(0).operators[0].envelope_state !=
                         superz80::YM2151EnvelopeState::Off) && ok;
    write_register(key_off_release, 0x28U, 0x00U);
    key_off_release.tick(64U);
    ok = expect_equal_i16("key-off-release-eventually-silences-sample",
                          key_off_release.current_sample(),
                          0) && ok;
    ok = expect_equal_u8("key-off-release-reaches-envelope-off",
                         static_cast<uint8_t>(key_off_release.channel(0).operators[0].envelope_state),
                         static_cast<uint8_t>(superz80::YM2151EnvelopeState::Off)) && ok;

    superz80::Bus bus;
    bus.write_port(superz80::Bus::kYm2151RegisterSelectPort, 0x20U);
    bus.write_port(superz80::Bus::kYm2151RegisterDataPort, 0x05U);
    ok = expect_equal_u8("bus-routes-register-select-port", bus.ym2151().selected_register(), 0x20U) && ok;
    ok = expect_equal_u8("bus-routes-register-data-port", bus.ym2151().register_value(0x20U), 0x05U) && ok;
    ok = expect_equal_u8("bus-routes-decoded-algorithm", bus.ym2151().channel(0).algorithm, 0x05U) && ok;
    bus.write_port(superz80::Bus::kYm2151RegisterSelectPort, 0x10U);
    bus.write_port(superz80::Bus::kYm2151RegisterDataPort, 0xFFU);
    bus.write_port(superz80::Bus::kYm2151RegisterSelectPort, 0x11U);
    bus.write_port(superz80::Bus::kYm2151RegisterDataPort, 0x03U);
    bus.write_port(superz80::Bus::kYm2151RegisterSelectPort, 0x14U);
    bus.write_port(superz80::Bus::kYm2151RegisterDataPort, 0x05U);
    bus.ym2151().tick(2U);
    ok = expect_true("bus-exposes-ym2151-irq-pending-hook", bus.ym2151_irq_pending()) && ok;

    constexpr std::array<uint8_t, 17> kRom = {
        0x3EU, 0x20U, // LD A,0x20
        0xD3U, 0x70U, // OUT (0x70),A
        0x3EU, 0x05U, // LD A,0x05
        0xD3U, 0x71U, // OUT (0x71),A
        0x3EU, 0x28U, // LD A,0x28
        0xD3U, 0x70U, // OUT (0x70),A
        0x3EU, 0xF0U, // LD A,0xF0
        0xD3U, 0x71U, // OUT (0x71),A
        0x76U         // HALT
    };

    bus.reset();
    bus.load_rom(kRom.data(), kRom.size());
    superz80::CPU cpu(bus);
    cpu.reset();
    for (int step = 0; step < 8; ++step) {
        cpu.step();
    }
    ok = expect_equal_u8("cpu-out-selects-ym2151-register", bus.ym2151().selected_register(), 0x28U) && ok;
    ok = expect_equal_u8("cpu-out-writes-ym2151-register", bus.ym2151().register_value(0x20U), 0x05U) && ok;
    ok = expect_true("cpu-out-writes-key-on-state", bus.ym2151().channel(0).operators[3].key_on) && ok;

    bus.reset();
    cpu.reset();
    superz80::Scheduler scheduler(cpu, bus.vdp(), bus.vblank(), bus.dma(), bus.ym2151());
    scheduler.reset();
    scheduler.step_scanline();
    ok = expect_equal_u64("scheduler-calls-ym2151-tick-hook", bus.ym2151().tick_call_count(), 1U) && ok;
    ok = expect_equal_u64("scheduler-uses-fixed-ym2151-cycle-budget",
                          bus.ym2151().accumulated_cycles(),
                          superz80::Scheduler::kYm2151CyclesPerScanline) && ok;

    return ok ? 0 : 1;
}
