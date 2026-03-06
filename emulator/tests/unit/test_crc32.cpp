#include "crc32.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iomanip>
#include <iostream>

namespace {

bool expect_equal(const char* name, uint32_t actual, uint32_t expected) {
    if (actual != expected) {
        std::cerr << "[FAIL] " << name << " expected=0x" << std::hex << std::uppercase
                  << expected << " actual=0x" << actual << std::dec << std::nouppercase
                  << std::endl;
        return false;
    }

    std::cout << "[PASS] " << name << " crc32=0x" << std::hex << std::uppercase << actual
              << std::dec << std::nouppercase << std::endl;
    return true;
}

} // namespace

int main() {
    bool ok = true;

    const uint32_t empty_crc = superz80::crc32(nullptr, 0);
    ok = expect_equal("empty", empty_crc, 0x00000000U) && ok;

    constexpr char kDigits[] = "123456789";
    const uint32_t digits_crc = superz80::crc32(kDigits, std::strlen(kDigits));
    ok = expect_equal("123456789", digits_crc, 0xCBF43926U) && ok;

    constexpr std::array<uint8_t, 9> kBuffer = {0x00, 0x01, 0x02, 0x03, 0x10,
                                                 0x20, 0x30, 0x40, 0xFF};
    const uint32_t buffer_crc = superz80::crc32(kBuffer.data(), kBuffer.size());
    ok = expect_equal("fixed-buffer", buffer_crc, 0x026A0A13U) && ok;

    return ok ? 0 : 1;
}
