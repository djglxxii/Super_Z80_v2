#include "bus.h"
#include "vdp.h"

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

bool expect_equal_u16(const char* name, uint16_t actual, uint16_t expected) {
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

    superz80::VDP vdp;
    vdp.reset();
    vdp.write_port(superz80::VDP::kDataPort, 0xA5U);
    ok = expect_equal_u8("vram-write-stores-byte", vdp.vram(0x0000U), 0xA5U) && ok;
    ok = expect_equal_u16("vram-pointer-advanced-after-write", vdp.vram_pointer(), 0x0001U) && ok;

    superz80::Bus bus;
    bus.reset();
    bus.write_port(superz80::Bus::kVdpDataPort, 0x3CU);
    for (uint32_t i = 0U; i < 65535U; ++i) {
        bus.write_port(superz80::Bus::kVdpDataPort, 0x00U);
    }
    ok = expect_equal_u8("vram-read-returns-written-value",
                         bus.read_port(superz80::Bus::kVdpDataPort),
                         0x3CU) && ok;

    vdp.reset();
    vdp.write_port(superz80::VDP::kDataPort, 0x10U);
    vdp.write_port(superz80::VDP::kDataPort, 0x11U);
    vdp.write_port(superz80::VDP::kDataPort, 0x12U);
    ok = expect_equal_u16("vram-pointer-increments-for-three-writes", vdp.vram_pointer(), 0x0003U) && ok;
    ok = expect_equal_u8("vram-byte-0", vdp.vram(0x0000U), 0x10U) && ok;
    ok = expect_equal_u8("vram-byte-1", vdp.vram(0x0001U), 0x11U) && ok;
    ok = expect_equal_u8("vram-byte-2", vdp.vram(0x0002U), 0x12U) && ok;

    vdp.reset();
    vdp.write_port(superz80::VDP::kDataPort, 0xFEU);
    vdp.reset();
    ok = expect_equal_u8("reset-clears-vram-first-byte", vdp.vram(0x0000U), 0x00U) && ok;
    ok = expect_equal_u8("reset-clears-vram-second-byte", vdp.vram(0x0001U), 0x00U) && ok;
    ok = expect_equal_u16("reset-clears-vram-pointer", vdp.vram_pointer(), 0x0000U) && ok;

    vdp.reset();
    for (uint32_t scanline = 1U; scanline <= superz80::VDP::kFrameReadyScanline; ++scanline) {
        vdp.step_scanline(scanline);
    }
    ok = expect_true("frame-ready-true-at-scanline-191", vdp.frame_ready()) && ok;
    ok = expect_equal_u8("status-exposes-frame-ready", vdp.read_port(superz80::VDP::kStatusPort), 0x02U) && ok;
    ok = expect_false("reading-status-clears-frame-ready", vdp.frame_ready()) && ok;
    ok = expect_equal_u8("status-cleared-after-read", vdp.read_port(superz80::VDP::kStatusPort), 0x00U) && ok;

    return ok ? 0 : 1;
}
