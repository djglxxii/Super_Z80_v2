#include "bus.h"
#include "cpu.h"

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
    superz80::CPU cpu(bus);
    constexpr std::array<uint8_t, 7> kRom = {
        0x3EU, 0x55U, // LD A,0x55
        0xD3U, 0x10U, // OUT (0x10),A
        0xDBU, 0x10U, // IN A,(0x10)
        0x76U         // HALT
    };

    bus.load_rom(kRom.data(), kRom.size());
    cpu.reset();

    cpu.step();
    ok = expect_equal("ld-a-immediate-loads-register-a", cpu.get_register_a(), 0x55U) && ok;

    cpu.step();
    ok = expect_equal("out-writes-to-bus-port-space", bus.read_port(0x10U), 0x55U) && ok;

    cpu.step();
    ok = expect_equal("in-reads-from-bus-port-space", cpu.get_register_a(), 0x55U) && ok;

    cpu.step();
    ok = expect_true("halt-instruction-sets-halt-state", cpu.is_halted()) && ok;

    return ok ? 0 : 1;
}
