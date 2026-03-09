showcase_font_8x8_first_ascii EQU $20
showcase_font_8x8_last_ascii  EQU $7F

showcase_font_8x8:
    INCLUDE "assets/font_8x8.asm"
showcase_font_8x8_end:
showcase_font_8x8_tile_count EQU (showcase_font_8x8_end - showcase_font_8x8) / 64

showcase_splash_tiles:
    INCLUDE "assets/splash_tiles.asm"
showcase_splash_tiles_end:
showcase_splash_tile_count EQU (showcase_splash_tiles_end - showcase_splash_tiles) / 64

showcase_vdp_init_registers:
    xor a
    out (SZ_PORT_VDP_CONTROL), a
    out (SZ_PORT_VDP_BG_SCROLL_X), a
    out (SZ_PORT_VDP_BG_SCROLL_Y), a
    out (SZ_PORT_VDP_FG_SCROLL_X), a
    out (SZ_PORT_VDP_FG_SCROLL_Y), a
    out (SZ_PORT_VDP_BG_PATTERN_BANK), a
    out (SZ_PORT_VDP_FG_PATTERN_BANK), a
    ret

showcase_init_palette:
    SZ_VDP_SET_PALETTE_RGBA 0, $08, $0C, $20, $FF
    SZ_VDP_SET_PALETTE_RGBA 1, $E8, $F0, $FF, $FF
    SZ_VDP_SET_PALETTE_RGBA 2, $FF, $C0, $40, $FF
    SZ_VDP_SET_PALETTE_RGBA 3, $20, $80, $E0, $FF
    SZ_VDP_SET_PALETTE_RGBA 4, $70, $24, $44, $FF
    ret

showcase_upload_scene_to_vram:
    ld hl, SHOWCASE_VRAM_START
    call vdp_set_address

    ld hl, showcase_font_8x8
    ld bc, showcase_font_8x8_end - showcase_font_8x8
    call vdp_upload_block

    ld hl, showcase_splash_tiles
    ld bc, showcase_splash_tiles_end - showcase_splash_tiles
    call vdp_upload_block

    ld bc, SHOWCASE_PATTERN_PADDING_BYTES
    call showcase_vdp_clear_bytes

    ld hl, SHOWCASE_BG_BUFFER
    ld bc, SZ_VDP_TILEMAP_BYTES
    call vdp_upload_block

    call vdp_clear_tilemap

    ld bc, SHOWCASE_VRAM_TAIL_CLEAR_BYTES
    call showcase_vdp_clear_bytes
    ret

vdp_set_address:
    ; The current VDP contract only exposes sequential writes from reset.
    ; Track the requested address in RAM so later milestones can reuse the
    ; helper surface without changing callers.
    ld (SHOWCASE_VRAM_CURSOR), hl
    ret

vdp_write_vram:
    out (SZ_PORT_VDP_DATA), a
    push hl
    ld hl, (SHOWCASE_VRAM_CURSOR)
    inc hl
    ld (SHOWCASE_VRAM_CURSOR), hl
    pop hl
    ret

vdp_upload_block:
    ld a, b
    or c
    ret z
.upload_loop
    ld a, (hl)
    call vdp_write_vram
    inc hl
    dec bc
    ld a, b
    or c
    jp nz, .upload_loop
    ret

vdp_clear_tilemap:
    ld bc, SZ_VDP_TILEMAP_BYTES
    jp showcase_vdp_clear_bytes

showcase_vdp_clear_bytes:
    ld a, b
    or c
    ret z
    xor a
.clear_loop
    call vdp_write_vram
    dec bc
    ld a, b
    or c
    jp nz, .clear_loop
    ret
