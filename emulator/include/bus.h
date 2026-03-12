#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>

#include "apu.h"
#include "dma.h"
#include "io.h"
#include "irq_controller.h"
#include "memory.h"
#include "vdp.h"
#include "vblank.h"
#include "ym2151.h"

namespace superz80 {

class Bus {
public:
    static constexpr uint16_t kRomStart = 0x0000U;
    static constexpr uint16_t kRomEnd = 0x7FFFU;
    static constexpr uint16_t kUnmappedStart = 0x8000U;
    static constexpr uint16_t kUnmappedEnd = 0xBFFFU;
    static constexpr uint16_t kRamStart = 0xC000U;
    static constexpr uint16_t kRamEnd = 0xFFFFU;
    static constexpr std::size_t kRomSize = 0x8000U;
    static constexpr std::size_t kRamWindowSize = 0x4000U;
    static constexpr uint8_t kIrqStatusPort = 0xF0U;
    static constexpr uint8_t kIrqEnablePort = 0xF1U;
    static constexpr uint8_t kIrqAckPort = 0xF2U;
    static constexpr uint8_t kVdpStatusPort = VDP::kStatusPort;
    static constexpr uint8_t kVdpControlPort = VDP::kControlPort;
    static constexpr uint8_t kVdpDataPort = VDP::kDataPort;
    static constexpr uint8_t kVdpPaletteIndexPort = VDP::kPaletteIndexPort;
    static constexpr uint8_t kVdpPaletteDataRPort = VDP::kPaletteDataRPort;
    static constexpr uint8_t kVdpPaletteDataGPort = VDP::kPaletteDataGPort;
    static constexpr uint8_t kVdpPaletteDataBPort = VDP::kPaletteDataBPort;
    static constexpr uint8_t kVdpPaletteDataAPort = VDP::kPaletteDataAPort;
    static constexpr uint8_t kVdpBgScrollXPort = VDP::kBgScrollXPort;
    static constexpr uint8_t kVdpBgScrollYPort = VDP::kBgScrollYPort;
    static constexpr uint8_t kVdpFgScrollXPort = VDP::kFgScrollXPort;
    static constexpr uint8_t kVdpFgScrollYPort = VDP::kFgScrollYPort;
    static constexpr uint8_t kVdpBgPatternBankPort = VDP::kBgPatternBankPort;
    static constexpr uint8_t kVdpFgPatternBankPort = VDP::kFgPatternBankPort;
    static constexpr uint8_t kDmaSrcLowPort = DMA::kSrcLowPort;
    static constexpr uint8_t kDmaSrcHighPort = DMA::kSrcHighPort;
    static constexpr uint8_t kDmaDstLowPort = DMA::kDstLowPort;
    static constexpr uint8_t kDmaDstHighPort = DMA::kDstHighPort;
    static constexpr uint8_t kDmaLengthPort = DMA::kLengthPort;
    static constexpr uint8_t kDmaControlPort = DMA::kControlPort;
    static constexpr uint8_t kPad1Port = IO::kPad1Port;
    static constexpr uint8_t kPad1SysPort = IO::kPad1SysPort;
    static constexpr uint8_t kAudioToneALowPort = APU::kToneALowPort;
    static constexpr uint8_t kAudioToneAHighPort = APU::kToneAHighPort;
    static constexpr uint8_t kAudioToneBLowPort = APU::kToneBLowPort;
    static constexpr uint8_t kAudioToneBHighPort = APU::kToneBHighPort;
    static constexpr uint8_t kAudioToneCLowPort = APU::kToneCLowPort;
    static constexpr uint8_t kAudioToneCHighPort = APU::kToneCHighPort;
    static constexpr uint8_t kAudioNoiseControlPort = APU::kNoiseControlPort;
    static constexpr uint8_t kAudioVolumeAPort = APU::kVolumeAPort;
    static constexpr uint8_t kAudioVolumeBPort = APU::kVolumeBPort;
    static constexpr uint8_t kAudioVolumeCPort = APU::kVolumeCPort;
    static constexpr uint8_t kAudioVolumeNoisePort = APU::kVolumeNoisePort;
    static constexpr uint8_t kAudioControlPort = APU::kControlPort;
    static constexpr uint8_t kYm2151RegisterSelectPort = YM2151::kRegisterSelectPort;
    static constexpr uint8_t kYm2151RegisterDataPort = YM2151::kRegisterDataPort;

    Bus();

    void reset();
    void load_rom(const void* data, std::size_t size);

    uint8_t read(uint16_t address) const;
    void write(uint16_t address, uint8_t value);
    uint8_t read_port(uint8_t port);
    void write_port(uint8_t port, uint8_t value);
    void set_controller_button(IO::Button button, bool pressed);
    bool controller_button(IO::Button button) const;
    IO::Snapshot controller_snapshot() const;
    void request_irq(uint8_t irq_bit);
    bool irq_line() const;
    bool ym2151_irq_pending() const;
    VDP& vdp();
    const VDP& vdp() const;
    VBlank& vblank();
    const VBlank& vblank() const;
    DMA& dma();
    const DMA& dma() const;
    APU& apu();
    const APU& apu() const;
    YM2151& ym2151();
    const YM2151& ym2151() const;

private:
    static constexpr uint8_t kOpenBusValue = 0xFFU;

    static bool is_rom_address(uint16_t address);
    static bool is_ram_address(uint16_t address);
    static uint16_t ram_offset(uint16_t address);

    std::vector<uint8_t> rom_;
    IO io_;
    IRQController irq_controller_;
    Memory memory_;
    APU apu_;
    YM2151 ym2151_;
    VDP vdp_;
    VBlank vblank_;
    DMA dma_;
};

} // namespace superz80
