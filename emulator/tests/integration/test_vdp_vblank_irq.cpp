#include "bus.h"
#include "cpu.h"
#include "scheduler.h"
#include "vblank.h"

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

bool expect_true(const char* name, bool value) {
    if (!value) {
        std::cerr << "[FAIL] " << name << " expected=true actual=false" << std::endl;
        return false;
    }

    std::cout << "[PASS] " << name << " value=true" << std::endl;
    return true;
}

std::array<uint8_t, 0x48U> make_rom() {
    std::array<uint8_t, 0x48U> rom{};

    rom[0x0000U] = 0x31U;
    rom[0x0001U] = 0xFEU;
    rom[0x0002U] = 0xD0U;
    rom[0x0003U] = 0x3EU;
    rom[0x0004U] = 0x01U;
    rom[0x0005U] = 0xD3U;
    rom[0x0006U] = 0xF1U;
    rom[0x0007U] = 0xFBU;
    rom[0x0008U] = 0x76U;
    rom[0x0009U] = 0xC3U;
    rom[0x000AU] = 0x08U;
    rom[0x000BU] = 0x00U;

    rom[0x0038U] = 0xDBU;
    rom[0x0039U] = 0xE0U;
    rom[0x003AU] = 0x32U;
    rom[0x003BU] = 0x20U;
    rom[0x003CU] = 0xC2U;
    rom[0x003DU] = 0xDBU;
    rom[0x003EU] = 0xF0U;
    rom[0x003FU] = 0x32U;
    rom[0x0040U] = 0x21U;
    rom[0x0041U] = 0xC2U;
    rom[0x0042U] = 0x3EU;
    rom[0x0043U] = 0x01U;
    rom[0x0044U] = 0xD3U;
    rom[0x0045U] = 0xF2U;
    rom[0x0046U] = 0xEDU;
    rom[0x0047U] = 0x4DU;

    return rom;
}

} // namespace

int main() {
    bool ok = true;

    superz80::Bus bus;
    superz80::CPU cpu(bus);
    superz80::Scheduler scheduler(cpu, bus.vdp(), bus.vblank(), bus.dma(), bus.ym2151());
    const auto rom = make_rom();

    bus.reset();
    bus.load_rom(rom.data(), rom.size());
    cpu.reset();
    scheduler.reset();

    for (uint32_t scanline = 0U; scanline < 191U; ++scanline) {
        scheduler.step_scanline();
    }

    ok = expect_true("frame-ready-latches-at-scanline-191", bus.vdp().frame_ready()) && ok;

    scheduler.step_scanline();
    ok = expect_true("vblank-starts-at-scanline-192", bus.vblank().active()) && ok;
    ok = expect_equal_u8("irq-controller-receives-vblank-request",
                         bus.read_port(superz80::Bus::kIrqStatusPort),
                         superz80::VBlank::kIrqBit) && ok;

    scheduler.step_scanline();
    ok = expect_equal_u8("cpu-vector-handler-sees-latched-vblank-and-frame-ready", bus.read(0xC220U), 0x03U) && ok;
    ok = expect_equal_u8("cpu-vector-handler-sees-pending-irq-status", bus.read(0xC221U), 0x01U) && ok;
    ok = expect_equal_u8("acknowledge-clears-vblank-irq", bus.read_port(superz80::Bus::kIrqStatusPort), 0x00U) && ok;

    return ok ? 0 : 1;
}
