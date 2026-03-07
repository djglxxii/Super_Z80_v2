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

void write_solid_tile(superz80::VDP& vdp, uint8_t tile_index, uint8_t value) {
    const auto pattern_base = static_cast<uint16_t>(superz80::VDP::kPatternBase +
                                                    (static_cast<uint16_t>(tile_index) *
                                                     superz80::VDP::kTileSizeBytes));
    for (std::size_t offset = 0U; offset < superz80::VDP::kTileSizeBytes; ++offset) {
        vdp.write_vram(static_cast<uint16_t>(pattern_base + offset), value);
    }
}

void set_bg_tilemap_entry(superz80::VDP& vdp, std::size_t tile_x, std::size_t tile_y, uint8_t tile_index) {
    const auto offset = static_cast<uint16_t>((tile_y * superz80::VDP::kTilemapWidth) + tile_x);
    vdp.write_vram(static_cast<uint16_t>(superz80::VDP::kBgTilemapBase + offset), tile_index);
}

void set_fg_tilemap_entry(superz80::VDP& vdp, std::size_t tile_x, std::size_t tile_y, uint8_t tile_index) {
    const auto offset = static_cast<uint16_t>((tile_y * superz80::VDP::kTilemapWidth) + tile_x);
    vdp.write_vram(static_cast<uint16_t>(superz80::VDP::kFgTilemapBase + offset), tile_index);
}

void write_sprite_entry(superz80::VDP& vdp,
                        std::size_t sprite_index,
                        uint8_t y,
                        uint8_t x,
                        uint8_t tile_index,
                        uint8_t flags) {
    const auto sprite_base = static_cast<uint16_t>(superz80::VDP::kSpriteAttributeTableBase +
                                                   (sprite_index * superz80::VDP::kSpriteSizeBytes));
    vdp.write_vram(sprite_base, y);
    vdp.write_vram(static_cast<uint16_t>(sprite_base + 1U), x);
    vdp.write_vram(static_cast<uint16_t>(sprite_base + 2U), tile_index);
    vdp.write_vram(static_cast<uint16_t>(sprite_base + 3U), flags);
}

void hide_all_sprites(superz80::VDP& vdp) {
    for (std::size_t sprite_index = 0U; sprite_index < superz80::VDP::kMaxSprites; ++sprite_index) {
        write_sprite_entry(vdp,
                           sprite_index,
                           static_cast<uint8_t>(superz80::VDP::kFramebufferHeight),
                           0x00U,
                           0x00U,
                           0x00U);
    }
}

bool expect_pixel(const superz80::VDP& vdp,
                  std::size_t x,
                  std::size_t y,
                  uint8_t expected,
                  const char* name) {
    return expect_equal_u8(name, vdp.framebuffer_pixel(x, y), expected);
}

} // namespace

int main() {
    bool ok = true;

    {
        superz80::VDP vdp;
        vdp.reset();
        hide_all_sprites(vdp);
        write_solid_tile(vdp, 0x01U, 0x11U);
        write_solid_tile(vdp, 0x02U, 0x22U);
        write_solid_tile(vdp, 0x03U, 0x33U);
        set_bg_tilemap_entry(vdp, 0U, 0U, 0x01U);
        set_fg_tilemap_entry(vdp, 0U, 0U, 0x02U);
        write_sprite_entry(vdp, 0U, 0U, 0U, 0x03U, 0x00U);
        vdp.render_frame();
        ok = expect_pixel(vdp, 0U, 0U, 0x33U, "high-priority-sprite-overwrites-foreground") && ok;
    }

    {
        superz80::VDP vdp;
        vdp.reset();
        hide_all_sprites(vdp);
        write_solid_tile(vdp, 0x01U, 0x11U);
        write_solid_tile(vdp, 0x02U, 0x22U);
        write_solid_tile(vdp, 0x03U, 0x33U);
        set_bg_tilemap_entry(vdp, 0U, 0U, 0x01U);
        set_fg_tilemap_entry(vdp, 0U, 0U, 0x02U);
        write_sprite_entry(vdp, 0U, 0U, 0U, 0x03U, superz80::VDP::kSpritePriorityFlag);
        vdp.render_frame();
        ok = expect_pixel(vdp, 0U, 0U, 0x22U, "low-priority-sprite-stays-behind-foreground") && ok;
    }

    {
        superz80::VDP vdp;
        vdp.reset();
        hide_all_sprites(vdp);
        write_solid_tile(vdp, 0x01U, 0x11U);
        write_solid_tile(vdp, 0x02U, 0x00U);
        write_solid_tile(vdp, 0x03U, 0x33U);
        set_bg_tilemap_entry(vdp, 0U, 0U, 0x01U);
        set_fg_tilemap_entry(vdp, 0U, 0U, 0x02U);
        write_sprite_entry(vdp, 0U, 0U, 0U, 0x03U, superz80::VDP::kSpritePriorityFlag);
        vdp.render_frame();
        ok = expect_pixel(vdp, 0U, 0U, 0x33U, "low-priority-sprite-still-overwrites-background") && ok;
    }

    {
        superz80::VDP vdp;
        vdp.reset();
        hide_all_sprites(vdp);
        write_solid_tile(vdp, 0x01U, 0x44U);
        write_solid_tile(vdp, 0x02U, 0x55U);
        write_sprite_entry(vdp, 0U, 0U, 0U, 0x01U, 0x00U);
        write_sprite_entry(vdp, 1U, 0U, 0U, 0x02U, 0x00U);
        vdp.render_frame();
        ok = expect_pixel(vdp, 0U, 0U, 0x55U, "later-sat-entry-overwrites-earlier-sprite") && ok;
    }

    {
        superz80::VDP vdp;
        vdp.reset();
        hide_all_sprites(vdp);
        write_solid_tile(vdp, 0x01U, 0x11U);
        write_solid_tile(vdp, 0x02U, 0x22U);
        write_solid_tile(vdp, 0x03U, 0x33U);
        write_solid_tile(vdp, 0x04U, 0x44U);
        set_bg_tilemap_entry(vdp, 0U, 0U, 0x01U);
        set_fg_tilemap_entry(vdp, 0U, 0U, 0x02U);
        write_sprite_entry(vdp, 0U, 0U, 0U, 0x03U, superz80::VDP::kSpritePriorityFlag);
        write_sprite_entry(vdp, 1U, 0U, 0U, 0x04U, 0x00U);
        vdp.render_frame();
        ok = expect_pixel(vdp, 0U, 0U, 0x44U, "high-priority-pass-overwrites-foreground-and-low-priority") && ok;
    }

    return ok ? 0 : 1;
}
