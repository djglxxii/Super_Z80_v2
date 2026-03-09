showcase_splash_tilemap:
    INCLUDE "assets/splash_tilemap.asm"
showcase_splash_tilemap_end:
showcase_splash_tilemap_bytes EQU showcase_splash_tilemap_end - showcase_splash_tilemap

showcase_text_ascii_first   EQU $20
showcase_text_ascii_after   EQU $80
showcase_text_newline       EQU $0A
showcase_text_blank_tile    EQU $00

showcase_prepare_bg_tilemap:
    ld hl, SHOWCASE_BG_BUFFER
    ld de, SHOWCASE_BG_BUFFER + 1
    xor a
    ld (hl), a
    ld bc, SZ_VDP_TILEMAP_BYTES - 1
    ldir

    ld hl, showcase_splash_tilemap
    ld de, SHOWCASE_BG_BUFFER
    ld bc, showcase_splash_tilemap_bytes
    ldir

    ld hl, showcase_title_text
    ld b, SHOWCASE_TITLE_TEXT_X
    ld c, SHOWCASE_TITLE_TEXT_Y
    call showcase_text_write_line_at

    ld hl, showcase_status_text
    ld b, SHOWCASE_STATUS_TEXT_X
    ld c, SHOWCASE_STATUS_TEXT_Y
    call showcase_text_write_line_at
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
    call showcase_text_get_xy_address
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
    call showcase_text_get_xy_address
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
    ld hl, SHOWCASE_BG_BUFFER
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
    db "SUPER Z80 SHOWCASE", 0

showcase_status_text:
    db "FONT AND TEXT OK", 0
