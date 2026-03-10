; Super_Z80 Showcase ROM entry point.
;
; The runtime jumps to sdk_main after reset and basic hardware setup. This
; file is intentionally small so developers can see the high-level ROM layout:
; boot, initialize the scene, then run one deterministic frame loop forever.

    INCLUDE "inc/super_z80.inc"
    INCLUDE "runtime/inc/runtime.inc"

    INCLUDE "inc/constants.inc"
    INCLUDE "inc/memory_map.inc"
    INCLUDE "inc/vdp_helpers.inc"

    ORG SZ_RUNTIME_MAIN_ORG

sdk_main:
    call showcase_init
    jp showcase_main_loop

    INCLUDE "src/init.asm"
    INCLUDE "src/vdp_init.asm"
    INCLUDE "src/text.asm"
    INCLUDE "src/main_loop.asm"
