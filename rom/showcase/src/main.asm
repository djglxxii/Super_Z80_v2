; Minimal SDK-backed Showcase validation ROM.

    INCLUDE "sdk/inc/super_z80.inc"
    INCLUDE "sdk/runtime/inc/runtime.inc"

    ORG SZ_RUNTIME_MAIN_ORG

SHOWCASE_BG_BUFFER            EQU SZ_RAM_BASE
SHOWCASE_SPLASH_ROW_OFFSET    EQU (6 * SZ_VDP_TILEMAP_WIDTH) + 13
SHOWCASE_TEXT_ROW0_OFFSET     EQU (10 * SZ_VDP_TILEMAP_WIDTH) + 9
SHOWCASE_TEXT_ROW1_OFFSET     EQU (12 * SZ_VDP_TILEMAP_WIDTH) + 8
SHOWCASE_SPLASH_TILE_BASE     EQU sz_font_8x8_tile_count
SHOWCASE_VRAM_PADDING_BYTES   EQU SZ_VDP_BG_TILEMAP_BASE - (sz_font_8x8_end - sz_font_8x8) - (sz_splash_tiles_end - sz_splash_tiles)

sdk_main:
    call showcase_init_palette
    call showcase_prepare_bg_tilemap
    call showcase_stream_scene

showcase_main_loop:
    jp showcase_main_loop

showcase_init_palette:
    SZ_VDP_SET_PALETTE_RGBA 0, $08, $0C, $20, $FF
    SZ_VDP_SET_PALETTE_RGBA 1, $E8, $F0, $FF, $FF
    SZ_VDP_SET_PALETTE_RGBA 2, $FF, $C0, $40, $FF
    SZ_VDP_SET_PALETTE_RGBA 3, $20, $80, $E0, $FF
    SZ_VDP_SET_PALETTE_RGBA 4, $70, $24, $44, $FF
    ret

showcase_prepare_bg_tilemap:
    ld hl, SHOWCASE_BG_BUFFER
    ld de, SHOWCASE_BG_BUFFER + 1
    xor a
    ld (hl), a
    ld bc, SZ_VDP_TILEMAP_BYTES - 1
    ldir

    ld hl, showcase_splash_row
    ld de, SHOWCASE_BG_BUFFER + SHOWCASE_SPLASH_ROW_OFFSET
    ld bc, showcase_splash_row_end - showcase_splash_row
    ldir

    ld hl, showcase_message_line0
    ld de, SHOWCASE_BG_BUFFER + SHOWCASE_TEXT_ROW0_OFFSET
    call showcase_write_ascii_line

    ld hl, showcase_message_line1
    ld de, SHOWCASE_BG_BUFFER + SHOWCASE_TEXT_ROW1_OFFSET
    call showcase_write_ascii_line
    ret

showcase_write_ascii_line:
    ld a, (hl)
    or a
    ret z
    sub $20
    ld (de), a
    inc hl
    inc de
    jp showcase_write_ascii_line

showcase_stream_scene:
    ld hl, sz_font_8x8
    ld bc, sz_font_8x8_end - sz_font_8x8
    call showcase_stream_bytes

    ld hl, sz_splash_tiles
    ld bc, sz_splash_tiles_end - sz_splash_tiles
    call showcase_stream_bytes

    ld bc, SHOWCASE_VRAM_PADDING_BYTES
    call showcase_stream_zeroes

    ld hl, SHOWCASE_BG_BUFFER
    ld bc, SZ_VDP_TILEMAP_BYTES
    call showcase_stream_bytes
    ret

showcase_stream_bytes:
    ld a, b
    or c
    ret z
.loop
    ld a, (hl)
    out (SZ_PORT_VDP_DATA), a
    inc hl
    dec bc
    ld a, b
    or c
    jp nz, .loop
    ret

showcase_stream_zeroes:
    ld a, b
    or c
    ret z
.loop
    xor a
    out (SZ_PORT_VDP_DATA), a
    dec bc
    ld a, b
    or c
    jp nz, .loop
    ret

showcase_splash_row:
    db SHOWCASE_SPLASH_TILE_BASE + 3
    db SHOWCASE_SPLASH_TILE_BASE + 4
    db SHOWCASE_SPLASH_TILE_BASE + 1
    db SHOWCASE_SPLASH_TILE_BASE + 2
    db SHOWCASE_SPLASH_TILE_BASE + 4
    db SHOWCASE_SPLASH_TILE_BASE + 3
showcase_splash_row_end:

showcase_message_line0:
    db "SUPER Z80 SDK", 0

showcase_message_line1:
    db "BOOT VALIDATED", 0

    INCLUDE "sdk/assets/font_8x8.asm"
    INCLUDE "sdk/assets/splash_tiles.asm"

    ASSERT SHOWCASE_VRAM_PADDING_BYTES >= 0
