#pragma once

#include <cstddef>
#include <cstdint>

namespace superz80 {

uint32_t crc32(const void* data, std::size_t size);

} // namespace superz80
