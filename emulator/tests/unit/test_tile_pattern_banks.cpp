#include "vdp.h"

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

bool expect_pixel(const superz80::VDP& vdp,
                  std::size_t x,
                  std::size_t y,
                  uint8_t expected,
                  const char* name) {
    return expect_equal_u8(name, vdp.framebuffer_pixel(x, y), expected);
}

void write_solid_tile(superz80::VDP& vdp, uint16_t bank_base, uint8_t tile_index, uint8_t value) {
    const auto pattern_base =
        static_cast<uint16_t>(bank_base + (static_cast<uint16_t>(tile_index) * superz80::VDP::kTileSizeBytes));
    for (std::size_t offset = 0U; offset < superz80::VDP::kTileSizeBytes; ++offset) {
        vdp.write_vram(static_cast<uint16_t>(pattern_base + offset), value);
    }
}

void set_bg_tile(superz80::VDP& vdp, std::size_t tile_x, std::size_t tile_y, uint8_t tile_index) {
    const auto offset = static_cast<uint16_t>((tile_y * superz80::VDP::kTilemapWidth) + tile_x);
    vdp.write_vram(static_cast<uint16_t>(superz80::VDP::kBgTilemapBase + offset), tile_index);
}

void set_fg_tile(superz80::VDP& vdp, std::size_t tile_x, std::size_t tile_y, uint8_t tile_index) {
    const auto offset = static_cast<uint16_t>((tile_y * superz80::VDP::kTilemapWidth) + tile_x);
    vdp.write_vram(static_cast<uint16_t>(superz80::VDP::kFgTilemapBase + offset), tile_index);
}

void hide_all_sprites(superz80::VDP& vdp) {
    for (std::size_t sprite_index = 0U; sprite_index < superz80::VDP::kMaxSprites; ++sprite_index) {
        const auto sprite_base = static_cast<uint16_t>(superz80::VDP::kSpriteAttributeTableBase +
                                                       (sprite_index * superz80::VDP::kSpriteSizeBytes));
        vdp.write_vram(sprite_base, static_cast<uint8_t>(superz80::VDP::kFramebufferHeight));
        vdp.write_vram(static_cast<uint16_t>(sprite_base + 1U), 0x00U);
        vdp.write_vram(static_cast<uint16_t>(sprite_base + 2U), 0x00U);
        vdp.write_vram(static_cast<uint16_t>(sprite_base + 3U), 0x00U);
    }
}

void hide_foreground_plane(superz80::VDP& vdp) {
    for (std::size_t tile_y = 0U; tile_y < superz80::VDP::kTilemapHeight; ++tile_y) {
        for (std::size_t tile_x = 0U; tile_x < superz80::VDP::kTilemapWidth; ++tile_x) {
            const auto offset = static_cast<uint16_t>((tile_y * superz80::VDP::kTilemapWidth) + tile_x);
            vdp.write_vram(static_cast<uint16_t>(superz80::VDP::kFgTilemapBase + offset), 0xFFU);
        }
    }
}

} // namespace

int main() {
    bool ok = true;

    {
        superz80::VDP vdp;
        vdp.reset();
        hide_all_sprites(vdp);
        hide_foreground_plane(vdp);
        write_solid_tile(vdp, superz80::VDP::kPatternBank0Base, 0x00U, 0x21U);
        set_bg_tile(vdp, 0U, 0U, 0x00U);
        vdp.write_port(superz80::VDP::kBgPatternBankPort, 0x00U);
        vdp.render_frame();
        ok = expect_pixel(vdp, 0U, 0U, 0x21U, "bg-bank0-fetch-origin") && ok;
        ok = expect_pixel(vdp, 7U, 7U, 0x21U, "bg-bank0-fetch-full-tile") && ok;
    }

    {
        superz80::VDP vdp;
        vdp.reset();
        hide_all_sprites(vdp);
        hide_foreground_plane(vdp);
        write_solid_tile(vdp, superz80::VDP::kPatternBank0Base, 0x00U, 0x12U);
        write_solid_tile(vdp, superz80::VDP::kPatternBank1Base, 0x00U, 0x34U);
        set_bg_tile(vdp, 0U, 0U, 0x00U);
        vdp.write_port(superz80::VDP::kBgPatternBankPort, 0x01U);
        vdp.render_frame();
        ok = expect_pixel(vdp, 0U, 0U, 0x34U, "bg-bank1-fetch-origin") && ok;
        ok = expect_pixel(vdp, 7U, 7U, 0x34U, "bg-bank1-fetch-full-tile") && ok;
    }

    {
        superz80::VDP vdp;
        vdp.reset();
        hide_all_sprites(vdp);
        write_solid_tile(vdp, superz80::VDP::kPatternBank0Base, 0x00U, 0x11U);
        write_solid_tile(vdp, superz80::VDP::kPatternBank0Base, 0x01U, 0x41U);
        write_solid_tile(vdp, superz80::VDP::kPatternBank1Base, 0x01U, 0x7AU);
        set_bg_tile(vdp, 0U, 0U, 0x00U);
        set_fg_tile(vdp, 0U, 0U, 0x01U);

        vdp.write_port(superz80::VDP::kFgPatternBankPort, 0x00U);
        vdp.render_frame();
        ok = expect_pixel(vdp, 0U, 0U, 0x41U, "fg-bank0-selected") && ok;

        vdp.write_port(superz80::VDP::kFgPatternBankPort, 0x01U);
        vdp.render_frame();
        ok = expect_pixel(vdp, 0U, 0U, 0x7AU, "fg-bank1-selected") && ok;
        ok = expect_pixel(vdp, 7U, 7U, 0x7AU, "fg-bank1-selected-full-tile") && ok;
    }

    {
        superz80::VDP vdp;
        vdp.reset();
        hide_all_sprites(vdp);
        hide_foreground_plane(vdp);
        write_solid_tile(vdp, superz80::VDP::kPatternBank0Base, 0x02U, 0x56U);
        write_solid_tile(vdp, superz80::VDP::kPatternBank1Base, 0x02U, 0xA9U);
        set_bg_tile(vdp, 0U, 0U, 0x02U);

        vdp.write_port(superz80::VDP::kBgPatternBankPort, 0x00U);
        vdp.render_frame();
        ok = expect_pixel(vdp, 0U, 0U, 0x56U, "bank-switch-bank0-value") && ok;

        vdp.write_port(superz80::VDP::kBgPatternBankPort, 0x01U);
        vdp.render_frame();
        ok = expect_pixel(vdp, 0U, 0U, 0xA9U, "bank-switch-bank1-value") && ok;
    }

    return ok ? 0 : 1;
}
