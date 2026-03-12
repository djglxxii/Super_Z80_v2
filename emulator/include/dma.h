#pragma once

#include <cstdint>

namespace superz80 {

class Bus;

class DMA {
public:
    struct Snapshot {
        uint16_t source_address = 0U;
        uint16_t destination_address = 0U;
        uint8_t transfer_length = 0U;
        uint8_t control = 0U;
        bool active = false;
    };

    static constexpr uint8_t kSrcLowPort = 0xF4U;
    static constexpr uint8_t kSrcHighPort = 0xF5U;
    static constexpr uint8_t kDstLowPort = 0xF6U;
    static constexpr uint8_t kDstHighPort = 0xF7U;
    static constexpr uint8_t kLengthPort = 0xF8U;
    static constexpr uint8_t kControlPort = 0xF9U;
    static constexpr uint8_t kControlStartBit = 0x01U;
    static constexpr uint8_t kControlBusyBit = 0x80U;

    explicit DMA(Bus& bus);

    void reset();

    void write_register(uint8_t port, uint8_t value);
    uint8_t read_register(uint8_t port) const;

    void step();

    bool busy() const;
    Snapshot snapshot() const;

private:
    Bus& bus_;
    uint16_t src_;
    uint16_t dst_;
    uint8_t length_;
    bool active_;
};

} // namespace superz80
