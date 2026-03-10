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

    ld a, (SHOWCASE_PAD1_STATE)
    bit 2, a
    jr nz, .check_right
    ld hl, SHOWCASE_META_X
    dec (hl)

.check_right
    ld a, (SHOWCASE_PAD1_STATE)
    bit 3, a
    jr nz, .check_up
    ld hl, SHOWCASE_META_X
    inc (hl)

.check_up
    ld a, (SHOWCASE_PAD1_STATE)
    bit 0, a
    jr nz, .check_down
    ld hl, SHOWCASE_META_Y
    dec (hl)

.check_down
    ld a, (SHOWCASE_PAD1_STATE)
    bit 1, a
    jr nz, .update_animation
    ld hl, SHOWCASE_META_Y
    inc (hl)

.update_animation
    ld a, (SHOWCASE_SPRITE_ANIM_COUNTER)
    inc a
    cp SHOWCASE_METASPRITE_ANIM_PERIOD
    jr c, .store_anim_counter

    xor a
    ld (SHOWCASE_SPRITE_ANIM_COUNTER), a
    ld a, (SHOWCASE_SPRITE_FRAME)
    xor 1
    ld (SHOWCASE_SPRITE_FRAME), a
    ret

.store_anim_counter
    ld (SHOWCASE_SPRITE_ANIM_COUNTER), a
    ret

showcase_render:
    call showcase_apply_scroll_registers
    call showcase_render_sprite
    ret
