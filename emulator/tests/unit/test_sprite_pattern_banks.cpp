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

void write_solid_tile(superz80::VDP& vdp, uint8_t tile_index, uint8_t value) {
    const auto pattern_base = static_cast<uint16_t>(superz80::VDP::kPatternBase +
                                                    (static_cast<uint16_t>(tile_index) *
                                                     superz80::VDP::kTileSizeBytes));
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

void write_sprite_pattern(superz80::VDP& vdp, uint16_t bank_base, uint8_t tile_index, const uint8_t* pixels) {
    const auto pattern_base =
        static_cast<uint16_t>(bank_base + (static_cast<uint16_t>(tile_index) * superz80::VDP::kTileSizeBytes));
    for (std::size_t offset = 0U; offset < superz80::VDP::kTileSizeBytes; ++offset) {
        vdp.write_vram(static_cast<uint16_t>(pattern_base + offset), pixels[offset]);
    }
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

constexpr uint8_t kBank0Pattern[superz80::VDP::kTileSizeBytes] = {
    0x10U, 0x11U, 0x12U, 0x13U, 0x14U, 0x15U, 0x16U, 0x17U,
    0x20U, 0x21U, 0x22U, 0x23U, 0x24U, 0x25U, 0x26U, 0x27U,
    0x30U, 0x31U, 0x32U, 0x33U, 0x34U, 0x35U, 0x36U, 0x37U,
    0x40U, 0x41U, 0x42U, 0x43U, 0x44U, 0x45U, 0x46U, 0x47U,
    0x50U, 0x51U, 0x52U, 0x53U, 0x54U, 0x55U, 0x56U, 0x57U,
    0x60U, 0x61U, 0x62U, 0x63U, 0x64U, 0x65U, 0x66U, 0x67U,
    0x70U, 0x71U, 0x72U, 0x73U, 0x74U, 0x75U, 0x76U, 0x77U,
    0x80U, 0x81U, 0x82U, 0x83U, 0x84U, 0x85U, 0x86U, 0x87U,
};

constexpr uint8_t kBank1Pattern[superz80::VDP::kTileSizeBytes] = {
    0x91U, 0x92U, 0x93U, 0x94U, 0x95U, 0x96U, 0x97U, 0x98U,
    0xA1U, 0xA2U, 0xA3U, 0xA4U, 0xA5U, 0xA6U, 0xA7U, 0xA8U,
    0xB1U, 0xB2U, 0xB3U, 0xB4U, 0xB5U, 0xB6U, 0xB7U, 0xB8U,
    0xC1U, 0xC2U, 0xC3U, 0xC4U, 0xC5U, 0xC6U, 0xC7U, 0xC8U,
    0xD1U, 0xD2U, 0xD3U, 0xD4U, 0xD5U, 0xD6U, 0xD7U, 0xD8U,
    0xE1U, 0xE2U, 0xE3U, 0xE4U, 0xE5U, 0xE6U, 0xE7U, 0xE8U,
    0xF1U, 0xF2U, 0xF3U, 0xF4U, 0xF5U, 0xF6U, 0xF7U, 0xF8U,
    0x81U, 0x82U, 0x83U, 0x84U, 0x85U, 0x86U, 0x87U, 0x88U,
};

constexpr uint8_t kTransparentBank1Pattern[superz80::VDP::kTileSizeBytes] = {
    0x00U, 0x51U, 0x00U, 0x52U, 0x00U, 0x53U, 0x00U, 0x54U,
    0x61U, 0x00U, 0x62U, 0x00U, 0x63U, 0x00U, 0x64U, 0x00U,
    0x00U, 0x71U, 0x00U, 0x72U, 0x00U, 0x73U, 0x00U, 0x74U,
    0x81U, 0x00U, 0x82U, 0x00U, 0x83U, 0x00U, 0x84U, 0x00U,
    0x00U, 0x91U, 0x00U, 0x92U, 0x00U, 0x93U, 0x00U, 0x94U,
    0xA1U, 0x00U, 0xA2U, 0x00U, 0xA3U, 0x00U, 0xA4U, 0x00U,
    0x00U, 0xB1U, 0x00U, 0xB2U, 0x00U, 0xB3U, 0x00U, 0xB4U,
    0xC1U, 0x00U, 0xC2U, 0x00U, 0xC3U, 0x00U, 0xC4U, 0x00U,
};

} // namespace

int main() {
    bool ok = true;

    {
        superz80::VDP vdp;
        vdp.reset();
        hide_all_sprites(vdp);
        write_sprite_pattern(vdp, superz80::VDP::kSpritePatternBank0Base, 0x00U, kBank0Pattern);
        write_sprite_entry(vdp, 0U, 0U, 0U, 0x00U, 0x00U);
        vdp.render_frame();
        ok = expect_pixel(vdp, 0U, 0U, 0x10U, "bank0-fetch-top-left") && ok;
        ok = expect_pixel(vdp, 7U, 7U, 0x87U, "bank0-fetch-bottom-right") && ok;
    }

    {
        superz80::VDP vdp;
        vdp.reset();
        hide_all_sprites(vdp);
        write_sprite_pattern(vdp, superz80::VDP::kSpritePatternBank1Base, 0x00U, kBank1Pattern);
        write_sprite_entry(vdp, 0U, 0U, 0U, 0x00U, superz80::VDP::kSpritePatternBankFlag);
        vdp.render_frame();
        ok = expect_pixel(vdp, 0U, 0U, 0x91U, "bank1-fetch-top-left") && ok;
        ok = expect_pixel(vdp, 7U, 7U, 0x88U, "bank1-fetch-bottom-right") && ok;
    }

    {
        superz80::VDP vdp;
        vdp.reset();
        hide_all_sprites(vdp);
        write_sprite_pattern(vdp, superz80::VDP::kSpritePatternBank0Base, 0x00U, kBank0Pattern);
        write_sprite_pattern(vdp, superz80::VDP::kSpritePatternBank1Base, 0x00U, kBank1Pattern);
        write_sprite_entry(vdp, 0U, 0U, 0U, 0x00U, 0x00U);
        write_sprite_entry(vdp, 1U, 0U, 8U, 0x00U, superz80::VDP::kSpritePatternBankFlag);
        vdp.render_frame();
        ok = expect_pixel(vdp, 0U, 0U, 0x10U, "same-tile-bank0-origin") && ok;
        ok = expect_pixel(vdp, 8U, 0U, 0x91U, "same-tile-bank1-origin") && ok;
        ok = expect_pixel(vdp, 15U, 7U, 0x88U, "same-tile-bank1-bottom-right") && ok;
    }

    {
        superz80::VDP vdp;
        vdp.reset();
        hide_all_sprites(vdp);
        write_solid_tile(vdp, 0x10U, 0x44U);
        set_bg_tile(vdp, 0U, 0U, 0x10U);
        write_sprite_pattern(vdp, superz80::VDP::kSpritePatternBank1Base, 0x00U, kTransparentBank1Pattern);
        write_sprite_entry(vdp, 0U, 0U, 0U, 0x00U, superz80::VDP::kSpritePatternBankFlag);
        vdp.render_frame();
        ok = expect_pixel(vdp, 0U, 0U, 0x44U, "bank1-zero-keeps-background") && ok;
        ok = expect_pixel(vdp, 1U, 0U, 0x51U, "bank1-nonzero-overwrites-background") && ok;
        ok = expect_pixel(vdp, 0U, 1U, 0x61U, "bank1-row1-nonzero-overwrites-background") && ok;
        ok = expect_pixel(vdp, 1U, 1U, 0x44U, "bank1-row1-zero-keeps-background") && ok;
    }

    {
        superz80::VDP vdp;
        vdp.reset();
        hide_all_sprites(vdp);
        write_solid_tile(vdp, 0x10U, 0x11U);
        write_solid_tile(vdp, 0x11U, 0x22U);
        set_bg_tile(vdp, 0U, 0U, 0x10U);
        set_fg_tile(vdp, 0U, 0U, 0x11U);
        write_sprite_pattern(vdp, superz80::VDP::kSpritePatternBank1Base, 0x00U, kBank0Pattern);
        write_sprite_entry(vdp,
                           0U,
                           0U,
                           0U,
                           0x00U,
                           static_cast<uint8_t>(superz80::VDP::kSpritePriorityFlag |
                                                superz80::VDP::kSpritePatternBankFlag |
                                                superz80::VDP::kSpriteHorizontalFlipFlag));
        write_sprite_entry(vdp,
                           1U,
                           8U,
                           0U,
                           0x00U,
                           static_cast<uint8_t>(superz80::VDP::kSpritePatternBankFlag |
                                                superz80::VDP::kSpriteHorizontalFlipFlag |
                                                superz80::VDP::kSpriteVerticalFlipFlag));
        vdp.render_frame();
        ok = expect_pixel(vdp, 0U, 0U, 0x22U, "banked-low-priority-sprite-stays-behind-foreground") && ok;
        ok = expect_pixel(vdp, 0U, 8U, 0x87U, "banked-flipped-sprite-top-left") && ok;
        ok = expect_pixel(vdp, 7U, 8U, 0x80U, "banked-flipped-sprite-top-right") && ok;
    }

    return ok ? 0 : 1;
}
