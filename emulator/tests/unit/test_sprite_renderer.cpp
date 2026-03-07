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

bool expect_sprite_pixel(const superz80::VDP& vdp,
                         std::size_t x,
                         std::size_t y,
                         uint8_t expected,
                         const char* name) {
    return expect_equal_u8(name, vdp.framebuffer_pixel(x, y), expected);
}

void write_sprite_pattern(superz80::VDP& vdp, uint8_t tile_index, const uint8_t* pixels) {
    const auto pattern_base = static_cast<uint16_t>(
        superz80::VDP::kPatternBase + (static_cast<uint16_t>(tile_index) * superz80::VDP::kTileSizeBytes));
    for (std::size_t offset = 0U; offset < superz80::VDP::kTileSizeBytes; ++offset) {
        vdp.write_vram(static_cast<uint16_t>(pattern_base + offset), pixels[offset]);
    }
}

void write_sprite_entry(superz80::VDP& vdp, std::size_t sprite_index, uint8_t y, uint8_t x, uint8_t tile_index) {
    const auto sprite_base = static_cast<uint16_t>(
        superz80::VDP::kSpriteAttributeTableBase +
        (static_cast<uint16_t>(sprite_index) * superz80::VDP::kSpriteSizeBytes));
    vdp.write_vram(sprite_base, y);
    vdp.write_vram(static_cast<uint16_t>(sprite_base + 1U), x);
    vdp.write_vram(static_cast<uint16_t>(sprite_base + 2U), tile_index);
    vdp.write_vram(static_cast<uint16_t>(sprite_base + 3U), 0x00U);
}

void write_background_tile(superz80::VDP& vdp, uint8_t tile_index, uint8_t fill_value) {
    const auto pattern_base = static_cast<uint16_t>(
        superz80::VDP::kPatternBase + (static_cast<uint16_t>(tile_index) * superz80::VDP::kTileSizeBytes));
    for (std::size_t offset = 0U; offset < superz80::VDP::kTileSizeBytes; ++offset) {
        vdp.write_vram(static_cast<uint16_t>(pattern_base + offset), fill_value);
    }
}

void hide_all_sprites(superz80::VDP& vdp) {
    for (std::size_t sprite_index = 0U; sprite_index < superz80::VDP::kMaxSprites; ++sprite_index) {
        write_sprite_entry(vdp, sprite_index, static_cast<uint8_t>(superz80::VDP::kFramebufferHeight), 0x00U, 0x00U);
    }
}

} // namespace

