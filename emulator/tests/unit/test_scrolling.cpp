#include "bus.h"
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

void write_pattern_tile(superz80::VDP& vdp, uint8_t tile_index, uint8_t base_value) {
    const auto pattern_base = static_cast<uint16_t>(superz80::VDP::kPatternBase +
                                                    (static_cast<uint16_t>(tile_index) *
                                                     superz80::VDP::kTileSizeBytes));
    for (std::size_t y = 0U; y < superz80::VDP::kTileHeight; ++y) {
        for (std::size_t x = 0U; x < superz80::VDP::kTileWidth; ++x) {
            const auto offset = static_cast<uint16_t>((y * superz80::VDP::kTileWidth) + x);
            vdp.write_vram(static_cast<uint16_t>(pattern_base + offset),
                           static_cast<uint8_t>(base_value + (y * 0x10U) + x));
        }
    }
}

void set_tilemap_entry(superz80::VDP& vdp, std::size_t tile_x, std::size_t tile_y, uint8_t tile_index) {
    const auto offset = static_cast<uint16_t>((tile_y * superz80::VDP::kTilemapWidth) + tile_x);
    vdp.write_vram(static_cast<uint16_t>(superz80::VDP::kTilemapBase + offset), tile_index);
}

void hide_all_sprites(superz80::VDP& vdp) {
    for (std::size_t sprite_index = 0U; sprite_index < superz80::VDP::kMaxSprites; ++sprite_index) {
        const auto sprite_base = static_cast<uint16_t>(superz80::VDP::kSpriteAttributeTableBase +
                                                       (sprite_index * superz80::VDP::kSpriteSizeBytes));
        vdp.write_vram(sprite_base, static_cast<uint8_t>(superz80::VDP::kFramebufferHeight));
        vdp.write_vram(static_cast<uint16_t>(sprite_base + 1U),
                       static_cast<uint8_t>(superz80::VDP::kFramebufferWidth));
        vdp.write_vram(static_cast<uint16_t>(sprite_base + 2U), 0x00U);
        vdp.write_vram(static_cast<uint16_t>(sprite_base + 3U), 0x00U);
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
        set_tilemap_entry(vdp, 0U, 0U, 0x01U);
        set_tilemap_entry(vdp, 1U, 0U, 0x02U);
        vdp.render_frame();
        ok = expect_pixel(vdp, 0U, 0U, 0x11U, "zero-scroll-origin-preserved") && ok;
        ok = expect_pixel(vdp, 7U, 0U, 0x11U, "zero-scroll-first-tile-right-edge") && ok;
        ok = expect_pixel(vdp, 8U, 0U, 0x22U, "zero-scroll-second-tile-still-adjacent") && ok;
    }

    {
        superz80::VDP vdp;
        vdp.reset();
        hide_all_sprites(vdp);
        write_solid_tile(vdp, 0x01U, 0x10U);
        write_solid_tile(vdp, 0x02U, 0x20U);
        set_tilemap_entry(vdp, 0U, 0U, 0x01U);
        set_tilemap_entry(vdp, 1U, 0U, 0x02U);
        vdp.write_port(superz80::VDP::kBgScrollXPort, 0x08U);
        vdp.render_frame();
        ok = expect_pixel(vdp, 0U, 0U, 0x20U, "horizontal-scroll-brings-next-tile-to-origin") && ok;
        ok = expect_pixel(vdp, 7U, 7U, 0x20U, "horizontal-scroll-fills-origin-tile") && ok;
    }

    {
        superz80::VDP vdp;
        vdp.reset();
        hide_all_sprites(vdp);
        write_solid_tile(vdp, 0x01U, 0x30U);
        write_solid_tile(vdp, 0x02U, 0x40U);
        set_tilemap_entry(vdp, 0U, 0U, 0x01U);
        set_tilemap_entry(vdp, 0U, 1U, 0x02U);
        vdp.write_port(superz80::VDP::kBgScrollYPort, 0x08U);
        vdp.render_frame();
        ok = expect_pixel(vdp, 0U, 0U, 0x40U, "vertical-scroll-brings-next-row-to-origin") && ok;
        ok = expect_pixel(vdp, 7U, 7U, 0x40U, "vertical-scroll-fills-origin-tile") && ok;
    }

    {
        superz80::VDP vdp;
        vdp.reset();
        hide_all_sprites(vdp);
        write_pattern_tile(vdp, 0x01U, 0x00U);
        write_pattern_tile(vdp, 0x02U, 0x80U);
        write_pattern_tile(vdp, 0x03U, 0x40U);
        write_pattern_tile(vdp, 0x04U, 0xC0U);
        set_tilemap_entry(vdp, 0U, 0U, 0x01U);
        set_tilemap_entry(vdp, 1U, 0U, 0x02U);
        set_tilemap_entry(vdp, 0U, 1U, 0x03U);
        set_tilemap_entry(vdp, 1U, 1U, 0x04U);
        vdp.write_port(superz80::VDP::kBgScrollXPort, 0x01U);
        vdp.write_port(superz80::VDP::kBgScrollYPort, 0x01U);
        vdp.render_frame();
        ok = expect_pixel(vdp, 0U, 0U, 0x11U, "partial-scroll-origin-shifts-inside-tile") && ok;
        ok = expect_pixel(vdp, 6U, 6U, 0x77U, "partial-scroll-bottom-right-within-first-tile") && ok;
        ok = expect_pixel(vdp, 7U, 0U, 0x90U, "partial-scroll-crosses-horizontal-tile-boundary") && ok;
        ok = expect_pixel(vdp, 0U, 7U, 0x41U, "partial-scroll-crosses-vertical-tile-boundary") && ok;
        ok = expect_pixel(vdp, 7U, 7U, 0xC0U, "partial-scroll-crosses-both-boundaries") && ok;
    }

    {
        superz80::VDP vdp;
        vdp.reset();
        hide_all_sprites(vdp);
        write_solid_tile(vdp, 0x01U, 0xA1U);
        write_solid_tile(vdp, 0x02U, 0xB2U);
        write_solid_tile(vdp, 0x03U, 0xC3U);
        write_solid_tile(vdp, 0x04U, 0xD4U);
        set_tilemap_entry(vdp, 31U, 23U, 0x01U);
        set_tilemap_entry(vdp, 0U, 23U, 0x02U);
        set_tilemap_entry(vdp, 31U, 0U, 0x03U);
        set_tilemap_entry(vdp, 0U, 0U, 0x04U);
        vdp.write_port(superz80::VDP::kBgScrollXPort, 0xFFU);
        vdp.write_port(superz80::VDP::kBgScrollYPort, 0xBFU);
        vdp.render_frame();
        ok = expect_pixel(vdp, 0U, 0U, 0xA1U, "wraparound-origin-reads-bottom-right-tile") && ok;
        ok = expect_pixel(vdp, 1U, 0U, 0xB2U, "wraparound-horizontal-advances-to-left-edge") && ok;
        ok = expect_pixel(vdp, 0U, 1U, 0xC3U, "wraparound-vertical-advances-to-top-edge") && ok;
        ok = expect_pixel(vdp, 1U, 1U, 0xD4U, "wraparound-both-axes-return-to-top-left") && ok;
    }

    {
        superz80::Bus bus;
        bus.reset();
        bus.write_port(superz80::Bus::kVdpBgScrollXPort, 0x34U);
        bus.write_port(superz80::Bus::kVdpBgScrollYPort, 0x56U);
        ok = expect_equal_u8("scroll-register-readback-x",
                             bus.read_port(superz80::Bus::kVdpBgScrollXPort),
                             0x34U) && ok;
        ok = expect_equal_u8("scroll-register-readback-y",
                             bus.read_port(superz80::Bus::kVdpBgScrollYPort),
                             0x56U) && ok;
        bus.reset();
        ok = expect_equal_u8("scroll-register-reset-x",
                             bus.read_port(superz80::Bus::kVdpBgScrollXPort),
                             0x00U) && ok;
        ok = expect_equal_u8("scroll-register-reset-y",
                             bus.read_port(superz80::Bus::kVdpBgScrollYPort),
                             0x00U) && ok;
    }

    return ok ? 0 : 1;
}
