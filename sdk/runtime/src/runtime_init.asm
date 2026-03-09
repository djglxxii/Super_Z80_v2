; Minimal deterministic startup path.

sz_runtime_reset:
 di
 ld sp, SZ_STACK_TOP
 call sz_runtime_init
 jp sdk_main

sz_runtime_init:
 xor a
 out (SZ_PORT_IRQ_ENABLE), a

 ld a, $FF
 out (SZ_PORT_IRQ_ACK), a

 xor a
 out (SZ_PORT_VDP_PALETTE_INDEX), a
 out (SZ_PORT_VDP_BG_SCROLL_X), a
 out (SZ_PORT_VDP_BG_SCROLL_Y), a
 out (SZ_PORT_VDP_FG_SCROLL_X), a
 out (SZ_PORT_VDP_FG_SCROLL_Y), a
 out (SZ_PORT_VDP_BG_PATTERN_BANK), a
 out (SZ_PORT_VDP_FG_PATTERN_BANK), a

 ld a, SZ_AUD_VOL_SILENT
 out (SZ_PORT_AUD_VOL_A), a
 out (SZ_PORT_AUD_VOL_B), a
 out (SZ_PORT_AUD_VOL_C), a
 out (SZ_PORT_AUD_VOL_N), a

 xor a
 out (SZ_PORT_AUD_CTRL), a
 ret

sz_runtime_irq_handler:
 push af
 ld a, SZ_IRQ_VBLANK_BIT
 out (SZ_PORT_IRQ_ACK), a
 pop af
 ei
 reti

 ASSERT $ <= SZ_RUNTIME_MAIN_ORG
