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

void write_tile(superz80::VDP& vdp, uint8_t tile_index, const uint8_t* pixels) {
    const auto pattern_base = static_cast<uint16_t>(
        superz80::VDP::kPatternBase + (static_cast<uint16_t>(tile_index) * superz80::VDP::kTileSizeBytes));
    for (std::size_t offset = 0U; offset < superz80::VDP::kTileSizeBytes; ++offset) {
        vdp.write_vram(static_cast<uint16_t>(pattern_base + offset), pixels[offset]);
    }
}

void write_solid_tile(superz80::VDP& vdp, uint8_t tile_index, uint8_t value) {
    const auto pattern_base = static_cast<uint16_t>(
        superz80::VDP::kPatternBase + (static_cast<uint16_t>(tile_index) * superz80::VDP::kTileSizeBytes));
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

} // namespace

int main() {
    bool ok = true;

    static constexpr uint8_t kPattern[superz80::VDP::kTileSizeBytes] = {
        0x10U, 0x11U, 0x12U, 0x13U, 0x14U, 0x15U, 0x16U, 0x17U,
        0x20U, 0x21U, 0x22U, 0x23U, 0x24U, 0x25U, 0x26U, 0x27U,
        0x30U, 0x31U, 0x32U, 0x33U, 0x34U, 0x35U, 0x36U, 0x37U,
        0x40U, 0x41U, 0x42U, 0x43U, 0x44U, 0x45U, 0x46U, 0x47U,
        0x50U, 0x51U, 0x52U, 0x53U, 0x54U, 0x55U, 0x56U, 0x57U,
        0x60U, 0x61U, 0x62U, 0x63U, 0x64U, 0x65U, 0x66U, 0x67U,
        0x70U, 0x71U, 0x72U, 0x73U, 0x74U, 0x75U, 0x76U, 0x77U,
        0x80U, 0x81U, 0x82U, 0x83U, 0x84U, 0x85U, 0x86U, 0x87U,
    };

    {
        superz80::VDP vdp;
        vdp.reset();
        hide_all_sprites(vdp);
        write_tile(vdp, 0x01U, kPattern);
        write_sprite_entry(vdp, 0U, 0U, 0U, 0x01U, superz80::VDP::kSpriteHorizontalFlipFlag);
        vdp.render_frame();
        ok = expect_pixel(vdp, 0U, 0U, 0x17U, "horizontal-flip-top-left") && ok;
        ok = expect_pixel(vdp, 7U, 0U, 0x10U, "horizontal-flip-top-right") && ok;
        ok = expect_pixel(vdp, 0U, 7U, 0x87U, "horizontal-flip-bottom-left") && ok;
        ok = expect_pixel(vdp, 7U, 7U, 0x80U, "horizontal-flip-bottom-right") && ok;
    }

    {
        superz80::VDP vdp;
        vdp.reset();
        hide_all_sprites(vdp);
        write_tile(vdp, 0x01U, kPattern);
        write_sprite_entry(vdp, 0U, 0U, 0U, 0x01U, superz80::VDP::kSpriteVerticalFlipFlag);
        vdp.render_frame();
        ok = expect_pixel(vdp, 0U, 0U, 0x80U, "vertical-flip-top-left") && ok;
        ok = expect_pixel(vdp, 7U, 0U, 0x87U, "vertical-flip-top-right") && ok;
        ok = expect_pixel(vdp, 0U, 7U, 0x10U, "vertical-flip-bottom-left") && ok;
        ok = expect_pixel(vdp, 7U, 7U, 0x17U, "vertical-flip-bottom-right") && ok;
    }

    {
        superz80::VDP vdp;
        vdp.reset();
        hide_all_sprites(vdp);
        write_tile(vdp, 0x01U, kPattern);
        write_sprite_entry(vdp,
                           0U,
                           0U,
                           0U,
                           0x01U,
                           static_cast<uint8_t>(superz80::VDP::kSpriteHorizontalFlipFlag |
                                                superz80::VDP::kSpriteVerticalFlipFlag));
        vdp.render_frame();
        ok = expect_pixel(vdp, 0U, 0U, 0x87U, "both-flips-top-left") && ok;
        ok = expect_pixel(vdp, 7U, 0U, 0x80U, "both-flips-top-right") && ok;
        ok = expect_pixel(vdp, 0U, 7U, 0x17U, "both-flips-bottom-left") && ok;
        ok = expect_pixel(vdp, 7U, 7U, 0x10U, "both-flips-bottom-right") && ok;
    }

    {
        superz80::VDP vdp;
        vdp.reset();
        hide_all_sprites(vdp);
        write_solid_tile(vdp, 0x10U, 0x22U);
        set_bg_tile(vdp, 0U, 0U, 0x10U);
        write_tile(vdp, 0x01U, kPattern);
        write_sprite_entry(vdp, 0U, 0U, 8U, 0x01U, superz80::VDP::kSpriteHorizontalFlipFlag);
        vdp.render_frame();
        ok = expect_pixel(vdp, 0U, 0U, 0x22U, "flip-does-not-change-background-origin") && ok;
        ok = expect_pixel(vdp, 7U, 7U, 0x22U, "flip-does-not-change-background-covered-tile") && ok;
        ok = expect_pixel(vdp, 8U, 0U, 0x17U, "flip-affects-only-sprite-region") && ok;
    }

    {
        superz80::VDP vdp;
        vdp.reset();
        hide_all_sprites(vdp);
        write_solid_tile(vdp, 0x10U, 0x11U);
        write_solid_tile(vdp, 0x11U, 0x22U);
        set_bg_tile(vdp, 0U, 0U, 0x10U);
        set_fg_tile(vdp, 0U, 0U, 0x11U);
        write_tile(vdp, 0x01U, kPattern);
        write_sprite_entry(vdp,
                           0U,
                           0U,
                           0U,
                           0x01U,
                           static_cast<uint8_t>(superz80::VDP::kSpritePriorityFlag |
                                                superz80::VDP::kSpriteHorizontalFlipFlag));
        write_sprite_entry(vdp,
                           1U,
                           8U,
                           0U,
                           0x01U,
                           static_cast<uint8_t>(superz80::VDP::kSpriteHorizontalFlipFlag));
        vdp.render_frame();
        ok = expect_pixel(vdp, 0U, 0U, 0x22U, "flipped-low-priority-sprite-stays-behind-foreground") && ok;
        ok = expect_pixel(vdp, 0U, 8U, 0x17U, "flipped-high-priority-sprite-renders") && ok;
    }

    return ok ? 0 : 1;
}
