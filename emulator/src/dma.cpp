#include "dma.h"

#include "bus.h"

namespace superz80 {

DMA::DMA(Bus& bus)
    : bus_(bus),
      src_(0U),
      dst_(0U),
      length_(0U),
      active_(false) {}

void DMA::reset() {
    src_ = 0U;
    dst_ = 0U;
    length_ = 0U;
    active_ = false;
}

void DMA::write_register(uint8_t port, uint8_t value) {
    switch (port) {
    case kSrcLowPort:
        src_ = static_cast<uint16_t>((src_ & 0xFF00U) | value);
        break;
    case kSrcHighPort:
        src_ = static_cast<uint16_t>((src_ & 0x00FFU) | (static_cast<uint16_t>(value) << 8U));
        break;
    case kDstLowPort:
        dst_ = static_cast<uint16_t>((dst_ & 0xFF00U) | value);
        break;
    case kDstHighPort:
        dst_ = static_cast<uint16_t>((dst_ & 0x00FFU) | (static_cast<uint16_t>(value) << 8U));
        break;
    case kLengthPort:
        length_ = value;
        if (length_ == 0U) {
            active_ = false;
        }
        break;
    case kControlPort:
        if ((value & kControlStartBit) != 0U && length_ != 0U) {
            active_ = true;
        }
        break;
    default:
        break;
    }
}

uint8_t DMA::read_register(uint8_t port) const {
    switch (port) {
    case kSrcLowPort:
        return static_cast<uint8_t>(src_ & 0x00FFU);
    case kSrcHighPort:
        return static_cast<uint8_t>((src_ >> 8U) & 0x00FFU);
    case kDstLowPort:
        return static_cast<uint8_t>(dst_ & 0x00FFU);
    case kDstHighPort:
        return static_cast<uint8_t>((dst_ >> 8U) & 0x00FFU);
    case kLengthPort:
        return length_;
    case kControlPort:
        return active_ ? kControlBusyBit : 0x00U;
    default:
        return 0x00U;
    }
}

void DMA::step() {
    if (!active_) {
        return;
    }

    bus_.write(dst_, bus_.read(src_));
    src_ = static_cast<uint16_t>(src_ + 1U);
    dst_ = static_cast<uint16_t>(dst_ + 1U);
    --length_;

    if (length_ == 0U) {
        active_ = false;
    }
}

bool DMA::busy() const {
    return active_;
}

DMA::Snapshot DMA::snapshot() const {
    return {
        src_,
        dst_,
        length_,
        read_register(kControlPort),
        active_,
    };
}

} // namespace superz80
