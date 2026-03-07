#include "io.h"

namespace superz80 {

IO::IO() {
    reset();
}

void IO::reset() {
    ports_.fill(0x00U);
}

uint8_t IO::read(uint8_t port) const {
    return ports_[port];
}

void IO::write(uint8_t port, uint8_t value) {
    ports_[port] = value;
}

} // namespace superz80
