#include "bus.h"
#include "cpu.h"
#include "dma.h"
#include "scheduler.h"

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

bool expect_false(const char* name, bool value) {
    if (value) {
        std::cerr << "[FAIL] " << name << " expected=false actual=true" << std::endl;
        return false;
    }

    std::cout << "[PASS] " << name << " value=false" << std::endl;
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

} // namespace

int main() {
    bool ok = true;

    superz80::Bus bus;

    bus.reset();
    bus.write_port(superz80::Bus::kDmaSrcLowPort, 0x34U);
    bus.write_port(superz80::Bus::kDmaSrcHighPort, 0x12U);
    bus.write_port(superz80::Bus::kDmaDstLowPort, 0x78U);
    bus.write_port(superz80::Bus::kDmaDstHighPort, 0x56U);
    bus.write_port(superz80::Bus::kDmaLengthPort, 0x09U);

    ok = expect_equal_u8("read-src-low", bus.read_port(superz80::Bus::kDmaSrcLowPort), 0x34U) && ok;
    ok = expect_equal_u8("read-src-high", bus.read_port(superz80::Bus::kDmaSrcHighPort), 0x12U) && ok;
    ok = expect_equal_u8("read-dst-low", bus.read_port(superz80::Bus::kDmaDstLowPort), 0x78U) && ok;
    ok = expect_equal_u8("read-dst-high", bus.read_port(superz80::Bus::kDmaDstHighPort), 0x56U) && ok;
    ok = expect_equal_u8("read-length", bus.read_port(superz80::Bus::kDmaLengthPort), 0x09U) && ok;
    ok = expect_equal_u8("control-idle-when-not-started", bus.read_port(superz80::Bus::kDmaControlPort), 0x00U) && ok;

    bus.reset();
    bus.write(0xC000U, 0x11U);
    bus.write(0xC001U, 0x22U);
    bus.write(0xC002U, 0x33U);
    bus.write_port(superz80::Bus::kDmaSrcLowPort, 0x00U);
    bus.write_port(superz80::Bus::kDmaSrcHighPort, 0xC0U);
    bus.write_port(superz80::Bus::kDmaDstLowPort, 0x10U);
    bus.write_port(superz80::Bus::kDmaDstHighPort, 0xC0U);
    bus.write_port(superz80::Bus::kDmaLengthPort, 0x03U);
    bus.write_port(superz80::Bus::kDmaControlPort, superz80::DMA::kControlStartBit);

    ok = expect_true("busy-after-start", bus.dma().busy()) && ok;
    ok = expect_equal_u8("control-busy-flag-after-start",
                         bus.read_port(superz80::Bus::kDmaControlPort),
                         superz80::DMA::kControlBusyBit) && ok;

    bus.dma().step();
    ok = expect_equal_u8("first-byte-copied", bus.read(0xC010U), 0x11U) && ok;
    ok = expect_equal_u8("length-decrements-after-step", bus.read_port(superz80::Bus::kDmaLengthPort), 0x02U) && ok;
    ok = expect_equal_u8("source-increments-after-step", bus.read_port(superz80::Bus::kDmaSrcLowPort), 0x01U) && ok;
    ok = expect_equal_u8("destination-increments-after-step", bus.read_port(superz80::Bus::kDmaDstLowPort), 0x11U) && ok;

    bus.dma().step();
    bus.dma().step();
    ok = expect_equal_u8("remaining-second-byte-copied", bus.read(0xC011U), 0x22U) && ok;
    ok = expect_equal_u8("remaining-third-byte-copied", bus.read(0xC012U), 0x33U) && ok;
    ok = expect_false("dma-idle-after-final-byte", bus.dma().busy()) && ok;
    ok = expect_equal_u8("control-idle-after-completion", bus.read_port(superz80::Bus::kDmaControlPort), 0x00U) && ok;
    ok = expect_equal_u8("length-zero-after-completion", bus.read_port(superz80::Bus::kDmaLengthPort), 0x00U) && ok;

    bus.reset();
    bus.write(0xC020U, 0xA5U);
    bus.write_port(superz80::Bus::kDmaSrcLowPort, 0x20U);
    bus.write_port(superz80::Bus::kDmaSrcHighPort, 0xC0U);
    bus.write_port(superz80::Bus::kDmaDstLowPort, 0x30U);
    bus.write_port(superz80::Bus::kDmaDstHighPort, 0xC0U);
    bus.write_port(superz80::Bus::kDmaLengthPort, 0x00U);
    bus.write_port(superz80::Bus::kDmaControlPort, superz80::DMA::kControlStartBit);
    bus.dma().step();
    ok = expect_false("zero-length-start-does-not-activate", bus.dma().busy()) && ok;
    ok = expect_equal_u8("zero-length-start-does-not-copy", bus.read(0xC030U), 0x00U) && ok;

    superz80::CPU cpu(bus);
    superz80::Scheduler scheduler(cpu, bus.vdp(), bus.vblank(), bus.dma());

    bus.reset();
    cpu.reset();
    scheduler.reset();
    bus.write(0xC040U, 0x5AU);
    bus.write(0xC041U, 0x6BU);
    bus.write_port(superz80::Bus::kDmaSrcLowPort, 0x40U);
    bus.write_port(superz80::Bus::kDmaSrcHighPort, 0xC0U);
    bus.write_port(superz80::Bus::kDmaDstLowPort, 0x50U);
    bus.write_port(superz80::Bus::kDmaDstHighPort, 0xC0U);
    bus.write_port(superz80::Bus::kDmaLengthPort, 0x02U);
    bus.write_port(superz80::Bus::kDmaControlPort, superz80::DMA::kControlStartBit);

    scheduler.step_scanline();
    ok = expect_equal_u8("scheduler-copies-one-byte-per-scanline-first", bus.read(0xC050U), 0x5AU) && ok;
    ok = expect_equal_u8("scheduler-does-not-copy-second-byte-early", bus.read(0xC051U), 0x00U) && ok;
    ok = expect_true("scheduler-leaves-dma-busy-until-length-zero", bus.dma().busy()) && ok;

    scheduler.step_scanline();
    ok = expect_equal_u8("scheduler-copies-one-byte-per-scanline-second", bus.read(0xC051U), 0x6BU) && ok;
    ok = expect_false("scheduler-completes-dma-after-required-scanlines", bus.dma().busy()) && ok;

    return ok ? 0 : 1;
}
