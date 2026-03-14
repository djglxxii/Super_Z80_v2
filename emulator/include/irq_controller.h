#pragma once

#include <cstdint>

namespace superz80 {

class IRQController {
public:
    struct Snapshot {
        uint8_t status = 0x00U;
        uint8_t enable = 0x00U;
    };

    IRQController();

    void reset();

    void request(uint8_t irq_bit);

    uint8_t status() const;

    uint8_t enable() const;
    void set_enable(uint8_t mask);

    void acknowledge(uint8_t mask);

    bool irq_line() const;
    Snapshot snapshot() const;
    void restore(const Snapshot& snapshot);

private:
    uint8_t irq_status_;
    uint8_t irq_enable_;
};

} // namespace superz80
