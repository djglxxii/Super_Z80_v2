#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "memory.h"

namespace superz80 {

class Bus {
public:
    static constexpr uint16_t kRomStart = 0x0000U;
    static constexpr uint16_t kRomEnd = 0x7FFFU;
    static constexpr uint16_t kUnmappedStart = 0x8000U;
    static constexpr uint16_t kUnmappedEnd = 0xBFFFU;
    static constexpr uint16_t kRamStart = 0xC000U;
    static constexpr uint16_t kRamEnd = 0xFFFFU;
    static constexpr std::size_t kRomSize = 0x8000U;
    static constexpr std::size_t kRamWindowSize = 0x4000U;

    Bus();

    void reset();
    void load_rom(const void* data, std::size_t size);

    uint8_t read(uint16_t address) const;
    void write(uint16_t address, uint8_t value);

private:
    static constexpr uint8_t kOpenBusValue = 0xFFU;

    static bool is_rom_address(uint16_t address);
    static bool is_ram_address(uint16_t address);
    static uint16_t ram_offset(uint16_t address);

    std::vector<uint8_t> rom_;
    Memory memory_;
};

} // namespace superz80
