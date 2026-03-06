#include "bus.h"

#include <algorithm>
#include <cstddef>
#include <cstdint>

namespace superz80 {

Bus::Bus()
    : rom_(kRomSize, kOpenBusValue) {}

void Bus::reset() {
    memory_.reset();
}

void Bus::load_rom(const void* data, std::size_t size) {
    rom_.assign(kRomSize, kOpenBusValue);

    if (data == nullptr || size == 0U) {
        return;
    }

    const auto bytes_to_copy = std::min(size, kRomSize);
    const auto* source = static_cast<const uint8_t*>(data);
    std::copy_n(source, bytes_to_copy, rom_.begin());
}

uint8_t Bus::read(uint16_t address) const {
    if (is_rom_address(address)) {
        return rom_[address];
    }

    if (is_ram_address(address)) {
        return memory_.read(ram_offset(address));
    }

    return kOpenBusValue;
}

void Bus::write(uint16_t address, uint8_t value) {
    if (is_ram_address(address)) {
        memory_.write(ram_offset(address), value);
    }
}

bool Bus::is_rom_address(uint16_t address) {
    return address >= kRomStart && address <= kRomEnd;
}

bool Bus::is_ram_address(uint16_t address) {
    return address >= kRamStart && address <= kRamEnd;
}

uint16_t Bus::ram_offset(uint16_t address) {
    return static_cast<uint16_t>(address - kRamStart);
}

} // namespace superz80
