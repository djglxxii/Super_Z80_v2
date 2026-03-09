#include "bus.h"
#include "cpu.h"
#include "scheduler.h"

#include <array>
#include <cstdint>
#include <iostream>

namespace {

bool expect_equal_u32(const char* name, uint32_t actual, uint32_t expected) {
    if (actual != expected) {
        std::cerr << "[FAIL] " << name << " expected=" << expected << " actual=" << actual
                  << std::endl;
        return false;
    }

    std::cout << "[PASS] " << name << " value=" << actual << std::endl;
    return true;
}

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

} // namespace

int main() {
    bool ok = true;

    superz80::Bus bus;
    superz80::CPU cpu(bus);
    superz80::Scheduler scheduler(cpu, bus.vdp(), bus.vblank(), bus.dma(), bus.ym2151());

    scheduler.reset();
    ok = expect_equal_u32("reset-frame", scheduler.frame(), 0U) && ok;
    ok = expect_equal_u32("reset-scanline", scheduler.scanline(), 0U) && ok;

    scheduler.step_scanline();
    ok = expect_equal_u32("scanline-increment", scheduler.scanline(), 1U) && ok;
    ok = expect_equal_u32("frame-still-zero-after-one-scanline", scheduler.frame(), 0U) && ok;
    ok = expect_equal_u32("ym2151-tick-hook-called-on-scanline",
                         static_cast<uint32_t>(bus.ym2151().tick_call_count()),
                         1U) && ok;
    ok = expect_equal_u32("ym2151-fixed-cycle-budget-used-on-scanline",
                         static_cast<uint32_t>(bus.ym2151().accumulated_cycles()),
                         superz80::Scheduler::kYm2151CyclesPerScanline) && ok;

    scheduler.reset();
    for (uint32_t i = 0U; i < superz80::Scheduler::kScanlinesPerFrame; ++i) {
        scheduler.step_scanline();
    }

    ok = expect_equal_u32("frame-wrap-scanline-reset", scheduler.scanline(), 0U) && ok;
    ok = expect_equal_u32("frame-wrap-frame-increment", scheduler.frame(), 1U) && ok;

    constexpr std::array<uint8_t, 5> kRom = {
        0x3EU, 0x44U, // LD A,0x44
        0xD3U, 0x20U, // OUT (0x20),A
        0x76U         // HALT
    };

    bus.reset();
    bus.load_rom(kRom.data(), kRom.size());
    cpu.reset();
    scheduler.reset();
    scheduler.step_scanline();

    ok = expect_equal_u8("cpu-steps-during-scanline", bus.read_port(0x20U), 0x44U) && ok;

    bus.reset();
    cpu.reset();
    scheduler.reset();
    for (uint32_t i = 0U; i < superz80::VBlank::kBeginScanline; ++i) {
        scheduler.step_scanline();
    }

    ok = expect_equal_u32("vblank-begins-at-scanline-192", scheduler.scanline(), 192U) && ok;
    ok = expect_equal_u8("scheduler-drives-vdp-vblank-status", bus.read_port(superz80::Bus::kVdpStatusPort), 0x03U) && ok;
    ok = expect_equal_u8("scheduler-requests-vblank-irq", bus.read_port(superz80::Bus::kIrqStatusPort), 0x01U) && ok;

    bus.reset();
    cpu.reset();
    scheduler.reset();
    for (uint32_t i = 0U; i < 191U; ++i) {
        scheduler.step_scanline();
    }

    ok = expect_equal_u32("frame-ready-trigger-scanline", scheduler.scanline(), 191U) && ok;
    ok = expect_equal_u8("scheduler-sets-frame-ready-at-scanline-191",
                         bus.read_port(superz80::Bus::kVdpStatusPort),
                         0x02U) && ok;
    ok = expect_equal_u8("reading-vdp-status-clears-frame-ready",
                         bus.read_port(superz80::Bus::kVdpStatusPort),
                         0x00U) && ok;

    bus.reset();
    cpu.reset();
    scheduler.reset();
    bus.write(0xC060U, 0x2AU);
    bus.write_port(superz80::Bus::kDmaSrcLowPort, 0x60U);
    bus.write_port(superz80::Bus::kDmaSrcHighPort, 0xC0U);
    bus.write_port(superz80::Bus::kDmaDstLowPort, 0x70U);
    bus.write_port(superz80::Bus::kDmaDstHighPort, 0xC0U);
    bus.write_port(superz80::Bus::kDmaLengthPort, 0x01U);
    bus.write_port(superz80::Bus::kDmaControlPort, superz80::DMA::kControlStartBit);
    scheduler.step_scanline();

    ok = expect_equal_u8("scheduler-steps-dma-on-scanline", bus.read(0xC070U), 0x2AU) && ok;
    ok = expect_equal_u8("scheduler-clears-dma-busy-after-final-byte",
                         bus.read_port(superz80::Bus::kDmaControlPort),
                         0x00U) && ok;

    return ok ? 0 : 1;
}
