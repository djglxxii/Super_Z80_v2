#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace superz80 {

class VDP {
public:
    static constexpr uint8_t kStatusPort = 0xE0U;
    static constexpr uint8_t kControlPort = 0xE1U;
    static constexpr uint8_t kDataPort = 0xE2U;
    static constexpr uint8_t kPaletteIndexPort = 0xE3U;
    static constexpr uint8_t kPaletteDataRPort = 0xE4U;
    static constexpr uint8_t kPaletteDataGPort = 0xE5U;
    static constexpr uint8_t kPaletteDataBPort = 0xE6U;
    static constexpr uint8_t kPaletteDataAPort = 0xE7U;
    static constexpr uint8_t kBgScrollXPort = 0xE8U;
    static constexpr uint8_t kBgScrollYPort = 0xE9U;
    static constexpr uint8_t kFgScrollXPort = 0xEAU;
    static constexpr uint8_t kFgScrollYPort = 0xEBU;
    static constexpr uint8_t kBgPatternBankPort = 0xECU;
    static constexpr uint8_t kFgPatternBankPort = 0xEDU;
    static constexpr std::size_t kFramebufferWidth = 256U;
    static constexpr std::size_t kFramebufferHeight = 192U;
    static constexpr std::size_t kTileWidth = 8U;
    static constexpr std::size_t kTileHeight = 8U;
    static constexpr std::size_t kTilemapWidth = 32U;
    static constexpr std::size_t kTilemapHeight = 24U;
    static constexpr uint16_t kPatternBase = 0x0000U;
    static constexpr uint16_t kPatternBank0Base = 0x0000U;
    static constexpr uint16_t kPatternBank1Base = 0x2000U;
    static constexpr uint16_t kSpritePatternBank0Base = 0x0000U;
    static constexpr uint16_t kSpritePatternBank1Base = 0x2000U;
    static constexpr uint16_t kBgTilemapBase = 0x4000U;
    static constexpr uint16_t kTilemapBase = kBgTilemapBase;
    static constexpr uint16_t kFgTilemapBase = 0x5000U;
    static constexpr uint16_t kSpriteAttributeTableBase = 0x6000U;
    static constexpr std::size_t kTileSizeBytes = kTileWidth * kTileHeight;
    static constexpr std::size_t kSpriteSizeBytes = 4U;
    static constexpr std::size_t kMaxSprites = 64U;
    static constexpr std::size_t kSpritesPerScanline = 16U;
    static constexpr uint8_t kSpritePriorityFlag = 0x01U;
    static constexpr uint8_t kSpriteHorizontalFlipFlag = 0x02U;
    static constexpr uint8_t kSpriteVerticalFlipFlag = 0x04U;
    static constexpr uint8_t kSpritePatternBankFlag = 0x08U;
    static constexpr uint8_t kSpriteReservedFlagsMask = 0xF0U;
    static constexpr uint32_t kFrameReadyScanline = 191U;
    static constexpr uint32_t kVBlankBeginScanline = 192U;
    static constexpr std::size_t kVramSize = 65536U;
    static constexpr std::size_t kFramebufferSize = kFramebufferWidth * kFramebufferHeight;
    static constexpr std::size_t kPaletteEntryCount = 256U;
    static constexpr uint32_t kPaletteResetValue = 0x00000000U;

    VDP();

    void reset();

    uint8_t read_port(uint8_t port);
    void write_port(uint8_t port, uint8_t value);

    void render_frame();
    void step_scanline(uint32_t scanline);

    bool frame_ready() const;
    uint16_t vram_pointer() const;
    uint8_t control_reg() const;
    void write_vram(uint16_t address, uint8_t value);
    uint8_t vram(uint16_t address) const;
    uint8_t framebuffer_pixel(std::size_t x, std::size_t y) const;
    uint32_t palette_entry(uint8_t index) const;
    uint32_t framebuffer_color(std::size_t pixel_index) const;
    void set_palette_entry(uint8_t index, uint32_t rgba);
    uint32_t resolve_pixel(uint8_t palette_index) const;
    uint8_t bg_scroll_x() const;
    uint8_t bg_scroll_y() const;
    uint8_t fg_scroll_x() const;
    uint8_t fg_scroll_y() const;
    uint8_t bg_pattern_bank() const;
    uint8_t fg_pattern_bank() const;

private:
    void render_background();
    void render_foreground();
    void render_plane(uint16_t tilemap_base,
                      uint8_t scroll_x,
                      uint8_t scroll_y,
                      uint8_t pattern_bank,
                      bool transparent_zero);
    void evaluate_sprites_for_scanline(std::size_t scanline);
    void render_sprites(bool behind_foreground);
    void render_sprites_for_scanline(std::size_t scanline, bool behind_foreground);
    uint16_t pattern_bank_base(uint8_t bank) const;
    uint8_t palette_component(uint8_t index, uint8_t shift) const;
    void set_palette_component(uint8_t index, uint8_t shift, uint8_t value);

    uint8_t status() const;

    std::array<uint8_t, kVramSize> vram_;
    std::array<uint8_t, kFramebufferSize> framebuffer_;
    std::array<uint8_t, kFramebufferSize> sprite_mask_;
    std::array<uint32_t, kPaletteEntryCount> palette_;
    uint16_t vram_ptr_;
    uint8_t control_address_low_;
    uint8_t palette_index_;
    uint8_t control_reg_;
    bool control_address_latch_low_;
    uint8_t bg_scroll_x_;
    uint8_t bg_scroll_y_;
    uint8_t fg_scroll_x_;
    uint8_t fg_scroll_y_;
    uint8_t bg_pattern_bank_;
    uint8_t fg_pattern_bank_;
    std::array<uint8_t, kSpritesPerScanline> sprite_scanline_list_;
    uint8_t sprite_scanline_count_;
    bool sprite_overflow_;
    bool sprite_collision_;
    bool vblank_active_;
    bool frame_ready_;
};

} // namespace superz80
