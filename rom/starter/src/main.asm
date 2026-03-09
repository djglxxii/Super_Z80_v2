; Minimal ROM that assembles against the checked-in SDK runtime surface.

    INCLUDE "sdk/inc/super_z80.inc"
    INCLUDE "sdk/runtime/inc/runtime.inc"

    ORG SZ_RUNTIME_MAIN_ORG

sdk_main:
    ld hl, sz_font_8x8
    ld (SZ_RAM_BASE + 0), hl

    ld hl, sz_splash_tiles
    ld (SZ_RAM_BASE + 2), hl

    ld hl, sz_splash_tilemap
    ld (SZ_RAM_BASE + 4), hl

starter_main_loop:
    SZ_INPUT_READ_PAD1 a
    ld (SZ_RAM_BASE + 6), a

    SZ_INPUT_READ_PAD1_SYS a
    ld (SZ_RAM_BASE + 7), a

    jp starter_main_loop

    INCLUDE "sdk/assets/font_8x8.asm"
    INCLUDE "sdk/assets/splash_tiles.asm"
    INCLUDE "sdk/assets/splash_tilemap.asm"
