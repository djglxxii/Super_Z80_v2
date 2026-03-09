#include "vdp.h"

#include <algorithm>

namespace superz80 {

namespace {

constexpr uint8_t kPaletteRedShift = 24U;
constexpr uint8_t kPaletteGreenShift = 16U;
constexpr uint8_t kPaletteBlueShift = 8U;
constexpr uint8_t kPaletteAlphaShift = 0U;

} // namespace

VDP::VDP()
    : vram_{},
      framebuffer_{},
      sprite_mask_{},
      palette_{},
      vram_ptr_(0U),
      control_address_low_(0U),
      palette_index_(0U),
      control_reg_(0U),
      control_address_latch_low_(false),
      bg_scroll_x_(0U),
      bg_scroll_y_(0U),
      fg_scroll_x_(0U),
      fg_scroll_y_(0U),
      bg_pattern_bank_(0U),
      fg_pattern_bank_(0U),
      sprite_scanline_list_{},
      sprite_scanline_count_(0U),
      sprite_overflow_(false),
      sprite_collision_(false),
      vblank_active_(false),
      frame_ready_(false) {}

void VDP::reset() {
    std::fill(vram_.begin(), vram_.end(), 0x00U);
    std::fill(framebuffer_.begin(), framebuffer_.end(), 0x00U);
    std::fill(sprite_mask_.begin(), sprite_mask_.end(), 0x00U);
    std::fill(palette_.begin(), palette_.end(), kPaletteResetValue);
    vram_ptr_ = 0U;
    control_address_low_ = 0x00U;
    palette_index_ = 0U;
    control_reg_ = 0x00U;
    control_address_latch_low_ = false;
    bg_scroll_x_ = 0x00U;
    bg_scroll_y_ = 0x00U;
    fg_scroll_x_ = 0x00U;
    fg_scroll_y_ = 0x00U;
    bg_pattern_bank_ = 0x00U;
    fg_pattern_bank_ = 0x00U;
    sprite_scanline_list_.fill(0x00U);
    sprite_scanline_count_ = 0U;
    sprite_overflow_ = false;
    sprite_collision_ = false;
    vblank_active_ = false;
    frame_ready_ = false;
}

uint8_t VDP::read_port(uint8_t port) {
    if (port == kStatusPort) {
        const uint8_t value = status();
        frame_ready_ = false;
        sprite_overflow_ = false;
        sprite_collision_ = false;
        return value;
    }

    if (port == kDataPort) {
        return vram_[vram_ptr_++];
    }

    if (port == kPaletteIndexPort) {
        return palette_index_;
    }

    if (port == kPaletteDataRPort) {
        return palette_component(palette_index_, kPaletteRedShift);
    }

    if (port == kPaletteDataGPort) {
        return palette_component(palette_index_, kPaletteGreenShift);
    }

    if (port == kPaletteDataBPort) {
        return palette_component(palette_index_, kPaletteBlueShift);
    }

    if (port == kPaletteDataAPort) {
        return palette_component(palette_index_, kPaletteAlphaShift);
    }

    if (port == kBgScrollXPort) {
        return bg_scroll_x_;
    }

    if (port == kBgScrollYPort) {
        return bg_scroll_y_;
    }

    if (port == kFgScrollXPort) {
        return fg_scroll_x_;
    }

    if (port == kFgScrollYPort) {
        return fg_scroll_y_;
    }

    if (port == kBgPatternBankPort) {
        return bg_pattern_bank_;
    }

    if (port == kFgPatternBankPort) {
        return fg_pattern_bank_;
    }

    return 0x00U;
}

void VDP::write_port(uint8_t port, uint8_t value) {
    if (port == kControlPort) {
        control_reg_ = value;
        if (!control_address_latch_low_) {
            control_address_low_ = value;
            control_address_latch_low_ = true;
        } else {
            vram_ptr_ = static_cast<uint16_t>((static_cast<uint16_t>(value) << 8U) |
                                              static_cast<uint16_t>(control_address_low_));
            control_address_latch_low_ = false;
        }
        return;
    }

    if (port == kDataPort) {
        vram_[vram_ptr_++] = value;
        return;
    }

    if (port == kPaletteIndexPort) {
        palette_index_ = value;
        return;
    }

    if (port == kPaletteDataRPort) {
        set_palette_component(palette_index_, kPaletteRedShift, value);
        return;
    }

    if (port == kPaletteDataGPort) {
        set_palette_component(palette_index_, kPaletteGreenShift, value);
        return;
    }

    if (port == kPaletteDataBPort) {
        set_palette_component(palette_index_, kPaletteBlueShift, value);
        return;
    }

    if (port == kPaletteDataAPort) {
        set_palette_component(palette_index_, kPaletteAlphaShift, value);
        return;
    }

    if (port == kBgScrollXPort) {
        bg_scroll_x_ = value;
        return;
    }

    if (port == kBgScrollYPort) {
        bg_scroll_y_ = value;
        return;
    }

    if (port == kFgScrollXPort) {
        fg_scroll_x_ = value;
        return;
    }

    if (port == kFgScrollYPort) {
        fg_scroll_y_ = value;
        return;
    }

    if (port == kBgPatternBankPort) {
        bg_pattern_bank_ = value & 0x01U;
        return;
    }

    if (port == kFgPatternBankPort) {
        fg_pattern_bank_ = value & 0x01U;
    }
}

void VDP::render_frame() {
    std::fill(framebuffer_.begin(), framebuffer_.end(), 0x00U);
    std::fill(sprite_mask_.begin(), sprite_mask_.end(), 0x00U);
    sprite_scanline_count_ = 0U;
    sprite_overflow_ = false;
    sprite_collision_ = false;
    render_background();
    render_sprites(true);
    render_foreground();
    render_sprites(false);
}

void VDP::render_background() {
    render_plane(kBgTilemapBase, bg_scroll_x_, bg_scroll_y_, bg_pattern_bank_, false);
}

void VDP::render_foreground() {
    render_plane(kFgTilemapBase, fg_scroll_x_, fg_scroll_y_, fg_pattern_bank_, true);
}

void VDP::render_plane(uint16_t tilemap_base,
                       uint8_t scroll_x,
                       uint8_t scroll_y,
                       uint8_t pattern_bank,
                       bool transparent_zero) {
    const uint16_t pattern_base = pattern_bank_base(pattern_bank);

    for (std::size_t screen_y = 0U; screen_y < kFramebufferHeight; ++screen_y) {
        const std::size_t world_y = (screen_y + scroll_y) % kFramebufferHeight;
        const std::size_t tile_y = world_y / kTileHeight;
        const std::size_t pixel_y = world_y % kTileHeight;

        for (std::size_t screen_x = 0U; screen_x < kFramebufferWidth; ++screen_x) {
            const std::size_t world_x = (screen_x + scroll_x) % kFramebufferWidth;
            const std::size_t tile_x = world_x / kTileWidth;
            const std::size_t pixel_x = world_x % kTileWidth;
            const auto tilemap_offset = static_cast<uint16_t>((tile_y * kTilemapWidth) + tile_x);
            const uint8_t tile_index = vram_[tilemap_base + tilemap_offset];
            const auto pattern_offset = static_cast<std::size_t>(tile_index) * kTileSizeBytes +
                                        (pixel_y * kTileWidth) + pixel_x;
            const uint8_t pixel_value = vram_[pattern_base + pattern_offset];
            if (transparent_zero && pixel_value == 0x00U) {
                continue;
            }

            framebuffer_[(screen_y * kFramebufferWidth) + screen_x] = pixel_value;
        }
    }
}

void VDP::render_sprites(bool behind_foreground) {
    for (std::size_t scanline = 0U; scanline < kFramebufferHeight; ++scanline) {
        evaluate_sprites_for_scanline(scanline);
        render_sprites_for_scanline(scanline, behind_foreground);
    }
}

void VDP::evaluate_sprites_for_scanline(std::size_t scanline) {
    sprite_scanline_count_ = 0U;

    for (std::size_t sprite_index = 0U; sprite_index < kMaxSprites; ++sprite_index) {
        const auto sprite_base =
            static_cast<std::size_t>(kSpriteAttributeTableBase) + (sprite_index * kSpriteSizeBytes);
        const std::size_t sprite_y = vram_[sprite_base];
        if (scanline < sprite_y || scanline >= (sprite_y + kTileHeight)) {
            continue;
        }

        if (sprite_scanline_count_ < kSpritesPerScanline) {
            sprite_scanline_list_[sprite_scanline_count_] = static_cast<uint8_t>(sprite_index);
            ++sprite_scanline_count_;
            continue;
        }

        sprite_overflow_ = true;
    }
}

void VDP::render_sprites_for_scanline(std::size_t scanline, bool behind_foreground) {
    const auto framebuffer_row = scanline * kFramebufferWidth;

    for (std::size_t list_index = 0U; list_index < sprite_scanline_count_; ++list_index) {
        const std::size_t sprite_index = sprite_scanline_list_[list_index];
        const auto sprite_base =
            static_cast<std::size_t>(kSpriteAttributeTableBase) + (sprite_index * kSpriteSizeBytes);
        const std::size_t sprite_y = vram_[sprite_base];
        const std::size_t sprite_x = vram_[sprite_base + 1U];
        const uint8_t tile_index = vram_[sprite_base + 2U];
        const uint8_t flags = vram_[sprite_base + 3U];
        const bool sprite_behind_foreground = (flags & kSpritePriorityFlag) != 0U;

        if (sprite_behind_foreground != behind_foreground || sprite_x >= kFramebufferWidth) {
            continue;
        }

        const std::size_t sprite_row = scanline - sprite_y;
        const bool horizontal_flip = (flags & kSpriteHorizontalFlipFlag) != 0U;
        const bool vertical_flip = (flags & kSpriteVerticalFlipFlag) != 0U;
        const uint16_t pattern_base =
            (flags & kSpritePatternBankFlag) != 0U ? kSpritePatternBank1Base : kSpritePatternBank0Base;
        const std::size_t pattern_row_index = vertical_flip ? (kTileHeight - 1U - sprite_row) : sprite_row;
        const auto pattern_row = static_cast<std::size_t>(pattern_base) +
                                 (static_cast<std::size_t>(tile_index) * kTileSizeBytes) +
                                 (pattern_row_index * kTileWidth);
        for (std::size_t pixel_x = 0U; pixel_x < kTileWidth; ++pixel_x) {
            const std::size_t framebuffer_x = sprite_x + pixel_x;
            if (framebuffer_x >= kFramebufferWidth) {
                break;
            }

            const std::size_t pattern_column_index = horizontal_flip ? (kTileWidth - 1U - pixel_x) : pixel_x;
            const uint8_t pixel_value = vram_[pattern_row + pattern_column_index];
            if (pixel_value == 0x00U) {
                continue;
            }

            if (sprite_mask_[framebuffer_row + framebuffer_x] != 0x00U) {
                sprite_collision_ = true;
            }

            sprite_mask_[framebuffer_row + framebuffer_x] = 0x01U;
            framebuffer_[framebuffer_row + framebuffer_x] = pixel_value;
        }
    }
}

void VDP::step_scanline(uint32_t scanline) {
    if (scanline == 0U) {
        vblank_active_ = false;
    } else if (scanline >= kVBlankBeginScanline) {
        vblank_active_ = true;
    }

    if (scanline == kFrameReadyScanline) {
        frame_ready_ = true;
    }
}

bool VDP::frame_ready() const {
    return frame_ready_;
}

uint16_t VDP::vram_pointer() const {
    return vram_ptr_;
}

uint8_t VDP::control_reg() const {
    return control_reg_;
}

void VDP::write_vram(uint16_t address, uint8_t value) {
    vram_[address] = value;
}

uint8_t VDP::vram(uint16_t address) const {
    return vram_[address];
}

uint8_t VDP::framebuffer_pixel(std::size_t x, std::size_t y) const {
    return framebuffer_[(y * kFramebufferWidth) + x];
}

uint32_t VDP::palette_entry(uint8_t index) const {
    return palette_[index];
}

uint32_t VDP::framebuffer_color(std::size_t pixel_index) const {
    return resolve_pixel(framebuffer_[pixel_index]);
}

void VDP::set_palette_entry(uint8_t index, uint32_t rgba) {
    palette_[index] = rgba;
}

uint32_t VDP::resolve_pixel(uint8_t palette_index) const {
    return palette_entry(palette_index);
}

uint8_t VDP::bg_scroll_x() const {
    return bg_scroll_x_;
}

uint8_t VDP::bg_scroll_y() const {
    return bg_scroll_y_;
}

uint8_t VDP::fg_scroll_x() const {
    return fg_scroll_x_;
}

uint8_t VDP::fg_scroll_y() const {
    return fg_scroll_y_;
}

uint8_t VDP::bg_pattern_bank() const {
    return bg_pattern_bank_;
}

uint8_t VDP::fg_pattern_bank() const {
    return fg_pattern_bank_;
}

uint16_t VDP::pattern_bank_base(uint8_t bank) const {
    return bank == 0x00U ? kPatternBank0Base : kPatternBank1Base;
}

uint8_t VDP::palette_component(uint8_t index, uint8_t shift) const {
    return static_cast<uint8_t>((palette_[index] >> shift) & 0xFFU);
}

void VDP::set_palette_component(uint8_t index, uint8_t shift, uint8_t value) {
    const uint32_t component_mask = static_cast<uint32_t>(0xFFU) << shift;
    palette_[index] = (palette_[index] & ~component_mask) | (static_cast<uint32_t>(value) << shift);
}

uint8_t VDP::status() const {
    uint8_t value = 0x00U;

    if (vblank_active_) {
        value |= 0x01U;
    }

    if (frame_ready_) {
        value |= 0x02U;
    }

    if (sprite_overflow_) {
        value |= 0x04U;
    }

    if (sprite_collision_) {
        value |= 0x08U;
    }

    return value;
}

} // namespace superz80
