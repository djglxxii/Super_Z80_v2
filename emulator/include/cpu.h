#pragma once

#include <cstdint>

#include "bus.h"

namespace superz80 {

class CPU {
public:
    struct RegisterSnapshot {
        uint16_t af = 0U;
        uint16_t bc = 0U;
        uint16_t de = 0U;
        uint16_t hl = 0U;
        uint16_t af_shadow = 0U;
        uint16_t bc_shadow = 0U;
        uint16_t de_shadow = 0U;
        uint16_t hl_shadow = 0U;
        uint16_t ix = 0U;
        uint16_t iy = 0U;
        uint16_t pc = 0U;
        uint16_t sp = 0U;
        uint16_t memptr = 0U;
        uint8_t i = 0U;
        uint8_t r = 0U;
        uint8_t r7 = 0U;
        uint8_t interrupt_mode = 0U;
        bool iff1 = false;
        bool iff2 = false;
        bool halted = false;
        bool int_line = false;
        bool noint_once = false;
        bool reset_pv_on_int = false;
        bool doing_opcode = false;
        bool int_vector_req = false;
        uint8_t prefix = 0U;
        uint32_t tstate = 0U;
        uint8_t op_tstate = 0U;
    };

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
    RegisterSnapshot snapshot() const;
    void restore(const RegisterSnapshot& snapshot);
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
