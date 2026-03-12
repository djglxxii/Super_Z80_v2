#include "emulator_core.h"

#include <array>
#include <cstddef>
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

} // namespace

int main() {
    bool ok = true;

    EmulatorCore core;
    core.initialize();

    constexpr std::array<uint8_t, 4> kRom = {0x3EU, 0x01U, 0xD3U, 0xE0U};
    core.load_rom(kRom.data(), kRom.size());
    core.bus().write(0xC000U, 0x12U);
    core.bus().write(0xC001U, 0x34U);
    core.bus().write(0xFFFFU, 0xABU);

    const EmulatorCore::RomSnapshot rom_snapshot = core.rom_snapshot();
    const EmulatorCore::RamSnapshot ram_snapshot = core.ram_snapshot();

    ok = expect_equal_u8("rom-snapshot-byte-0", rom_snapshot[0x0000U], 0x3EU) && ok;
    ok = expect_equal_u8("rom-snapshot-byte-1", rom_snapshot[0x0001U], 0x01U) && ok;
    ok = expect_equal_u8("rom-snapshot-open-bus-fill", rom_snapshot[0x0010U], 0xFFU) && ok;
    ok = expect_equal_u8("ram-snapshot-byte-0", ram_snapshot[0x0000U], 0x12U) && ok;
    ok = expect_equal_u8("ram-snapshot-byte-1", ram_snapshot[0x0001U], 0x34U) && ok;
    ok = expect_equal_u8("ram-snapshot-last-byte", ram_snapshot[0x3FFFU], 0xABU) && ok;

    return ok ? 0 : 1;
}
