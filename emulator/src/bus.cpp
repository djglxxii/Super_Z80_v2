#include "bus.h"

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>

namespace superz80 {

Bus::Bus()
    : rom_(kRomSize, kOpenBusValue),
      vdp_(),
      vblank_(irq_controller_),
      dma_(/* bus = */ *this) {}

void Bus::reset() {
    io_.reset();
    irq_controller_.reset();
    memory_.reset();
    apu_.reset();
    ym2151_.reset();
    vdp_.reset();
    vblank_.reset();
    dma_.reset();
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

uint8_t Bus::read_port(uint8_t port) {
    if (port == kIrqStatusPort) {
        return irq_controller_.status();
    }

    if (port == kIrqEnablePort) {
        return irq_controller_.enable();
    }

    if (port == kIrqAckPort) {
        return 0x00U;
    }

    if (port >= kVdpStatusPort && port <= kVdpFgPatternBankPort) {
        return vdp_.read_port(port);
    }

    if (port >= APU::kPortStart && port <= APU::kPortEnd) {
        return apu_.read_port(port);
    }

    if (port >= YM2151::kPortStart && port <= YM2151::kPortEnd) {
        return ym2151_.read_port(port);
    }

    if (port >= kDmaSrcLowPort && port <= kDmaControlPort) {
        return dma_.read_register(port);
    }

    return io_.read(port);
}

void Bus::write_port(uint8_t port, uint8_t value) {
    if (port == kIrqStatusPort) {
        return;
    }

    if (port == kIrqEnablePort) {
        irq_controller_.set_enable(value);
        return;
    }

    if (port == kIrqAckPort) {
        irq_controller_.acknowledge(value);
        return;
    }

    if (port >= kVdpStatusPort && port <= kVdpFgPatternBankPort) {
        vdp_.write_port(port, value);
        return;
    }

    if (port >= APU::kPortStart && port <= APU::kPortEnd) {
        apu_.write_port(port, value);
        return;
    }

    if (port >= YM2151::kPortStart && port <= YM2151::kPortEnd) {
        ym2151_.write_port(port, value);
        return;
    }

    if (port >= kDmaSrcLowPort && port <= kDmaControlPort) {
        dma_.write_register(port, value);
        return;
    }

    io_.write(port, value);
}

void Bus::set_controller_button(IO::Button button, bool pressed) {
    io_.set_button(button, pressed);
}

bool Bus::controller_button(IO::Button button) const {
    return io_.button(button);
}

IO::Snapshot Bus::controller_snapshot() const {
    return io_.snapshot();
}

void Bus::request_irq(uint8_t irq_bit) {
    irq_controller_.request(irq_bit);
}

bool Bus::irq_line() const {
    return irq_controller_.irq_line();
}

bool Bus::ym2151_irq_pending() const {
    return ym2151_.irq_pending();
}

VDP& Bus::vdp() {
    return vdp_;
}

const VDP& Bus::vdp() const {
    return vdp_;
}

VBlank& Bus::vblank() {
    return vblank_;
}

const VBlank& Bus::vblank() const {
    return vblank_;
}

DMA& Bus::dma() {
    return dma_;
}

const DMA& Bus::dma() const {
    return dma_;
}

APU& Bus::apu() {
    return apu_;
}

const APU& Bus::apu() const {
    return apu_;
}

YM2151& Bus::ym2151() {
    return ym2151_;
}

const YM2151& Bus::ym2151() const {
    return ym2151_;
}

Bus::Snapshot Bus::snapshot() const {
    Snapshot snapshot = {};
    std::copy(rom_.begin(), rom_.end(), snapshot.rom.begin());
    snapshot.io = io_.snapshot();
    snapshot.irq_controller = irq_controller_.snapshot();
    snapshot.memory = memory_.snapshot();
    snapshot.apu = apu_.snapshot();
    snapshot.ym2151 = ym2151_.snapshot();
    snapshot.vdp = vdp_.snapshot();
    snapshot.vblank = vblank_.snapshot();
    snapshot.dma = dma_.snapshot();
    return snapshot;
}

void Bus::restore(const Snapshot& snapshot) {
    rom_.assign(snapshot.rom.begin(), snapshot.rom.end());
    io_.restore(snapshot.io);
    irq_controller_.restore(snapshot.irq_controller);
    memory_.restore(snapshot.memory);
    apu_.restore(snapshot.apu);
    ym2151_.restore(snapshot.ym2151);
    vdp_.restore(snapshot.vdp);
    vblank_.restore(snapshot.vblank);
    dma_.restore(snapshot.dma);
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
