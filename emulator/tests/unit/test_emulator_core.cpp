#include "emulator_core.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>

namespace {

bool expect_equal_u8(const char* name, uint8_t actual, uint8_t expected) {
    if (actual != expected) {
        std::cerr << "[FAIL] " << name << " expected=0x" << std::hex << std::uppercase
                  << static_cast<unsigned int>(expected) << " actual=0x"
                  << static_cast<unsigned int>(actual) << std::dec << std::nouppercase
                  << std::endl;
        return false;
    }

    std::cout << "[PASS] " << name << " value=0x" << std::hex << std::uppercase
              << static_cast<unsigned int>(actual) << std::dec << std::nouppercase
              << std::endl;
    return true;
}

} // namespace

int main() {
    bool ok = true;

    EmulatorCore core;
    core.initialize();

    constexpr std::array<uint8_t, 4> kRom = {0x3EU, 0x01U, 0xD3U, 0xE0U};
    core.load_rom(kRom.data(), kRom.size());
    core.bus().write(0xC000U, 0x12U);
    core.bus().write(0xC001U, 0x34U);
    core.bus().write(0xFFFFU, 0xABU);
    core.bus().vdp().write_vram(0x0000U, 0x56U);
    core.bus().vdp().write_vram(0x1234U, 0x78U);
    core.bus().vdp().write_vram(0xFFFFU, 0x9AU);
    core.bus().vdp().write_vram(superz80::VDP::kSpriteAttributeTableBase, 0x10U);
    core.bus().vdp().write_vram(static_cast<uint16_t>(superz80::VDP::kSpriteAttributeTableBase + 1U), 0x20U);
    core.bus().vdp().write_vram(static_cast<uint16_t>(superz80::VDP::kSpriteAttributeTableBase + 2U), 0x30U);
    core.bus().vdp().write_vram(static_cast<uint16_t>(superz80::VDP::kSpriteAttributeTableBase + 3U), 0x40U);
    const uint16_t last_sprite_base = static_cast<uint16_t>(
        superz80::VDP::kSpriteAttributeTableBase +
        ((superz80::VDP::kMaxSprites - 1U) * superz80::VDP::kSpriteSizeBytes));
    core.bus().vdp().write_vram(last_sprite_base, 0xAAU);
    core.bus().vdp().write_vram(static_cast<uint16_t>(last_sprite_base + 1U), 0xBBU);
    core.bus().vdp().write_vram(static_cast<uint16_t>(last_sprite_base + 2U), 0xCCU);
    core.bus().vdp().write_vram(static_cast<uint16_t>(last_sprite_base + 3U), 0xDDU);
    core.bus().write_port(superz80::Bus::kDmaSrcLowPort, 0x00U);
    core.bus().write_port(superz80::Bus::kDmaSrcHighPort, 0xC0U);
    core.bus().write_port(superz80::Bus::kDmaDstLowPort, 0x20U);
    core.bus().write_port(superz80::Bus::kDmaDstHighPort, 0xC0U);
    core.bus().write_port(superz80::Bus::kDmaLengthPort, 0x02U);

    const EmulatorCore::DmaSnapshot dma_idle_snapshot = core.dma_snapshot();
    core.bus().write_port(superz80::Bus::kDmaControlPort, superz80::DMA::kControlStartBit);
    const EmulatorCore::DmaSnapshot dma_active_snapshot = core.dma_snapshot();
    core.bus().write_port(superz80::Bus::kAudioToneALowPort, 0x34U);
    core.bus().write_port(superz80::Bus::kAudioToneAHighPort, 0x02U);
    core.bus().write_port(superz80::Bus::kAudioVolumeAPort, 0x05U);
    core.bus().write_port(superz80::Bus::kAudioNoiseControlPort, 0x03U);
    core.bus().write_port(superz80::Bus::kAudioControlPort, 0x01U);
    core.bus().write_port(superz80::Bus::kYm2151RegisterSelectPort, 0x20U);
    core.bus().write_port(superz80::Bus::kYm2151RegisterDataPort, 0x2DU);
    core.bus().write_port(superz80::Bus::kYm2151RegisterSelectPort, 0x28U);
    core.bus().write_port(superz80::Bus::kYm2151RegisterDataPort, 0x30U);
    core.step_scanline();
    const EmulatorCore::AudioSnapshot audio_snapshot = core.audio_snapshot();
    const EmulatorCore::RomSnapshot rom_snapshot = core.rom_snapshot();
    const EmulatorCore::RamSnapshot ram_snapshot = core.ram_snapshot();
    const EmulatorCore::VramSnapshot vram_snapshot = core.vram_snapshot();
    const EmulatorCore::SpriteTableSnapshot sprite_table_snapshot = core.sprite_table_snapshot();

    ok = expect_equal_u8("rom-snapshot-byte-0", rom_snapshot[0x0000U], 0x3EU) && ok;
    ok = expect_equal_u8("rom-snapshot-byte-1", rom_snapshot[0x0001U], 0x01U) && ok;
    ok = expect_equal_u8("rom-snapshot-open-bus-fill", rom_snapshot[0x0010U], 0xFFU) && ok;
    ok = expect_equal_u8("ram-snapshot-byte-0", ram_snapshot[0x0000U], 0x12U) && ok;
    ok = expect_equal_u8("ram-snapshot-byte-1", ram_snapshot[0x0001U], 0x34U) && ok;
    ok = expect_equal_u8("ram-snapshot-last-byte", ram_snapshot[0x3FFFU], 0xABU) && ok;
    ok = expect_equal_u8("vram-snapshot-byte-0", vram_snapshot[0x0000U], 0x56U) && ok;
    ok = expect_equal_u8("vram-snapshot-byte-1234", vram_snapshot[0x1234U], 0x78U) && ok;
    ok = expect_equal_u8("vram-snapshot-last-byte", vram_snapshot[0xFFFFU], 0x9AU) && ok;
    ok = expect_equal_u8("dma-snapshot-idle-src-low",
                         static_cast<uint8_t>(dma_idle_snapshot.source_address & 0x00FFU),
                         0x00U) && ok;
    ok = expect_equal_u8("dma-snapshot-idle-src-high",
                         static_cast<uint8_t>((dma_idle_snapshot.source_address >> 8U) & 0x00FFU),
                         0xC0U) && ok;
    ok = expect_equal_u8("dma-snapshot-idle-dst-low",
                         static_cast<uint8_t>(dma_idle_snapshot.destination_address & 0x00FFU),
                         0x20U) && ok;
    ok = expect_equal_u8("dma-snapshot-idle-dst-high",
                         static_cast<uint8_t>((dma_idle_snapshot.destination_address >> 8U) & 0x00FFU),
                         0xC0U) && ok;
    ok = expect_equal_u8("dma-snapshot-idle-length", dma_idle_snapshot.transfer_length, 0x02U) && ok;
    ok = expect_equal_u8("dma-snapshot-idle-control", dma_idle_snapshot.control, 0x00U) && ok;
    ok = expect_equal_u8("dma-snapshot-active-control",
                         dma_active_snapshot.control,
                         superz80::DMA::kControlBusyBit) && ok;
    ok = expect_equal_u8("dma-snapshot-active-flag",
                         static_cast<uint8_t>(dma_active_snapshot.active ? 1U : 0U),
                         0x01U) && ok;
    ok = expect_equal_u8("audio-snapshot-psg-tone-a-low",
                         audio_snapshot.apu.registers[0U],
                         0x34U) && ok;
    ok = expect_equal_u8("audio-snapshot-psg-tone-a-high",
                         audio_snapshot.apu.registers[1U],
                         0x02U) && ok;
    ok = expect_equal_u8("audio-snapshot-psg-volume-a",
                         audio_snapshot.apu.registers[7U],
                         0x05U) && ok;
    ok = expect_equal_u8("audio-snapshot-psg-control",
                         audio_snapshot.apu.control,
                         0x01U) && ok;
    ok = expect_equal_u8("audio-snapshot-psg-enabled",
                         static_cast<uint8_t>(audio_snapshot.apu.enabled ? 1U : 0U),
                         0x01U) && ok;
    ok = expect_equal_u8("audio-snapshot-psg-noise-control",
                         audio_snapshot.apu.noise.control,
                         0x03U) && ok;
    ok = expect_equal_u8("audio-snapshot-ym-register-20",
                         audio_snapshot.ym2151.registers[0x20U],
                         0x2DU) && ok;
    ok = expect_equal_u8("audio-snapshot-ym-selected-register",
                         audio_snapshot.ym2151.selected_register,
                         0x28U) && ok;
    ok = expect_equal_u8("audio-snapshot-ym-channel-0-algorithm",
                         audio_snapshot.ym2151.channels[0U].algorithm,
                         0x05U) && ok;
    ok = expect_equal_u8("audio-snapshot-ym-channel-0-feedback",
                         audio_snapshot.ym2151.channels[0U].feedback,
                         0x05U) && ok;
    ok = expect_equal_u8("audio-snapshot-ym-channel-0-key-on-mask",
                         audio_snapshot.ym2151.channels[0U].key_on_mask,
                         0x03U) && ok;
    ok = expect_equal_u8("sprite-snapshot-first-y", sprite_table_snapshot[0U].y, 0x10U) && ok;
    ok = expect_equal_u8("sprite-snapshot-first-x", sprite_table_snapshot[0U].x, 0x20U) && ok;
    ok = expect_equal_u8("sprite-snapshot-first-tile", sprite_table_snapshot[0U].tile_index, 0x30U) && ok;
    ok = expect_equal_u8("sprite-snapshot-first-attr", sprite_table_snapshot[0U].attributes, 0x40U) && ok;
    ok = expect_equal_u8("sprite-snapshot-last-y",
                         sprite_table_snapshot[superz80::VDP::kMaxSprites - 1U].y,
                         0xAAU) && ok;
    ok = expect_equal_u8("sprite-snapshot-last-x",
                         sprite_table_snapshot[superz80::VDP::kMaxSprites - 1U].x,
                         0xBBU) && ok;
    ok = expect_equal_u8("sprite-snapshot-last-tile",
                         sprite_table_snapshot[superz80::VDP::kMaxSprites - 1U].tile_index,
                         0xCCU) && ok;
    ok = expect_equal_u8("sprite-snapshot-last-attr",
                         sprite_table_snapshot[superz80::VDP::kMaxSprites - 1U].attributes,
                         0xDDU) && ok;
    ok = expect_equal_u8("sprite-snapshot-entry-count",
                         static_cast<uint8_t>(sprite_table_snapshot.size()),
                         static_cast<uint8_t>(superz80::VDP::kMaxSprites)) && ok;

    return ok ? 0 : 1;
}
