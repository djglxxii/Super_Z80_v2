#include "cpu.h"

extern "C" {
#include "z80ex.h"
#include "../vendor/z80ex/typedefs.h"
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

CPU::RegisterSnapshot CPU::snapshot() const {
    RegisterSnapshot snapshot = {};
    snapshot.af = z80ex_get_reg(as_context(cpu_), regAF);
    snapshot.bc = z80ex_get_reg(as_context(cpu_), regBC);
    snapshot.de = z80ex_get_reg(as_context(cpu_), regDE);
    snapshot.hl = z80ex_get_reg(as_context(cpu_), regHL);
    snapshot.af_shadow = z80ex_get_reg(as_context(cpu_), regAF_);
    snapshot.bc_shadow = z80ex_get_reg(as_context(cpu_), regBC_);
    snapshot.de_shadow = z80ex_get_reg(as_context(cpu_), regDE_);
    snapshot.hl_shadow = z80ex_get_reg(as_context(cpu_), regHL_);
    snapshot.ix = z80ex_get_reg(as_context(cpu_), regIX);
    snapshot.iy = z80ex_get_reg(as_context(cpu_), regIY);
    snapshot.pc = z80ex_get_reg(as_context(cpu_), regPC);
    snapshot.sp = z80ex_get_reg(as_context(cpu_), regSP);
    snapshot.memptr = as_context(cpu_)->memptr.w;
    snapshot.i = static_cast<uint8_t>(z80ex_get_reg(as_context(cpu_), regI) & 0x00FFU);
    snapshot.r = static_cast<uint8_t>(z80ex_get_reg(as_context(cpu_), regR) & 0x00FFU);
    snapshot.r7 = static_cast<uint8_t>(z80ex_get_reg(as_context(cpu_), regR7) & 0x00FFU);
    snapshot.interrupt_mode =
        static_cast<uint8_t>(z80ex_get_reg(as_context(cpu_), regIM) & 0x00FFU);
    snapshot.iff1 = z80ex_get_reg(as_context(cpu_), regIFF1) != 0U;
    snapshot.iff2 = z80ex_get_reg(as_context(cpu_), regIFF2) != 0U;
    snapshot.halted = is_halted();
    snapshot.int_line = int_line_;
    snapshot.noint_once = as_context(cpu_)->noint_once != 0;
    snapshot.reset_pv_on_int = as_context(cpu_)->reset_PV_on_int != 0;
    snapshot.doing_opcode = as_context(cpu_)->doing_opcode != 0;
    snapshot.int_vector_req = as_context(cpu_)->int_vector_req != 0;
    snapshot.prefix = as_context(cpu_)->prefix;
    snapshot.tstate = static_cast<uint32_t>(as_context(cpu_)->tstate);
    snapshot.op_tstate = as_context(cpu_)->op_tstate;
    return snapshot;
}

void CPU::restore(const RegisterSnapshot& snapshot) {
    z80ex_set_reg(as_context(cpu_), regAF, snapshot.af);
    z80ex_set_reg(as_context(cpu_), regBC, snapshot.bc);
    z80ex_set_reg(as_context(cpu_), regDE, snapshot.de);
    z80ex_set_reg(as_context(cpu_), regHL, snapshot.hl);
    z80ex_set_reg(as_context(cpu_), regAF_, snapshot.af_shadow);
    z80ex_set_reg(as_context(cpu_), regBC_, snapshot.bc_shadow);
    z80ex_set_reg(as_context(cpu_), regDE_, snapshot.de_shadow);
    z80ex_set_reg(as_context(cpu_), regHL_, snapshot.hl_shadow);
    z80ex_set_reg(as_context(cpu_), regIX, snapshot.ix);
    z80ex_set_reg(as_context(cpu_), regIY, snapshot.iy);
    z80ex_set_reg(as_context(cpu_), regPC, snapshot.pc);
    z80ex_set_reg(as_context(cpu_), regSP, snapshot.sp);
    z80ex_set_reg(as_context(cpu_), regI, snapshot.i);
    z80ex_set_reg(as_context(cpu_), regR, snapshot.r);
    z80ex_set_reg(as_context(cpu_), regR7, snapshot.r7);
    z80ex_set_reg(as_context(cpu_), regIM, snapshot.interrupt_mode);
    z80ex_set_reg(as_context(cpu_), regIFF1, snapshot.iff1 ? 1U : 0U);
    z80ex_set_reg(as_context(cpu_), regIFF2, snapshot.iff2 ? 1U : 0U);

    as_context(cpu_)->memptr.w = snapshot.memptr;
    as_context(cpu_)->halted = snapshot.halted ? 1 : 0;
    as_context(cpu_)->noint_once = snapshot.noint_once ? 1 : 0;
    as_context(cpu_)->reset_PV_on_int = snapshot.reset_pv_on_int ? 1 : 0;
    as_context(cpu_)->doing_opcode = snapshot.doing_opcode ? 1 : 0;
    as_context(cpu_)->int_vector_req = snapshot.int_vector_req ? 1 : 0;
    as_context(cpu_)->prefix = snapshot.prefix;
    as_context(cpu_)->tstate = snapshot.tstate;
    as_context(cpu_)->op_tstate = snapshot.op_tstate;
    int_line_ = snapshot.int_line;
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
