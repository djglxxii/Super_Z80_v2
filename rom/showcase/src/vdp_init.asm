showcase_font_8x8_first_ascii EQU $20
showcase_font_8x8_last_ascii  EQU $7F

; Tile upload order defines tile indices used later by the text and sprite code:
; font tiles first, splash tiles second, metasprite tiles last.
showcase_font_8x8:
    INCLUDE "assets/font_8x8.asm"
showcase_font_8x8_end:
showcase_font_8x8_tile_count EQU (showcase_font_8x8_end - showcase_font_8x8) / 64

showcase_splash_tiles:
    INCLUDE "assets/splash_tiles.asm"
showcase_splash_tiles_end:
showcase_splash_tile_count EQU (showcase_splash_tiles_end - showcase_splash_tiles) / 64
showcase_demo_sprite_tile_base EQU showcase_font_8x8_tile_count + showcase_splash_tile_count
showcase_demo_sprite_tiles:
    INCLUDE "assets/demo_sprite.asm"
showcase_demo_sprite_tiles_end:
showcase_demo_sprite_tile_count EQU (showcase_demo_sprite_tiles_end - showcase_demo_sprite_tiles) / 64

showcase_vdp_init_registers:
    ; Reset local VDP-facing state to the baseline used by the ROM.
    ; NOTE: Do not write a single byte to the VDP control port here.
    ; The control port uses a two-byte address latch (low then high),
    ; and a lone write leaves the latch phase offset, causing every
    ; subsequent vdp_set_address call to byte-swap and misroute.
    xor a
    out (SZ_PORT_VDP_BG_SCROLL_X), a
    out (SZ_PORT_VDP_BG_SCROLL_Y), a
    out (SZ_PORT_VDP_FG_SCROLL_X), a
    out (SZ_PORT_VDP_FG_SCROLL_Y), a
    out (SZ_PORT_VDP_BG_PATTERN_BANK), a
    out (SZ_PORT_VDP_FG_PATTERN_BANK), a
    ret

showcase_reset_scroll_state:
    xor a
    ld (SHOWCASE_BACKGROUND_SCROLL_X), a
    ld (SHOWCASE_BACKGROUND_SCROLL_Y), a
    ld (SHOWCASE_FOREGROUND_SCROLL_X), a
    ld (SHOWCASE_FOREGROUND_SCROLL_Y), a
    ret

showcase_reset_sprite_state:
    ld a, SHOWCASE_METASPRITE_START_X
    ld (SHOWCASE_META_X), a
    ld a, SHOWCASE_METASPRITE_START_Y
    ld (SHOWCASE_META_Y), a
    xor a
    ld (SHOWCASE_SPRITE_FRAME), a
    ld (SHOWCASE_SPRITE_ANIM_COUNTER), a
    ret

showcase_init_audio_state:
    ; Seed RAM bookkeeping first, silence PSG outputs, then enable the audio
    ; block and initialize the YM2151 example voice.
    ld a, SZ_PAD_IDLE
    ld (SHOWCASE_PAD1_PREV_STATE), a
    xor a
    ld (SHOWCASE_SFX_TIMER), a
    ld (SHOWCASE_MUSIC_NOTE_INDEX), a
    ld (SHOWCASE_MUSIC_NOTE_TIMER), a
    out (SZ_PORT_AUD_TONE_A_LOW), a
    out (SZ_PORT_AUD_TONE_A_HIGH), a
    out (SZ_PORT_AUD_TONE_B_LOW), a
    out (SZ_PORT_AUD_TONE_B_HIGH), a
    out (SZ_PORT_AUD_TONE_C_LOW), a
    out (SZ_PORT_AUD_TONE_C_HIGH), a
    out (SZ_PORT_AUD_NOISE_CTRL), a
    ld a, SZ_AUD_VOL_SILENT
    out (SZ_PORT_AUD_VOL_A), a
    out (SZ_PORT_AUD_VOL_B), a
    out (SZ_PORT_AUD_VOL_C), a
    out (SZ_PORT_AUD_VOL_N), a
    ld a, SZ_AUD_CTRL_ENABLE
    out (SZ_PORT_AUD_CTRL), a
    call showcase_init_music
    ret

showcase_init_palette:
    SZ_VDP_SET_PALETTE_RGBA 0, $08, $0C, $20, $FF
    SZ_VDP_SET_PALETTE_RGBA 1, $E8, $F0, $FF, $FF
    SZ_VDP_SET_PALETTE_RGBA 2, $FF, $C0, $40, $FF
    SZ_VDP_SET_PALETTE_RGBA 3, $20, $80, $E0, $FF
    SZ_VDP_SET_PALETTE_RGBA 4, $70, $24, $44, $FF
    SZ_VDP_SET_PALETTE_RGBA 5, $F8, $48, $54, $FF
    ret

