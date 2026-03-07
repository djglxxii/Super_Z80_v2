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
    static constexpr uint8_t kScanlineY = 40U;

    {
        superz80::VDP vdp;
        vdp.reset();
        hide_all_sprites(vdp);

        for (std::size_t sprite_index = 0U; sprite_index < 5U; ++sprite_index) {
            const uint8_t tile_index = static_cast<uint8_t>(sprite_index + 1U);
            const uint8_t pixel_value = static_cast<uint8_t>(0x10U + sprite_index);
            write_solid_tile(vdp, tile_index, pixel_value);
            write_sprite_entry(vdp,
                               sprite_index,
                               kScanlineY,
                               static_cast<uint8_t>(sprite_index * superz80::VDP::kTileWidth),
                               tile_index);
        }

        vdp.render_frame();
        for (std::size_t sprite_index = 0U; sprite_index < 5U; ++sprite_index) {
            const uint8_t expected = static_cast<uint8_t>(0x10U + sprite_index);
            const auto x = sprite_index * superz80::VDP::kTileWidth;
            ok = expect_pixel(vdp,
                              x,
                              kScanlineY,
                              expected,
                              "under-limit-sprite-renders") &&
                 ok;
        }
    }

    {
        superz80::VDP vdp;
        vdp.reset();
        hide_all_sprites(vdp);

        for (std::size_t sprite_index = 0U; sprite_index < 20U; ++sprite_index) {
            const uint8_t tile_index = static_cast<uint8_t>(sprite_index + 1U);
            const uint8_t pixel_value = static_cast<uint8_t>(0x20U + sprite_index);
            write_solid_tile(vdp, tile_index, pixel_value);
            write_sprite_entry(vdp,
                               sprite_index,
                               kScanlineY,
                               static_cast<uint8_t>(sprite_index * superz80::VDP::kTileWidth),
                               tile_index);
        }

        vdp.render_frame();
        ok = expect_pixel(vdp, 0U, kScanlineY, 0x20U, "first-sprite-under-limit-renders") && ok;
        ok = expect_pixel(vdp, 120U, kScanlineY, 0x2FU, "sixteenth-sprite-renders") && ok;
        ok = expect_pixel(vdp, 128U, kScanlineY, 0x00U, "seventeenth-sprite-is-ignored") && ok;
    }

    {
        superz80::VDP vdp;
        vdp.reset();
        hide_all_sprites(vdp);

        for (std::size_t sprite_index = 0U; sprite_index < 17U; ++sprite_index) {
            const uint8_t tile_index = static_cast<uint8_t>(sprite_index + 1U);
            write_solid_tile(vdp, tile_index, static_cast<uint8_t>(0x40U + sprite_index));
            write_sprite_entry(vdp,
                               sprite_index,
                               kScanlineY,
                               static_cast<uint8_t>(sprite_index * superz80::VDP::kTileWidth),
                               tile_index);
        }

        vdp.render_frame();
        ok = expect_equal_u8("overflow-flag-set-on-status-read",
                             vdp.read_port(superz80::VDP::kStatusPort),
                             0x04U) &&
             ok;
        ok = expect_equal_u8("overflow-flag-clears-after-read",
                             vdp.read_port(superz80::VDP::kStatusPort),
                             0x00U) &&
             ok;
    }

    {
        superz80::VDP vdp;
        vdp.reset();
        hide_all_sprites(vdp);

        for (std::size_t sprite_index = 0U; sprite_index < 17U; ++sprite_index) {
            const uint8_t tile_index = static_cast<uint8_t>(sprite_index + 1U);
            const uint8_t pixel_value = static_cast<uint8_t>(0x60U + sprite_index);
            write_solid_tile(vdp, tile_index, pixel_value);
            write_sprite_entry(vdp, sprite_index, kScanlineY, 0U, tile_index);
        }

        vdp.render_frame();
        ok = expect_pixel(vdp, 0U, kScanlineY, 0x6FU, "first-sixteen-follow-sat-order") && ok;
    }

    return ok ? 0 : 1;
}
