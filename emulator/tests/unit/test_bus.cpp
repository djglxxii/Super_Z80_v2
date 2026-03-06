#include "bus.h"

#include <array>
#include <cstdint>
#include <iostream>

namespace {

bool expect_equal(const char* name, uint8_t actual, uint8_t expected) {
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
    constexpr std::array<uint8_t, 3> kRom = {0xAAU, 0xBBU, 0xCCU};

    bus.load_rom(kRom.data(), kRom.size());

    ok = expect_equal("rom-read-0x0000", bus.read(0x0000U), 0xAAU) && ok;
    ok = expect_equal("rom-read-0x0001", bus.read(0x0001U), 0xBBU) && ok;
    ok = expect_equal("rom-read-0x0002", bus.read(0x0002U), 0xCCU) && ok;

    ok = expect_equal("rom-defaults-to-open-bus", bus.read(0x0100U), 0xFFU) && ok;

    bus.write(0x0001U, 0x11U);
    ok = expect_equal("rom-write-ignored", bus.read(0x0001U), 0xBBU) && ok;

    bus.write(0xC000U, 0x42U);
    ok = expect_equal("ram-read-write", bus.read(0xC000U), 0x42U) && ok;

    bus.write(0xC000U, 0x10U);
    bus.write(0xD234U, 0x20U);
    ok = expect_equal("ram-address-independent-first", bus.read(0xC000U), 0x10U) && ok;
    ok = expect_equal("ram-address-independent-second", bus.read(0xD234U), 0x20U) && ok;

    ok = expect_equal("open-bus-read-0x8000", bus.read(0x8000U), 0xFFU) && ok;
    ok = expect_equal("open-bus-read-0xBFFF", bus.read(0xBFFFU), 0xFFU) && ok;

    bus.write(0xC100U, 0x55U);
    bus.write(0x8000U, 0x99U);
    ok = expect_equal("unmapped-write-does-not-change-ram", bus.read(0xC100U), 0x55U) && ok;
    ok = expect_equal("unmapped-write-does-not-change-rom", bus.read(0x0000U), 0xAAU) && ok;

    bus.write(0xC000U, 0x77U);
    bus.reset();
    ok = expect_equal("reset-clears-ram", bus.read(0xC000U), 0x00U) && ok;
    ok = expect_equal("reset-preserves-rom", bus.read(0x0002U), 0xCCU) && ok;

    return ok ? 0 : 1;
}
