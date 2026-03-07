#include "bus.h"
#include "irq_controller.h"
#include "vblank.h"

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
    superz80::VBlank vblank(irq);

    vblank.reset();
    vblank.update_scanline(192U);
    ok = expect_true("vblank-begins-at-scanline-192", vblank.active()) && ok;

    vblank.reset();
    vblank.update_scanline(192U);
    vblank.update_scanline(0U);
    ok = expect_false("vblank-ends-at-scanline-0", vblank.active()) && ok;

    irq.reset();
    vblank.reset();
    vblank.update_scanline(192U);
    ok = expect_equal_u8("irq-requested-on-vblank-begin", irq.status(), superz80::VBlank::kIrqBit) && ok;

    irq.reset();
    vblank.reset();
    vblank.update_scanline(192U);
    irq.acknowledge(superz80::VBlank::kIrqBit);
    vblank.update_scanline(193U);
    vblank.update_scanline(261U);
    ok = expect_equal_u8("no-repeated-irq-within-vblank", irq.status(), 0x00U) && ok;

    superz80::Bus bus;
    bus.reset();
    bus.vblank().update_scanline(192U);
    bus.vdp().step_scanline(192U);
    ok = expect_equal_u8("bus-vdp-status-exposes-vblank", bus.read_port(superz80::Bus::kVdpStatusPort), 0x01U) && ok;
    bus.vblank().update_scanline(0U);
    bus.vdp().step_scanline(0U);
    ok = expect_equal_u8("bus-vdp-status-clears-outside-vblank", bus.read_port(superz80::Bus::kVdpStatusPort), 0x00U) && ok;
    bus.write_port(superz80::Bus::kVdpStatusPort, 0xFFU);
    ok = expect_equal_u8("vdp-status-write-ignored", bus.read_port(superz80::Bus::kVdpStatusPort), 0x00U) && ok;

    return ok ? 0 : 1;
}
