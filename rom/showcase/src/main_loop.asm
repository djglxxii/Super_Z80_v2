showcase_main_loop:
    call showcase_wait_vblank
    call showcase_poll_input
    call showcase_update
    call showcase_render
    jp showcase_main_loop

showcase_wait_vblank:
.wait_for_vblank_end
    in a, (SZ_PORT_VDP_STATUS)
    and SZ_VDP_STATUS_VBLANK
    jr nz, .wait_for_vblank_end
.wait_for_vblank_start
    in a, (SZ_PORT_VDP_STATUS)
    and SZ_VDP_STATUS_VBLANK
    jr z, .wait_for_vblank_start
    ret

showcase_poll_input:
    SZ_INPUT_READ_PAD1 a
    ld (SHOWCASE_PAD1_STATE), a
    SZ_INPUT_READ_PAD1_SYS a
    ld (SHOWCASE_PAD1_SYS_STATE), a
    ret

showcase_update:
    ld a, (SHOWCASE_BACKGROUND_SCROLL_X)
    inc a
    ld (SHOWCASE_BACKGROUND_SCROLL_X), a
    ld a, (SHOWCASE_BACKGROUND_SCROLL_Y)
    inc a
    ld (SHOWCASE_BACKGROUND_SCROLL_Y), a
    ld a, (SHOWCASE_FOREGROUND_SCROLL_X)
    inc a
    inc a
    ld (SHOWCASE_FOREGROUND_SCROLL_X), a
    ld a, (SHOWCASE_SPRITE_X)
    inc a
    ld (SHOWCASE_SPRITE_X), a
    ret

showcase_render:
    call showcase_apply_scroll_registers
    call showcase_render_sprite
    ret
