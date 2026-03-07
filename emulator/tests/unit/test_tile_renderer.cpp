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

bool expect_tile_pixel(const superz80::VDP& vdp,
                       std::size_t x,
                       std::size_t y,
                       uint8_t expected,
                       const char* name) {
    return expect_equal_u8(name, vdp.framebuffer_pixel(x, y), expected);
}

void hide_foreground_plane(superz80::VDP& vdp) {
    for (std::size_t tile_y = 0U; tile_y < superz80::VDP::kTilemapHeight; ++tile_y) {
        for (std::size_t tile_x = 0U; tile_x < superz80::VDP::kTilemapWidth; ++tile_x) {
            const auto offset =
                static_cast<uint16_t>((tile_y * superz80::VDP::kTilemapWidth) + tile_x);
            vdp.write_vram(static_cast<uint16_t>(superz80::VDP::kFgTilemapBase + offset), 0xFFU);
        }
    }
}

} // namespace

int main() {
    bool ok = true;

    superz80::VDP vdp;

    vdp.reset();
    for (std::size_t y = 0U; y < superz80::VDP::kTileHeight; ++y) {
        for (std::size_t x = 0U; x < superz80::VDP::kTileWidth; ++x) {
            const auto address =
                static_cast<uint16_t>(superz80::VDP::kPatternBase + (y * superz80::VDP::kTileWidth) + x);
            vdp.write_vram(address, static_cast<uint8_t>((y * superz80::VDP::kTileWidth) + x));
        }
    }
    vdp.write_vram(superz80::VDP::kTilemapBase, 0x00U);
    hide_foreground_plane(vdp);
    vdp.render_frame();
    for (std::size_t y = 0U; y < superz80::VDP::kTileHeight; ++y) {
        for (std::size_t x = 0U; x < superz80::VDP::kTileWidth; ++x) {
            const auto expected = static_cast<uint8_t>((y * superz80::VDP::kTileWidth) + x);
            ok = expect_tile_pixel(vdp, x, y, expected, "single-tile-render") && ok;
        }
    }

    vdp.reset();
    for (std::size_t y = 0U; y < superz80::VDP::kTileHeight; ++y) {
        for (std::size_t x = 0U; x < superz80::VDP::kTileWidth; ++x) {
            const auto offset = static_cast<uint16_t>((y * superz80::VDP::kTileWidth) + x);
            vdp.write_vram(static_cast<uint16_t>(superz80::VDP::kPatternBase + offset),
                           static_cast<uint8_t>(0x10U + offset));
            vdp.write_vram(
                static_cast<uint16_t>(superz80::VDP::kPatternBase + superz80::VDP::kTileSizeBytes + offset),
                static_cast<uint8_t>(0x80U + offset));
        }
    }
    vdp.write_vram(superz80::VDP::kTilemapBase, 0x00U);
    vdp.write_vram(static_cast<uint16_t>(superz80::VDP::kTilemapBase + 1U), 0x01U);
    hide_foreground_plane(vdp);
    vdp.render_frame();
    ok = expect_tile_pixel(vdp, 0U, 0U, 0x10U, "tilemap-placement-left-origin") && ok;
    ok = expect_tile_pixel(vdp, 7U, 0U, 0x17U, "tilemap-placement-left-right-edge") && ok;
    ok = expect_tile_pixel(vdp, 8U, 0U, 0x80U, "tilemap-placement-right-origin") && ok;
    ok = expect_tile_pixel(vdp, 15U, 7U, 0xBFU, "tilemap-placement-right-bottom-edge") && ok;

    vdp.reset();
    hide_foreground_plane(vdp);
    vdp.render_frame();
    ok = expect_tile_pixel(vdp, 0U, 0U, 0x00U, "default-zero-top-left") && ok;
    ok = expect_tile_pixel(vdp, 128U, 96U, 0x00U, "default-zero-middle") && ok;
    ok = expect_tile_pixel(vdp, 255U, 191U, 0x00U, "default-zero-bottom-right") && ok;

    vdp.reset();
    for (std::size_t y = 0U; y < superz80::VDP::kTileHeight; ++y) {
        for (std::size_t x = 0U; x < superz80::VDP::kTileWidth; ++x) {
            const auto address =
                static_cast<uint16_t>(superz80::VDP::kPatternBase + (y * superz80::VDP::kTileWidth) + x);
            vdp.write_vram(address, static_cast<uint8_t>((y * 0x10U) + x));
        }
    }
    vdp.write_vram(superz80::VDP::kTilemapBase, 0x00U);
    hide_foreground_plane(vdp);
    vdp.render_frame();
    ok = expect_tile_pixel(vdp, 0U, 0U, 0x00U, "tile-row-0-address") && ok;
    ok = expect_tile_pixel(vdp, 0U, 1U, 0x10U, "tile-row-1-address") && ok;
    ok = expect_tile_pixel(vdp, 0U, 2U, 0x20U, "tile-row-2-address") && ok;
    ok = expect_tile_pixel(vdp, 3U, 7U, 0x73U, "tile-row-7-address") && ok;

    return ok ? 0 : 1;
}
