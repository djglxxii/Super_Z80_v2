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
