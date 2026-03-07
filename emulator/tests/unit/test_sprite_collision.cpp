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

void write_sprite_entry(superz80::VDP& vdp,
                        std::size_t sprite_index,
                        uint8_t y,
                        uint8_t x,
                        uint8_t tile_index,
                        uint8_t flags = 0x00U) {
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
                           0x00U);
    }
}

void write_solid_tile(superz80::VDP& vdp, uint8_t tile_index, uint8_t value) {
    const auto pattern_base = static_cast<uint16_t>(superz80::VDP::kPatternBase +
                                                    (static_cast<uint16_t>(tile_index) *
                                                     superz80::VDP::kTileSizeBytes));
    for (std::size_t offset = 0U; offset < superz80::VDP::kTileSizeBytes; ++offset) {
        vdp.write_vram(static_cast<uint16_t>(pattern_base + offset), value);
    }
}

void write_sparse_tile(superz80::VDP& vdp,
                       uint8_t tile_index,
                       std::size_t pixel_x,
                       std::size_t pixel_y,
                       uint8_t value) {
    const auto pattern_base = static_cast<uint16_t>(superz80::VDP::kPatternBase +
                                                    (static_cast<uint16_t>(tile_index) *
                                                     superz80::VDP::kTileSizeBytes));
    for (std::size_t offset = 0U; offset < superz80::VDP::kTileSizeBytes; ++offset) {
        vdp.write_vram(static_cast<uint16_t>(pattern_base + offset), 0x00U);
    }

    const auto pixel_offset =
        static_cast<uint16_t>(pattern_base + (pixel_y * superz80::VDP::kTileWidth) + pixel_x);
    vdp.write_vram(pixel_offset, value);
}

uint8_t read_status(superz80::VDP& vdp) {
    return vdp.read_port(superz80::VDP::kStatusPort);
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
        write_sprite_entry(vdp, 0U, 32U, 16U, 0x01U);
        write_sprite_entry(vdp, 1U, 32U, 64U, 0x02U);
        vdp.render_frame();
        ok = expect_equal_u8("no-collision-status", read_status(vdp), 0x00U) && ok;
    }

    {
        superz80::VDP vdp;
        vdp.reset();
        hide_all_sprites(vdp);
        write_solid_tile(vdp, 0x01U, 0x11U);
        write_solid_tile(vdp, 0x02U, 0x22U);
        write_sprite_entry(vdp, 0U, 40U, 24U, 0x01U);
        write_sprite_entry(vdp, 1U, 40U, 24U, 0x02U);
        vdp.render_frame();
        ok = expect_equal_u8("simple-collision-status", read_status(vdp), 0x08U) && ok;
    }

    {
        superz80::VDP vdp;
        vdp.reset();
        hide_all_sprites(vdp);
        write_sparse_tile(vdp, 0x01U, 0U, 0U, 0x11U);
        write_sparse_tile(vdp, 0x02U, 1U, 0U, 0x22U);
        write_sprite_entry(vdp, 0U, 48U, 48U, 0x01U);
        write_sprite_entry(vdp, 1U, 48U, 48U, 0x02U);
        vdp.render_frame();
        ok = expect_equal_u8("transparent-overlap-no-collision", read_status(vdp), 0x00U) && ok;
    }

    {
        superz80::VDP vdp;
        vdp.reset();
        hide_all_sprites(vdp);
        write_solid_tile(vdp, 0x01U, 0x11U);
        write_solid_tile(vdp, 0x02U, 0x22U);
        write_sprite_entry(vdp, 0U, 56U, 56U, 0x01U);
        write_sprite_entry(vdp, 1U, 56U, 56U, 0x02U);
        vdp.render_frame();
        ok = expect_equal_u8("collision-flag-set-on-read", read_status(vdp), 0x08U) && ok;
        ok = expect_equal_u8("collision-flag-clears-after-read", read_status(vdp), 0x00U) && ok;
    }

    {
        superz80::VDP vdp;
        vdp.reset();
        hide_all_sprites(vdp);
        write_solid_tile(vdp, 0x01U, 0x11U);
        write_solid_tile(vdp, 0x02U, 0x22U);
        write_sprite_entry(vdp, 0U, 64U, 64U, 0x01U, superz80::VDP::kSpritePriorityFlag);
        write_sprite_entry(vdp, 1U, 64U, 64U, 0x02U, 0x00U);
        vdp.render_frame();
        ok = expect_equal_u8("priority-independent-collision", read_status(vdp), 0x08U) && ok;
    }

    return ok ? 0 : 1;
}
