#include "bus.h"
#include "cpu.h"
#include "dma.h"
#include "scheduler.h"

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
    rom[0x0009U] = 0x3AU;
    rom[0x000AU] = 0x10U;
    rom[0x000BU] = 0xC2U;
    rom[0x000CU] = 0x3CU;
    rom[0x000DU] = 0x32U;
    rom[0x000EU] = 0x10U;
    rom[0x000FU] = 0xC2U;
    rom[0x0010U] = 0xC3U;
    rom[0x0011U] = 0x08U;
    rom[0x0012U] = 0x00U;

    rom[0x0038U] = 0xDBU;
    rom[0x0039U] = 0xF0U;
    rom[0x003AU] = 0x32U;
    rom[0x003BU] = 0x11U;
    rom[0x003CU] = 0xC2U;
    rom[0x003DU] = 0x3EU;
    rom[0x003EU] = 0x01U;
    rom[0x003FU] = 0xD3U;
    rom[0x0040U] = 0xF2U;
    rom[0x0041U] = 0xEDU;
    rom[0x0042U] = 0x4DU;

    return rom;
}

} // namespace

int main() {
    bool ok = true;

    superz80::Bus bus;
    superz80::CPU cpu(bus);
    superz80::Scheduler scheduler(cpu, bus.vdp(), bus.vblank(), bus.dma(), bus.apu(),
                                  bus.ym2151());

    const auto rom = make_rom();

    bus.reset();
    bus.load_rom(rom.data(), rom.size());
    bus.write(0xC100U, 0x11U);
    bus.write(0xC101U, 0x22U);
    bus.write(0xC102U, 0x33U);
    bus.write_port(superz80::Bus::kDmaSrcLowPort, 0x00U);
    bus.write_port(superz80::Bus::kDmaSrcHighPort, 0xC1U);
    bus.write_port(superz80::Bus::kDmaDstLowPort, 0x20U);
    bus.write_port(superz80::Bus::kDmaDstHighPort, 0xC1U);
    bus.write_port(superz80::Bus::kDmaLengthPort, 0x03U);
    bus.write_port(superz80::Bus::kDmaControlPort, superz80::DMA::kControlStartBit);
    cpu.reset();
    scheduler.reset();

    for (uint32_t scanline = 0U; scanline < 191U; ++scanline) {
        scheduler.step_scanline();
    }

    ok = expect_equal_u8("dma-copies-first-byte-before-vblank", bus.read(0xC120U), 0x11U) && ok;
    ok = expect_equal_u8("dma-copies-second-byte-before-vblank", bus.read(0xC121U), 0x22U) && ok;
    ok = expect_equal_u8("dma-copies-third-byte-before-vblank", bus.read(0xC122U), 0x33U) && ok;
    ok = expect_equal_u8("irq-handler-has-not-run-before-vblank", bus.read(0xC210U), 0x00U) && ok;
    ok = expect_equal_u8("irq-status-idle-before-boundary", bus.read_port(superz80::Bus::kIrqStatusPort), 0x00U) && ok;

    scheduler.step_scanline();
    ok = expect_equal_u8("vblank-requests-irq-at-scanline-192",
                         bus.read_port(superz80::Bus::kIrqStatusPort),
                         superz80::VBlank::kIrqBit) && ok;
    ok = expect_equal_u8("cpu-remains-halted-until-following-scanline", bus.read(0xC210U), 0x00U) && ok;

    scheduler.step_scanline();
    ok = expect_equal_u8("irq-handler-observes-vblank-request", bus.read(0xC211U), 0x01U) && ok;
    ok = expect_equal_u8("irq-acknowledge-clears-request", bus.read_port(superz80::Bus::kIrqStatusPort), 0x00U) && ok;
    ok = expect_equal_u8("cpu-resumes-and-executes-post-irq-work", bus.read(0xC210U), 0x01U) && ok;
    ok = expect_true("dma-is-idle-after-transfer-completes", !bus.dma().busy()) && ok;

    return ok ? 0 : 1;
}
