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

void set_bg_tilemap_entry(superz80::VDP& vdp, std::size_t tile_x, std::size_t tile_y, uint8_t tile_index) {
    const auto offset = static_cast<uint16_t>((tile_y * superz80::VDP::kTilemapWidth) + tile_x);
    vdp.write_vram(static_cast<uint16_t>(superz80::VDP::kBgTilemapBase + offset), tile_index);
}

void set_fg_tilemap_entry(superz80::VDP& vdp, std::size_t tile_x, std::size_t tile_y, uint8_t tile_index) {
    const auto offset = static_cast<uint16_t>((tile_y * superz80::VDP::kTilemapWidth) + tile_x);
    vdp.write_vram(static_cast<uint16_t>(superz80::VDP::kFgTilemapBase + offset), tile_index);
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
        set_bg_tilemap_entry(vdp, 0U, 0U, 0x01U);
        set_fg_tilemap_entry(vdp, 0U, 0U, 0x02U);
        vdp.render_frame();
        ok = expect_pixel(vdp, 0U, 0U, 0x22U, "foreground-overwrites-background-origin") && ok;
        ok = expect_pixel(vdp, 7U, 7U, 0x22U, "foreground-overwrites-background-full-tile") && ok;
    }

    {
        superz80::VDP vdp;
        vdp.reset();
        hide_all_sprites(vdp);
        write_solid_tile(vdp, 0x01U, 0x33U);
        write_solid_tile(vdp, 0x02U, 0x00U);
        set_bg_tilemap_entry(vdp, 0U, 0U, 0x01U);
        set_fg_tilemap_entry(vdp, 0U, 0U, 0x02U);
        vdp.render_frame();
        ok = expect_pixel(vdp, 0U, 0U, 0x33U, "foreground-zero-keeps-background-origin") && ok;
        ok = expect_pixel(vdp, 7U, 7U, 0x33U, "foreground-zero-keeps-background-full-tile") && ok;
    }

    {
        superz80::VDP vdp;
        vdp.reset();
        hide_all_sprites(vdp);
        write_solid_tile(vdp, 0x03U, 0x44U);
        set_fg_tilemap_entry(vdp, 1U, 0U, 0x03U);
        vdp.render_frame();
        ok = expect_pixel(vdp, 0U, 0U, 0x00U, "foreground-placement-left-tile-remains-empty") && ok;
        ok = expect_pixel(vdp, 8U, 0U, 0x44U, "foreground-placement-tile-origin") && ok;
        ok = expect_pixel(vdp, 15U, 7U, 0x44U, "foreground-placement-tile-bottom-right") && ok;
    }

    {
        superz80::VDP vdp;
        vdp.reset();
        hide_all_sprites(vdp);
        write_solid_tile(vdp, 0x04U, 0x55U);
        write_solid_tile(vdp, 0x05U, 0x66U);
        set_fg_tilemap_entry(vdp, 0U, 0U, 0x04U);
        set_fg_tilemap_entry(vdp, 1U, 0U, 0x05U);
        vdp.write_port(superz80::VDP::kFgScrollXPort, 0x08U);
        vdp.render_frame();
        ok = expect_pixel(vdp, 0U, 0U, 0x66U, "foreground-horizontal-scroll-brings-next-tile-to-origin") &&
             ok;
        ok = expect_pixel(vdp, 7U, 7U, 0x66U, "foreground-horizontal-scroll-fills-origin-tile") && ok;
    }

    {
        superz80::VDP vdp;
        vdp.reset();
        hide_all_sprites(vdp);
        write_solid_tile(vdp, 0x06U, 0x11U);
        write_solid_tile(vdp, 0x07U, 0x22U);
        write_solid_tile(vdp, 0x08U, 0x33U);
        write_solid_tile(vdp, 0x09U, 0x44U);
        write_solid_tile(vdp, 0x0AU, 0x00U);
        set_bg_tilemap_entry(vdp, 0U, 0U, 0x06U);
        set_bg_tilemap_entry(vdp, 1U, 0U, 0x07U);
        set_bg_tilemap_entry(vdp, 2U, 0U, 0x08U);
        set_bg_tilemap_entry(vdp, 3U, 0U, 0x07U);
        set_fg_tilemap_entry(vdp, 0U, 0U, 0x0AU);
        set_fg_tilemap_entry(vdp, 1U, 0U, 0x09U);
        set_fg_tilemap_entry(vdp, 2U, 0U, 0x0AU);
        vdp.write_port(superz80::VDP::kBgScrollXPort, 0x10U);
        vdp.write_port(superz80::VDP::kFgScrollXPort, 0x08U);
        vdp.render_frame();
        ok = expect_pixel(vdp, 0U, 0U, 0x44U, "independent-scroll-foreground-uses-own-viewport") && ok;
        ok = expect_pixel(vdp, 8U, 0U, 0x22U, "independent-scroll-background-visible-beneath-transparent-foreground") &&
             ok;
    }

    {
        superz80::Bus bus;
        bus.reset();
        bus.write_port(superz80::Bus::kVdpFgScrollXPort, 0x12U);
        bus.write_port(superz80::Bus::kVdpFgScrollYPort, 0x34U);
        ok = expect_equal_u8("foreground-scroll-register-readback-x",
                             bus.read_port(superz80::Bus::kVdpFgScrollXPort),
                             0x12U) && ok;
        ok = expect_equal_u8("foreground-scroll-register-readback-y",
                             bus.read_port(superz80::Bus::kVdpFgScrollYPort),
                             0x34U) && ok;
        bus.reset();
        ok = expect_equal_u8("foreground-scroll-register-reset-x",
                             bus.read_port(superz80::Bus::kVdpFgScrollXPort),
                             0x00U) && ok;
        ok = expect_equal_u8("foreground-scroll-register-reset-y",
                             bus.read_port(superz80::Bus::kVdpFgScrollYPort),
                             0x00U) && ok;
    }

    return ok ? 0 : 1;
}
