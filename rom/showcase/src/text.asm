showcase_splash_tilemap:
    INCLUDE "assets/splash_tilemap.asm"
showcase_splash_tilemap_end:
showcase_splash_tilemap_bytes EQU showcase_splash_tilemap_end - showcase_splash_tilemap

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
    ld de, SHOWCASE_BG_BUFFER + SHOWCASE_TEXT_ROW0_OFFSET
    call showcase_write_ascii_line

    ld hl, showcase_status_text
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

showcase_title_text:
    db "SUPER Z80", 0

showcase_status_text:
    db "SHOWCASE SCAFFOLD", 0
