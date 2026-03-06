#include "crc32.h"

#include <cstddef>
#include <cstdint>

namespace superz80 {

uint32_t crc32(const void* data, std::size_t size) {
    if (size == 0 || data == nullptr) {
        return 0U;
    }

    constexpr uint32_t kPolynomial = 0xEDB88320U;
    uint32_t crc = 0xFFFFFFFFU;

    const auto* bytes = static_cast<const uint8_t*>(data);
    for (std::size_t i = 0; i < size; ++i) {
        crc ^= bytes[i];
        for (int bit = 0; bit < 8; ++bit) {
            if ((crc & 1U) != 0U) {
                crc = (crc >> 1U) ^ kPolynomial;
            } else {
                crc >>= 1U;
            }
        }
    }

    return crc ^ 0xFFFFFFFFU;
}

} // namespace superz80
