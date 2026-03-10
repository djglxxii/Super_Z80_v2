showcase_main_loop:
    ; Deterministic frame contract:
    ; 1. Wait for the next visible frame boundary.
    ; 2. Sample controller state once.
    ; 3. Advance all ROM-owned state once.
    ; 4. Emit scroll and SAT updates for the new frame.
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
    ; Audio updates run before scene motion so button edges and music timing are
    ; both derived from the same freshly-sampled controller/frame state.
    call showcase_update_sfx
    call showcase_update_music

    ; Background and foreground scroll at different speeds to make the plane
    ; separation obvious on real hardware and in headless captures.
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
    ; The metasprite toggles between two 2x2 frames on a fixed cadence.
    ld a, (SHOWCASE_SPRITE_ANIM_COUNTER)
    inc a
    cp SHOWCASE_METASPRITE_ANIM_PERIOD
    jr c, .store_anim_counter

    xor a
    ld (SHOWCASE_SPRITE_ANIM_COUNTER), a
    ld a, (SHOWCASE_SPRITE_FRAME)
    xor 1
    ld (SHOWCASE_SPRITE_FRAME), a
    jr showcase_store_pad1_prev_state

.store_anim_counter
    ld (SHOWCASE_SPRITE_ANIM_COUNTER), a
showcase_store_pad1_prev_state:
    ld a, (SHOWCASE_PAD1_STATE)
    ld (SHOWCASE_PAD1_PREV_STATE), a
    ret

showcase_update_sfx:
    ; Trigger the PSG effect only on the A-button press edge so holding the
    ; button does not continuously restart the tone.
    ld a, (SHOWCASE_SFX_TIMER)
    or a
    jr z, .check_trigger

    dec a
    ld (SHOWCASE_SFX_TIMER), a
    jr nz, .check_trigger
    call showcase_stop_sfx

.check_trigger
    ld a, (SHOWCASE_PAD1_STATE)
    and SHOWCASE_SFX_TRIGGER_MASK
    jr nz, .done

    ld a, (SHOWCASE_PAD1_PREV_STATE)
    and SHOWCASE_SFX_TRIGGER_MASK
    jr z, .done

    call showcase_start_sfx

.done
    ret

showcase_start_sfx:
    ; Use channel A only so the example remains small and isolated.
    ld a, SHOWCASE_SFX_TONE_A_PERIOD_LO
    out (SZ_PORT_AUD_TONE_A_LOW), a
    ld a, SHOWCASE_SFX_TONE_A_PERIOD_HI
    out (SZ_PORT_AUD_TONE_A_HIGH), a
    ld a, SHOWCASE_SFX_TONE_A_VOLUME
    out (SZ_PORT_AUD_VOL_A), a
    ld a, SHOWCASE_SFX_DURATION_FRAMES
    ld (SHOWCASE_SFX_TIMER), a
    ret

showcase_stop_sfx:
    ld a, SZ_AUD_VOL_SILENT
    out (SZ_PORT_AUD_VOL_A), a
    ret

showcase_init_music:
    ; Put the YM2151 in a known ROM-local state before starting channel 0.
    ld b, $14
    ld c, $30
    call showcase_ym_write

    ld b, $28
.key_off_channels
    ld c, SHOWCASE_YM_KEY_OFF
    call showcase_ym_write
    inc b
    ld a, b
    cp $30
    jr nz, .key_off_channels

    ld b, $20
    ld c, SHOWCASE_YM_CH0_ALG_FEEDBACK
    call showcase_ym_write

    ld b, $40
    ld c, SHOWCASE_YM_CH0_OPERATOR_DT_MUL
    call showcase_ym_write
    ld b, $48
    call showcase_ym_write
    ld b, $50
    call showcase_ym_write
    ld b, $58
    call showcase_ym_write

    ld b, $80
    ld c, SHOWCASE_YM_CH0_OPERATOR_ATTACK
    call showcase_ym_write
    ld b, $88
    call showcase_ym_write
    ld b, $90
    call showcase_ym_write
    ld b, $98
    call showcase_ym_write

    ld b, $E0
    ld c, SHOWCASE_YM_CH0_OPERATOR_RELEASE
    call showcase_ym_write
    ld b, $E8
    call showcase_ym_write
    ld b, $F0
    call showcase_ym_write
    ld b, $F8
    call showcase_ym_write

    xor a
    ld (SHOWCASE_MUSIC_NOTE_INDEX), a
    jp showcase_start_music_note

showcase_update_music:
    ; The music path is intentionally a tiny fixed-step loop, not a song engine.
    ld a, (SHOWCASE_MUSIC_NOTE_TIMER)
    or a
    jr z, .advance_note

    dec a
    ld (SHOWCASE_MUSIC_NOTE_TIMER), a
    ret nz

.advance_note
    ld a, (SHOWCASE_MUSIC_NOTE_INDEX)
    inc a
    cp SHOWCASE_MUSIC_NOTE_COUNT
    jr c, .store_note_index
    xor a

.store_note_index
    ld (SHOWCASE_MUSIC_NOTE_INDEX), a
    jp showcase_start_music_note

showcase_start_music_note:
    ; Each pattern entry stores low-byte then block/frequency bits for channel 0.
    ld a, (SHOWCASE_MUSIC_NOTE_INDEX)
    add a, a
    ld e, a
    ld d, 0
    ld hl, showcase_music_pattern
    add hl, de

    ld b, $30
    ld c, (hl)
    call showcase_ym_write
    inc hl
    ld b, $38
    ld c, (hl)
    call showcase_ym_write

    ld b, $28
    ld c, SHOWCASE_YM_KEY_OFF
    call showcase_ym_write
    ld c, SHOWCASE_YM_KEY_ON_ALL_OPERATORS
    call showcase_ym_write

    ld a, SHOWCASE_MUSIC_STEP_FRAMES
    ld (SHOWCASE_MUSIC_NOTE_TIMER), a
    ret

showcase_ym_write:
    ld a, b
    out (SZ_PORT_YM2151_ADDR), a
    ld a, c
    out (SZ_PORT_YM2151_DATA), a
    ret

showcase_render:
    ; Only hardware-visible state is written here. The tilemaps stay resident in
    ; VRAM after boot; per-frame rendering only updates scroll registers and SAT.
    call showcase_apply_scroll_registers
    call showcase_render_sprite
    ret

showcase_music_pattern:
    DB $AA, $0D
    DB $D0, $0D
    DB $10, $0E
    DB $40, $0E
