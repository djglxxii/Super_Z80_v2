#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace superz80 {

class Memory {
public:
    static constexpr std::size_t kSize = 65536;

    Memory();

    uint8_t read(uint16_t address) const;
    void write(uint16_t address, uint8_t value);
    void reset();

private:
    std::array<uint8_t, kSize> ram_;
};

} // namespace superz80
