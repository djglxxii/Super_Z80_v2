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

    io.write(0x10U, 0x55U);
    ok = expect_equal("write-read-port", io.read(0x10U), 0x55U) && ok;

    io.write(0x20U, 0x12U);
    io.write(0x21U, 0x34U);
    ok = expect_equal("independent-ports-first", io.read(0x20U), 0x12U) && ok;
    ok = expect_equal("independent-ports-second", io.read(0x21U), 0x34U) && ok;

    io.write(0xFEU, 0xAAU);
    io.reset();
    ok = expect_equal("reset-clears-port-0x10", io.read(0x10U), 0x00U) && ok;
    ok = expect_equal("reset-clears-port-0xFE", io.read(0xFEU), 0x00U) && ok;

    return ok ? 0 : 1;
}
