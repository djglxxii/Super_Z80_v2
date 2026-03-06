#include "memory.h"

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

    superz80::Memory memory;

    ok = expect_equal("initial-0x0000", memory.read(0x0000), 0x00U) && ok;
    ok = expect_equal("initial-0xFFFF", memory.read(0xFFFF), 0x00U) && ok;

    memory.write(0x1234, 0xABU);
    ok = expect_equal("write-read-same-value", memory.read(0x1234), 0xABU) && ok;

    memory.write(0x1000, 0x11U);
    memory.write(0x2000, 0x22U);
    ok = expect_equal("independent-0x1000", memory.read(0x1000), 0x11U) && ok;
    ok = expect_equal("independent-0x2000", memory.read(0x2000), 0x22U) && ok;

    memory.write(0x3000, 0x55U);
    memory.reset();
    ok = expect_equal("reset-clears-memory", memory.read(0x3000), 0x00U) && ok;

    return ok ? 0 : 1;
}
