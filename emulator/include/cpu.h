#pragma once

#include <cstdint>

#include "bus.h"

namespace superz80 {

class CPU {
public:
    explicit CPU(Bus& bus);
    ~CPU();

    CPU(const CPU&) = delete;
    CPU& operator=(const CPU&) = delete;

    CPU(CPU&&) = delete;
    CPU& operator=(CPU&&) = delete;

    void reset();
    void step();
    void set_int_line(bool level);

    uint8_t get_register_a() const;
    bool is_halted() const;
    bool int_line() const;

    static uint8_t memory_read_callback(void* user_data, uint16_t address);
    static void memory_write_callback(void* user_data, uint16_t address, uint8_t value);
    static uint8_t port_read_callback(void* user_data, uint16_t port);
    static void port_write_callback(void* user_data, uint16_t port, uint8_t value);

private:
    Bus& bus_;
    void* cpu_;
    bool int_line_;
};

} // namespace superz80
