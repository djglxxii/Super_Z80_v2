#include "cpu.h"

extern "C" {
#include "z80ex.h"
}

#include <cstdint>
#include <stdexcept>

namespace superz80 {

namespace {

Z80EX_CONTEXT* as_context(void* cpu) {
    return static_cast<Z80EX_CONTEXT*>(cpu);
}

Z80EX_BYTE z80ex_memory_read(Z80EX_CONTEXT*, Z80EX_WORD address, int, void* user_data) {
    return CPU::memory_read_callback(user_data, address);
}

void z80ex_memory_write(Z80EX_CONTEXT*, Z80EX_WORD address, Z80EX_BYTE value, void* user_data) {
    CPU::memory_write_callback(user_data, address, value);
}

Z80EX_BYTE z80ex_port_read(Z80EX_CONTEXT*, Z80EX_WORD port, void* user_data) {
    return CPU::port_read_callback(user_data, port);
}

void z80ex_port_write(Z80EX_CONTEXT*, Z80EX_WORD port, Z80EX_BYTE value, void* user_data) {
    CPU::port_write_callback(user_data, port, value);
}

Z80EX_BYTE z80ex_interrupt_read(Z80EX_CONTEXT*, void*) {
    return 0xFFU;
}

} // namespace

CPU::CPU(Bus& bus)
    : bus_(bus),
      cpu_(z80ex_create(
          &z80ex_memory_read,
          this,
          &z80ex_memory_write,
          this,
          &z80ex_port_read,
          this,
          &z80ex_port_write,
          this,
          &z80ex_interrupt_read,
          nullptr)),
      int_line_(false) {
    if (cpu_ == nullptr) {
        throw std::runtime_error("failed to create Z80ex CPU context");
    }
}

CPU::~CPU() {
    if (cpu_ != nullptr) {
        z80ex_destroy(as_context(cpu_));
        cpu_ = nullptr;
    }
}

void CPU::reset() {
    z80ex_reset(as_context(cpu_));
    z80ex_set_reg(as_context(cpu_), regPC, 0x0000U);
    int_line_ = false;
}

void CPU::step() {
    set_int_line(bus_.irq_line());
    if (int_line_) {
        z80ex_int(as_context(cpu_));
    }

    do {
        z80ex_step(as_context(cpu_));
    } while (z80ex_last_op_type(as_context(cpu_)) != 0U);
}

void CPU::set_int_line(bool level) {
    int_line_ = level;
}

uint8_t CPU::get_register_a() const {
    const uint16_t af = z80ex_get_reg(as_context(cpu_), regAF);
    return static_cast<uint8_t>((af >> 8U) & 0xFFU);
}

bool CPU::is_halted() const {
    return z80ex_doing_halt(as_context(cpu_)) != 0;
}

bool CPU::int_line() const {
    return int_line_;
}

uint8_t CPU::memory_read_callback(void* user_data, uint16_t address) {
    auto* cpu = static_cast<CPU*>(user_data);
    return cpu->bus_.read(address);
}

void CPU::memory_write_callback(void* user_data, uint16_t address, uint8_t value) {
    auto* cpu = static_cast<CPU*>(user_data);
    cpu->bus_.write(address, value);
}

uint8_t CPU::port_read_callback(void* user_data, uint16_t port) {
    auto* cpu = static_cast<CPU*>(user_data);
    return cpu->bus_.read_port(static_cast<uint8_t>(port & 0x00FFU));
}

void CPU::port_write_callback(void* user_data, uint16_t port, uint8_t value) {
    auto* cpu = static_cast<CPU*>(user_data);
    cpu->bus_.write_port(static_cast<uint8_t>(port & 0x00FFU), value);
}

} // namespace superz80
