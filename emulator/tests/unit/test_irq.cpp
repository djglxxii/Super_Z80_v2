#include "bus.h"
#include "cpu.h"
#include "irq_controller.h"

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

} // namespace

int main() {
    bool ok = true;

    superz80::IRQController irq;
    irq.request(0x01U);
    ok = expect_equal_u8("request-sets-bit", irq.status(), 0x01U) && ok;

    irq.reset();
    irq.request(0x01U);
    ok = expect_false("enable-mask-blocks-irq-line", irq.irq_line()) && ok;
    irq.set_enable(0x01U);
    ok = expect_true("enable-mask-allows-irq-line", irq.irq_line()) && ok;

    irq.reset();
    irq.request(0x02U);
    irq.acknowledge(0x02U);
    ok = expect_equal_u8("acknowledge-clears-bit", irq.status(), 0x00U) && ok;

    irq.reset();
    irq.request(0x01U);
    irq.request(0x04U);
    ok = expect_equal_u8("multiple-irq-bits-accumulate", irq.status(), 0x05U) && ok;

    superz80::Bus bus;
    bus.request_irq(0x04U);
    ok = expect_equal_u8("bus-status-port-exposes-irq-status",
                         bus.read_port(superz80::Bus::kIrqStatusPort),
                         0x04U) &&
         ok;
    bus.write_port(superz80::Bus::kIrqEnablePort, 0x04U);
    ok = expect_equal_u8("bus-enable-port-exposes-enable-mask",
                         bus.read_port(superz80::Bus::kIrqEnablePort),
                         0x04U) &&
         ok;
    ok = expect_true("bus-irq-line-follows-enabled-status", bus.irq_line()) && ok;
    bus.write_port(superz80::Bus::kIrqAckPort, 0x04U);
    ok = expect_equal_u8("bus-ack-port-clears-status",
                         bus.read_port(superz80::Bus::kIrqStatusPort),
                         0x00U) &&
         ok;
    ok = expect_false("bus-irq-line-clears-after-ack", bus.irq_line()) && ok;

    bus.write_port(superz80::Bus::kIrqEnablePort, 0x01U);
    bus.request_irq(0x01U);
    superz80::CPU cpu(bus);
    cpu.reset();
    cpu.set_int_line(bus.irq_line());
    ok = expect_true("cpu-exposes-driven-int-line", cpu.int_line()) && ok;

    bus.write_port(superz80::Bus::kIrqAckPort, 0x01U);
    cpu.set_int_line(bus.irq_line());
    ok = expect_false("cpu-int-line-clears-after-acknowledge", cpu.int_line()) && ok;

    bus.write_port(superz80::Bus::kIrqEnablePort, 0xFFU);
    bus.request_irq(0x08U);
    bus.reset();
    ok = expect_equal_u8("bus-reset-clears-irq-status",
                         bus.read_port(superz80::Bus::kIrqStatusPort),
                         0x00U) &&
         ok;
    ok = expect_equal_u8("bus-reset-clears-irq-enable",
                         bus.read_port(superz80::Bus::kIrqEnablePort),
                         0x00U) &&
         ok;

    return ok ? 0 : 1;
}
