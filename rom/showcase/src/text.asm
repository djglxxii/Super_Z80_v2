showcase_splash_tile_base EQU showcase_font_8x8_tile_count

showcase_splash_tilemap:
    INCLUDE "assets/splash_tilemap.asm"
showcase_splash_tilemap_end:
showcase_splash_tilemap_bytes EQU showcase_splash_tilemap_end - showcase_splash_tilemap

showcase_text_ascii_first   EQU $20
showcase_text_ascii_after   EQU $80
showcase_text_newline       EQU $0A
showcase_text_blank_tile    EQU $00

showcase_compose_parallax_scene:
    ; Compose both planes in RAM first, then upload once during boot. Keeping
    ; composition CPU-side makes the text helpers simple and deterministic.
    call showcase_select_bg_buffer
    call showcase_text_clear_buffer
    call showcase_select_fg_buffer
    call showcase_text_clear_buffer

    ld hl, showcase_splash_tilemap
    ld de, SHOWCASE_BG_BUFFER
    ld bc, showcase_splash_tilemap_bytes
    ldir

    ld hl, showcase_bg_band_top_text
    ld b, 0
    ld c, 0
    call showcase_bg_text_write_string_at

    ld hl, showcase_bg_band_mid_text
    ld b, 0
    ld c, 11
    call showcase_bg_text_write_string_at

    ld hl, showcase_bg_band_bottom_text
    ld b, 0
    ld c, 22
    call showcase_bg_text_write_string_at

    ld hl, showcase_title_text
    ld b, SHOWCASE_TITLE_TEXT_X
    ld c, SHOWCASE_TITLE_TEXT_Y
    call showcase_fg_text_write_line_at

    ld hl, showcase_status_text
    ld b, SHOWCASE_STATUS_TEXT_X
    ld c, SHOWCASE_STATUS_TEXT_Y
    call showcase_fg_text_write_string_at

    ld hl, showcase_background_label_text
    ld b, SHOWCASE_BACKGROUND_LABEL_X
    ld c, SHOWCASE_BACKGROUND_LABEL_Y
    call showcase_bg_text_write_line_at

    ld hl, showcase_foreground_label_text
    ld b, SHOWCASE_FOREGROUND_LABEL_X
    ld c, SHOWCASE_FOREGROUND_LABEL_Y
    call showcase_fg_text_write_line_at

    ld hl, showcase_fg_banner_top_text
    ld b, 2
    ld c, 8
    call showcase_fg_text_write_line_at

    ld hl, showcase_fg_banner_bottom_text
    ld b, 1
    ld c, 14
    call showcase_fg_text_write_line_at
    ret

showcase_select_bg_buffer:
    ld hl, SHOWCASE_BG_BUFFER
    ld (SHOWCASE_TEXT_BUFFER_PTR), hl
    ret

showcase_select_fg_buffer:
    ld hl, SHOWCASE_FG_BUFFER
    ld (SHOWCASE_TEXT_BUFFER_PTR), hl
    ret

showcase_bg_text_write_line_at:
    push hl
    call showcase_select_bg_buffer
    pop hl
    jp showcase_text_write_line_at

showcase_bg_text_write_string_at:
    push hl
    call showcase_select_bg_buffer
    pop hl
    jp showcase_text_write_string_at

showcase_fg_text_write_line_at:
    push hl
    call showcase_select_fg_buffer
    pop hl
    jp showcase_text_write_line_at

showcase_fg_text_write_string_at:
    push hl
    call showcase_select_fg_buffer
    pop hl
    jp showcase_text_write_string_at

showcase_text_clear_buffer:
    ld hl, (SHOWCASE_TEXT_BUFFER_PTR)
    ld d, h
    ld e, l
    inc de
    xor a
    ld (hl), a
    ld bc, SZ_VDP_TILEMAP_BYTES - 1
    ldir
    ret

; Clear the target tilemap row, then write a zero-terminated ASCII string at
; tile coordinates B=x and C=y. Newline bytes move to the next row and restart
; from the original X position. Unsupported characters map to the blank tile.
showcase_text_write_line_at:
    push bc
    push hl
    call showcase_text_clear_line
    pop hl
    pop bc

showcase_text_write_string_at:
    ; Writes a zero-terminated string without storing hidden cursor state.
    ; HL = string pointer, B = X, C = Y.
    ; get_xy_address overwrites HL, so save/restore the string pointer.
    push hl
    call showcase_text_get_xy_address
    pop hl
    ld a, b
    push af
.write_loop
    ld a, (hl)
    or a
    jr z, .done
    cp showcase_text_newline
    jr z, .newline
    call showcase_text_map_ascii_to_tile
    ld (de), a
    inc hl
    inc de
    jr .write_loop
.newline
    inc hl
    inc c
    pop af
    ld b, a
    push af
    push hl
    call showcase_text_get_xy_address
    pop hl
    jr .write_loop
.done
    pop af
    ret

showcase_text_clear_line:
    call showcase_text_get_row_address
    ex de, hl
    ld b, SZ_VDP_TILEMAP_WIDTH
    xor a
.clear_loop
    ld (de), a
    inc de
    djnz .clear_loop
    ret

showcase_text_get_xy_address:
    call showcase_text_get_row_address
    ld d, 0
    ld e, b
    add hl, de
    ex de, hl
    ret

showcase_text_get_row_address:
    ; Advance one tilemap row at a time from the currently selected plane buffer.
    ld hl, (SHOWCASE_TEXT_BUFFER_PTR)
    ld a, c
.row_loop
    or a
    ret z
    ld de, SZ_VDP_TILEMAP_WIDTH
    add hl, de
    dec a
    jr .row_loop

showcase_text_map_ascii_to_tile:
    cp showcase_text_ascii_first
    jr c, .unsupported
    cp showcase_text_ascii_after
    jr nc, .unsupported
    sub showcase_text_ascii_first
    ret
.unsupported
    ld a, showcase_text_blank_tile
    ret

showcase_title_text:
    db "SUPER Z80 PLATFORM SHOWCASE", 0

showcase_status_text:
    db "D-PAD MOVE  A:SOUND", $0A, "YM+PSG MIX ACTIVE", 0

showcase_background_label_text:
    db "BG X1 Y1 PARALLAX", 0

showcase_foreground_label_text:
    db "FG X2 Y0 OVERLAY", 0

showcase_fg_banner_top_text:
    db "== METASPRITE + FM MUSIC ==", 0

showcase_fg_banner_bottom_text:
    db "-- PSG SFX OVER FM LOOP --", 0

showcase_bg_band_top_text:
    db ". + . + . + . + . + . + . + .", 0

showcase_bg_band_mid_text:
    db "+ . + . + . + . + . + . + . +", 0

showcase_bg_band_bottom_text:
    db ". + . + . + . + . + . + . + .", 0
