#pragma once

#include <array>
#include <cstdint>

namespace superz80 {

class IO {
public:
    IO();

    void reset();

    uint8_t read(uint8_t port) const;
    void write(uint8_t port, uint8_t value);

private:
    std::array<uint8_t, 256> ports_;
};

} // namespace superz80
