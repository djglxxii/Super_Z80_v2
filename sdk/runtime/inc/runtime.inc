; Super_Z80 minimal runtime entry surface.
; Contract:
; - ROM code includes this file once.
; - ROM code defines `sdk_main` at `SZ_RUNTIME_MAIN_ORG` or later.

 INCLUDE "../src/reset.asm"
 INCLUDE "../src/runtime_init.asm"
