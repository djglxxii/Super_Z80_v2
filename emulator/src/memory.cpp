#include "memory.h"

namespace superz80 {

Memory::Memory() {
    reset();
}

uint8_t Memory::read(uint16_t address) const {
    return ram_[address];
}

void Memory::write(uint16_t address, uint8_t value) {
    ram_[address] = value;
}

void Memory::reset() {
    ram_.fill(0x00U);
}

Memory::Snapshot Memory::snapshot() const {
    return ram_;
}

void Memory::restore(const Snapshot& snapshot) {
    ram_ = snapshot;
}

} // namespace superz80
