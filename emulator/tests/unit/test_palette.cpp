#include "vdp.h"

#include <cstddef>
#include <cstdint>
#include <iostream>

namespace {

constexpr uint32_t rgba(uint8_t red, uint8_t green, uint8_t blue, uint8_t alpha) {
    return (static_cast<uint32_t>(red) << 24U) | (static_cast<uint32_t>(green) << 16U) |
           (static_cast<uint32_t>(blue) << 8U) | static_cast<uint32_t>(alpha);
}

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

bool expect_equal_u32(const char* name, uint32_t actual, uint32_t expected) {
    if (actual != expected) {
        std::cerr << "[FAIL] " << name << " expected=0x" << std::hex << std::uppercase << expected
                  << " actual=0x" << actual << std::dec << std::nouppercase << std::endl;
        return false;
    }

    std::cout << "[PASS] " << name << " value=0x" << std::hex << std::uppercase << actual
              << std::dec << std::nouppercase << std::endl;
    return true;
}

void write_first_tile_pixel(superz80::VDP& vdp, uint8_t palette_index) {
    vdp.write_vram(superz80::VDP::kPatternBase, palette_index);
    vdp.write_vram(superz80::VDP::kTilemapBase, 0x00U);
}

std::size_t framebuffer_index(std::size_t x, std::size_t y) {
    return (y * superz80::VDP::kFramebufferWidth) + x;
}

} // namespace

int main() {
    bool ok = true;

    superz80::VDP vdp;

    vdp.reset();
    vdp.write_port(superz80::VDP::kPaletteIndexPort, 0x12U);
    vdp.write_port(superz80::VDP::kPaletteDataRPort, 0x34U);
    vdp.write_port(superz80::VDP::kPaletteDataGPort, 0x56U);
    vdp.write_port(superz80::VDP::kPaletteDataBPort, 0x78U);
    vdp.write_port(superz80::VDP::kPaletteDataAPort, 0x9AU);
    ok = expect_equal_u8("palette-readback-index", vdp.read_port(superz80::VDP::kPaletteIndexPort), 0x12U) && ok;
    ok = expect_equal_u8("palette-readback-red", vdp.read_port(superz80::VDP::kPaletteDataRPort), 0x34U) && ok;
    ok = expect_equal_u8("palette-readback-green", vdp.read_port(superz80::VDP::kPaletteDataGPort), 0x56U) && ok;
    ok = expect_equal_u8("palette-readback-blue", vdp.read_port(superz80::VDP::kPaletteDataBPort), 0x78U) && ok;
    ok = expect_equal_u8("palette-readback-alpha", vdp.read_port(superz80::VDP::kPaletteDataAPort), 0x9AU) && ok;
    ok = expect_equal_u32("palette-packed-entry", vdp.palette_entry(0x12U), rgba(0x34U, 0x56U, 0x78U, 0x9AU)) && ok;

    vdp.reset();
    vdp.set_palette_entry(0x22U, rgba(0x10U, 0x20U, 0x30U, 0x40U));
    vdp.set_palette_entry(0x22U, rgba(0xAAU, 0xBBU, 0xCCU, 0xDDU));
    ok = expect_equal_u32("palette-entry-overwrite", vdp.palette_entry(0x22U), rgba(0xAAU, 0xBBU, 0xCCU, 0xDDU)) && ok;

    vdp.reset();
    vdp.set_palette_entry(0x05U, rgba(0x01U, 0x23U, 0x45U, 0x67U));
    write_first_tile_pixel(vdp, 0x05U);
    vdp.render_frame();
    ok = expect_equal_u32("framebuffer-pixel-resolves-through-palette",
                          vdp.framebuffer_color(framebuffer_index(0U, 0U)),
                          rgba(0x01U, 0x23U, 0x45U, 0x67U)) && ok;

    vdp.reset();
    vdp.set_palette_entry(0x01U, rgba(0x10U, 0x11U, 0x12U, 0x13U));
    vdp.set_palette_entry(0x02U, rgba(0x20U, 0x21U, 0x22U, 0x23U));
    vdp.write_vram(superz80::VDP::kPatternBase, 0x01U);
    vdp.write_vram(static_cast<uint16_t>(superz80::VDP::kPatternBase + 1U), 0x02U);
    vdp.write_vram(superz80::VDP::kTilemapBase, 0x00U);
    vdp.render_frame();
    ok = expect_equal_u32("framebuffer-first-index-resolves-independently",
                          vdp.framebuffer_color(framebuffer_index(0U, 0U)),
                          rgba(0x10U, 0x11U, 0x12U, 0x13U)) && ok;
    ok = expect_equal_u32("framebuffer-second-index-resolves-independently",
                          vdp.framebuffer_color(framebuffer_index(1U, 0U)),
                          rgba(0x20U, 0x21U, 0x22U, 0x23U)) && ok;

    vdp.reset();
    vdp.set_palette_entry(0x7FU, rgba(0xDEU, 0xADU, 0xBEU, 0xEFU));
    vdp.reset();
    ok = expect_equal_u32("reset-clears-palette-entry", vdp.palette_entry(0x7FU), superz80::VDP::kPaletteResetValue) && ok;
    ok = expect_equal_u32("reset-clears-palette-entry-zero", vdp.palette_entry(0x00U), superz80::VDP::kPaletteResetValue) && ok;

    return ok ? 0 : 1;
}