showcase_upload_scene_to_vram:
    ; Upload static assets once at boot. The frame loop only rewrites scroll
    ; registers and SAT bytes, so the initial upload order is part of the ROM
    ; contract documented in the programmer guide.
    ld hl, SHOWCASE_VRAM_START
    call vdp_set_address

    ld hl, showcase_font_8x8
    ld bc, showcase_font_8x8_end - showcase_font_8x8
    call vdp_upload_block

    ld hl, showcase_splash_tiles
    ld bc, showcase_splash_tiles_end - showcase_splash_tiles
    call vdp_upload_block

    ld hl, SZ_VDP_BG_TILEMAP_BASE
    call vdp_set_address
    ld hl, SHOWCASE_BG_BUFFER
    ld bc, SZ_VDP_TILEMAP_BYTES
    call vdp_upload_block

    ld hl, SZ_VDP_FG_TILEMAP_BASE
    call vdp_set_address
    ld hl, SHOWCASE_FG_BUFFER
    ld bc, SZ_VDP_TILEMAP_BYTES
    call vdp_upload_block

    ld hl, showcase_demo_sprite_tile_base * SZ_VDP_TILE_BYTES
    call vdp_set_address
    ld hl, showcase_demo_sprite_tiles
    ld bc, showcase_demo_sprite_tiles_end - showcase_demo_sprite_tiles
    call vdp_upload_block

    ld hl, SZ_VDP_SAT_BASE
    call vdp_set_address
    ld bc, $0100
    call showcase_vdp_clear_bytes
    ret

vdp_set_address:
    ; The control port loads the VRAM pointer as low byte then high byte.
    ld (SHOWCASE_VRAM_CURSOR), hl
    ld a, l
    out (SZ_PORT_VDP_CONTROL), a
    ld a, h
    out (SZ_PORT_VDP_CONTROL), a
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

showcase_apply_scroll_registers:
    ld a, (SHOWCASE_BACKGROUND_SCROLL_X)
    out (SZ_PORT_VDP_BG_SCROLL_X), a
    ld a, (SHOWCASE_BACKGROUND_SCROLL_Y)
    out (SZ_PORT_VDP_BG_SCROLL_Y), a
    ld a, (SHOWCASE_FOREGROUND_SCROLL_X)
    out (SZ_PORT_VDP_FG_SCROLL_X), a
    ld a, (SHOWCASE_FOREGROUND_SCROLL_Y)
    out (SZ_PORT_VDP_FG_SCROLL_Y), a
    ret

showcase_render_sprite:
    ; The logical metasprite origin is stored once in RAM and expanded here into
    ; four hardware sprites with fixed 8-pixel offsets.
    ld hl, SZ_VDP_SAT_BASE
    call vdp_set_address

    ld a, (SHOWCASE_SPRITE_FRAME)
    add a, a
    add a, a
    add a, showcase_demo_sprite_tile_base
    ld d, a

    ld a, (SHOWCASE_META_X)
    ld b, a
    ld c, d
    ld a, (SHOWCASE_META_Y)
    call showcase_render_sprite_entry

    ld a, (SHOWCASE_META_X)
    add a, SHOWCASE_METASPRITE_RIGHT_X
    ld b, a
    ld a, d
    inc a
    ld c, a
    ld a, (SHOWCASE_META_Y)
    call showcase_render_sprite_entry

    ld a, (SHOWCASE_META_X)
    ld b, a
    ld a, d
    add a, 2
    ld c, a
    ld a, (SHOWCASE_META_Y)
    add a, SHOWCASE_METASPRITE_BOTTOM_Y
    call showcase_render_sprite_entry

    ld a, (SHOWCASE_META_X)
    add a, SHOWCASE_METASPRITE_RIGHT_X
    ld b, a
    ld a, d
    add a, 3
    ld c, a
    ld a, (SHOWCASE_META_Y)
    add a, SHOWCASE_METASPRITE_BOTTOM_Y
    call showcase_render_sprite_entry
    ret

showcase_render_sprite_entry:
    call vdp_write_vram
    ld a, b
    call vdp_write_vram
    ld a, c
    call vdp_write_vram
    ld a, SHOWCASE_METASPRITE_FLAGS
    call vdp_write_vram
    ret