int main() {
    bool ok = true;

    superz80::VDP vdp;

    {
        static constexpr uint8_t kSingleSpritePattern[superz80::VDP::kTileSizeBytes] = {
            0x10U, 0x11U, 0x12U, 0x13U, 0x14U, 0x15U, 0x16U, 0x17U,
            0x18U, 0x19U, 0x1AU, 0x1BU, 0x1CU, 0x1DU, 0x1EU, 0x1FU,
            0x20U, 0x21U, 0x22U, 0x23U, 0x24U, 0x25U, 0x26U, 0x27U,
            0x28U, 0x29U, 0x2AU, 0x2BU, 0x2CU, 0x2DU, 0x2EU, 0x2FU,
            0x30U, 0x31U, 0x32U, 0x33U, 0x34U, 0x35U, 0x36U, 0x37U,
            0x38U, 0x39U, 0x3AU, 0x3BU, 0x3CU, 0x3DU, 0x3EU, 0x3FU,
            0x40U, 0x41U, 0x42U, 0x43U, 0x44U, 0x45U, 0x46U, 0x47U,
            0x48U, 0x49U, 0x4AU, 0x4BU, 0x4CU, 0x4DU, 0x4EU, 0x4FU,
        };

        vdp.reset();
        hide_all_sprites(vdp);
        write_sprite_pattern(vdp, 0x01U, kSingleSpritePattern);
        write_sprite_entry(vdp, 0U, 0U, 0U, 0x01U);
        vdp.render_frame();
        ok = expect_sprite_pixel(vdp, 0U, 0U, 0x10U, "single-sprite-top-left") && ok;
        ok = expect_sprite_pixel(vdp, 7U, 0U, 0x17U, "single-sprite-top-right") && ok;
        ok = expect_sprite_pixel(vdp, 0U, 7U, 0x48U, "single-sprite-bottom-left") && ok;
        ok = expect_sprite_pixel(vdp, 7U, 7U, 0x4FU, "single-sprite-bottom-right") && ok;
    }

    {
        static constexpr uint8_t kTransparentSpritePattern[superz80::VDP::kTileSizeBytes] = {
            0x00U, 0x91U, 0x00U, 0x92U, 0x00U, 0x93U, 0x00U, 0x94U,
            0x95U, 0x00U, 0x96U, 0x00U, 0x97U, 0x00U, 0x98U, 0x00U,
            0x00U, 0xA1U, 0x00U, 0xA2U, 0x00U, 0xA3U, 0x00U, 0xA4U,
            0xA5U, 0x00U, 0xA6U, 0x00U, 0xA7U, 0x00U, 0xA8U, 0x00U,
            0x00U, 0xB1U, 0x00U, 0xB2U, 0x00U, 0xB3U, 0x00U, 0xB4U,
            0xB5U, 0x00U, 0xB6U, 0x00U, 0xB7U, 0x00U, 0xB8U, 0x00U,
            0x00U, 0xC1U, 0x00U, 0xC2U, 0x00U, 0xC3U, 0x00U, 0xC4U,
            0xC5U, 0x00U, 0xC6U, 0x00U, 0xC7U, 0x00U, 0xC8U, 0x00U,
        };

        vdp.reset();
        hide_all_sprites(vdp);
        write_background_tile(vdp, 0x01U, 0x55U);
        vdp.write_vram(superz80::VDP::kTilemapBase, 0x01U);
        write_sprite_pattern(vdp, 0x02U, kTransparentSpritePattern);
        write_sprite_entry(vdp, 0U, 0U, 0U, 0x02U);
        vdp.render_frame();
        ok = expect_sprite_pixel(vdp, 0U, 0U, 0x55U, "sprite-zero-keeps-background") && ok;
        ok = expect_sprite_pixel(vdp, 1U, 0U, 0x91U, "sprite-nonzero-overwrites-background") && ok;
        ok = expect_sprite_pixel(vdp, 0U, 1U, 0x95U, "sprite-row-1-overwrites-background") && ok;
        ok = expect_sprite_pixel(vdp, 1U, 1U, 0x55U, "sprite-zero-keeps-background-row-1") && ok;
    }

    {
        static constexpr uint8_t kPositionedSpritePattern[superz80::VDP::kTileSizeBytes] = {
            0x31U, 0x32U, 0x33U, 0x34U, 0x35U, 0x36U, 0x37U, 0x38U,
            0x39U, 0x3AU, 0x3BU, 0x3CU, 0x3DU, 0x3EU, 0x3FU, 0x40U,
            0x41U, 0x42U, 0x43U, 0x44U, 0x45U, 0x46U, 0x47U, 0x48U,
            0x49U, 0x4AU, 0x4BU, 0x4CU, 0x4DU, 0x4EU, 0x4FU, 0x50U,
            0x51U, 0x52U, 0x53U, 0x54U, 0x55U, 0x56U, 0x57U, 0x58U,
            0x59U, 0x5AU, 0x5BU, 0x5CU, 0x5DU, 0x5EU, 0x5FU, 0x60U,
            0x61U, 0x62U, 0x63U, 0x64U, 0x65U, 0x66U, 0x67U, 0x68U,
            0x69U, 0x6AU, 0x6BU, 0x6CU, 0x6DU, 0x6EU, 0x6FU, 0x70U,
        };

        vdp.reset();
        hide_all_sprites(vdp);
        write_sprite_pattern(vdp, 0x03U, kPositionedSpritePattern);
        write_sprite_entry(vdp, 0U, 16U, 16U, 0x03U);
        vdp.render_frame();
        ok = expect_sprite_pixel(vdp, 16U, 16U, 0x31U, "sprite-position-top-left") && ok;
        ok = expect_sprite_pixel(vdp, 23U, 16U, 0x38U, "sprite-position-top-right") && ok;
        ok = expect_sprite_pixel(vdp, 16U, 23U, 0x69U, "sprite-position-bottom-left") && ok;
        ok = expect_sprite_pixel(vdp, 23U, 23U, 0x70U, "sprite-position-bottom-right") && ok;
        ok = expect_sprite_pixel(vdp, 15U, 16U, 0x00U, "sprite-position-left-neighbor-unchanged") && ok;
        ok = expect_sprite_pixel(vdp, 16U, 15U, 0x00U, "sprite-position-top-neighbor-unchanged") && ok;
    }

    {
        static constexpr uint8_t kFirstSpritePattern[superz80::VDP::kTileSizeBytes] = {
            0x11U, 0x12U, 0x13U, 0x14U, 0x15U, 0x16U, 0x17U, 0x18U,
            0x19U, 0x1AU, 0x1BU, 0x1CU, 0x1DU, 0x1EU, 0x1FU, 0x20U,
            0x21U, 0x22U, 0x23U, 0x24U, 0x25U, 0x26U, 0x27U, 0x28U,
            0x29U, 0x2AU, 0x2BU, 0x2CU, 0x2DU, 0x2EU, 0x2FU, 0x30U,
            0x31U, 0x32U, 0x33U, 0x34U, 0x35U, 0x36U, 0x37U, 0x38U,
            0x39U, 0x3AU, 0x3BU, 0x3CU, 0x3DU, 0x3EU, 0x3FU, 0x40U,
            0x41U, 0x42U, 0x43U, 0x44U, 0x45U, 0x46U, 0x47U, 0x48U,
            0x49U, 0x4AU, 0x4BU, 0x4CU, 0x4DU, 0x4EU, 0x4FU, 0x50U,
        };
        static constexpr uint8_t kSecondSpritePattern[superz80::VDP::kTileSizeBytes] = {
            0x81U, 0x82U, 0x83U, 0x84U, 0x85U, 0x86U, 0x87U, 0x88U,
            0x89U, 0x8AU, 0x8BU, 0x8CU, 0x8DU, 0x8EU, 0x8FU, 0x90U,
            0x91U, 0x92U, 0x93U, 0x94U, 0x95U, 0x96U, 0x97U, 0x98U,
            0x99U, 0x9AU, 0x9BU, 0x9CU, 0x9DU, 0x9EU, 0x9FU, 0xA0U,
            0xA1U, 0xA2U, 0xA3U, 0xA4U, 0xA5U, 0xA6U, 0xA7U, 0xA8U,
            0xA9U, 0xAAU, 0xABU, 0xACU, 0xADU, 0xAEU, 0xAFU, 0xB0U,
            0xB1U, 0xB2U, 0xB3U, 0xB4U, 0xB5U, 0xB6U, 0xB7U, 0xB8U,
            0xB9U, 0xBAU, 0xBBU, 0xBCU, 0xBDU, 0xBEU, 0xBFU, 0xC0U,
        };

        vdp.reset();
        hide_all_sprites(vdp);
        write_sprite_pattern(vdp, 0x04U, kFirstSpritePattern);
        write_sprite_pattern(vdp, 0x05U, kSecondSpritePattern);
        write_sprite_entry(vdp, 0U, 0U, 0U, 0x04U);
        write_sprite_entry(vdp, 1U, 24U, 24U, 0x05U);
        vdp.render_frame();
        ok = expect_sprite_pixel(vdp, 0U, 0U, 0x11U, "multiple-sprites-first") && ok;
        ok = expect_sprite_pixel(vdp, 24U, 24U, 0x81U, "multiple-sprites-second") && ok;
        ok = expect_sprite_pixel(vdp, 31U, 31U, 0xC0U, "multiple-sprites-second-bottom-right") && ok;
    }

    return ok ? 0 : 1;
}
