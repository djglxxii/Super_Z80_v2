#include "bus.h"
#include "io.h"

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

    superz80::IO io;
    ok = expect_equal("io-pad1-idle-default", io.read(superz80::IO::kPad1Port), 0xFFU) && ok;
    ok = expect_equal("io-pad1-sys-idle-default", io.read(superz80::IO::kPad1SysPort), 0xFFU) && ok;

    io.set_button(superz80::IO::Button::Up, true);
    ok = expect_equal("io-up-press-clears-pad1-bit0", io.read(superz80::IO::kPad1Port), 0xFEU) && ok;
    io.set_button(superz80::IO::Button::Up, false);
    ok = expect_equal("io-up-release-restores-idle", io.read(superz80::IO::kPad1Port), 0xFFU) && ok;

    io.set_button(superz80::IO::Button::Start, true);
    ok = expect_equal("io-start-press-clears-pad1-sys-bit0", io.read(superz80::IO::kPad1SysPort), 0xFEU) && ok;
    io.set_button(superz80::IO::Button::Start, false);
    ok = expect_equal("io-start-release-restores-idle", io.read(superz80::IO::kPad1SysPort), 0xFFU) && ok;

    io.set_button(superz80::IO::Button::Up, true);
    io.set_button(superz80::IO::Button::Left, true);
    io.set_button(superz80::IO::Button::A, true);
    io.set_button(superz80::IO::Button::B, true);
    ok = expect_equal("io-simultaneous-pad1-combination", io.read(superz80::IO::kPad1Port), 0xCAU) && ok;

    io.reset();
    ok = expect_equal("io-reset-restores-pad1-idle", io.read(superz80::IO::kPad1Port), 0xFFU) && ok;
    ok = expect_equal("io-reset-restores-pad1-sys-idle", io.read(superz80::IO::kPad1SysPort), 0xFFU) && ok;

    superz80::Bus bus;
    ok = expect_equal("bus-pad1-idle-visible", bus.read_port(superz80::Bus::kPad1Port), 0xFFU) && ok;
    ok = expect_equal("bus-pad1-sys-idle-visible", bus.read_port(superz80::Bus::kPad1SysPort), 0xFFU) && ok;

    bus.set_controller_button(superz80::IO::Button::Right, true);
    bus.set_controller_button(superz80::IO::Button::B, true);
    bus.set_controller_button(superz80::IO::Button::Start, true);
    ok = expect_equal("bus-pad1-reads-host-updated-state", bus.read_port(superz80::Bus::kPad1Port), 0xD7U) && ok;
    ok = expect_equal("bus-pad1-sys-reads-host-updated-state", bus.read_port(superz80::Bus::kPad1SysPort), 0xFEU) && ok;

    bus.write_port(superz80::Bus::kPad1Port, 0x00U);
    bus.write_port(superz80::Bus::kPad1SysPort, 0x00U);
    ok = expect_equal("controller-ports-ignore-direct-writes-pad1", bus.read_port(superz80::Bus::kPad1Port), 0xD7U) && ok;
    ok = expect_equal("controller-ports-ignore-direct-writes-pad1-sys", bus.read_port(superz80::Bus::kPad1SysPort), 0xFEU) && ok;

    return ok ? 0 : 1;
}
