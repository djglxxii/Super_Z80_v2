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
    ret

showcase_render:
    ret
