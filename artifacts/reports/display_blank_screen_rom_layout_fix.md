# Display Blank Screen Investigation: ROM Binary Layout Fix

## Problem

After loading `rom/showcase/build/showcase.bin`, the emulator window shows no visual output despite the previous M60 display pipeline fixes being in place. The CPU executes and the frame counter advances, but the VDP framebuffer remains entirely black.

## Investigation

### Display pipeline verified working

The SDL display pipeline (added in M60) is correct:

| Component | Status | Evidence |
|---|---|---|
| `Scheduler::step_scanline()` calls `vdp_.render_frame()` | Working | Called at frame boundary (scanline 262) |
| `render_frontend_frame()` reads framebuffer | Working | Calls `core.framebuffer_snapshot()` each frame |
| `Frontend::update_framebuffer()` uploads to SDL | Working | Converts RGBA to byte buffer, calls `SDL_UpdateTexture` |
| `Frontend::render_framebuffer()` draws texture | Working | `SDL_RenderCopy` with integer-scaled destination rect |
| SDL texture format (`SDL_PIXELFORMAT_RGBA32`) | Correct | Byte layout R,G,B,A matches upload buffer ordering |
| VDP port routing through bus | Working | Ports `$E0`-`$ED` correctly routed to `vdp_.read_port()`/`write_port()` |
| VDP palette write/read | Working | Direct port writes and CPU OUT instructions both function correctly |
| VDP VRAM write via control+data ports | Working | Address latch and auto-increment verified |

### Framebuffer diagnostic revealed the real issue

A diagnostic program loaded the showcase ROM, ran 100 frames, and checked VDP state:

```
Palette (all 8 entries): 0x00000000
VRAM pattern bank 0 non-zero bytes: 0
BG tilemap non-zero bytes: 0
FG tilemap non-zero bytes: 0
Framebuffer non-zero RGBA pixels: 0
```

After 100 frames of execution, the VDP palette was never programmed, VRAM was never written, and the framebuffer was entirely palette index 0 (black). The ROM was executing (PC advanced each frame, RAM CRC was non-trivial), but no VDP initialization was happening.

### CPU execution trace exposed the fault

Tracing the first 50 scanlines of execution:

```
Scanline  0: PC=$0000  (reset vector: JP $003B)
Scanline  1: PC=$004F  (runtime init: writing VDP/audio port defaults)
Scanline  2: PC=$0063  (runtime init: finishing audio port writes)
Scanline  3: PC=$0105  (NOP-land: executing zeros)
Scanline  4: PC=$010F  (NOP-land: PC advances by 10 each scanline)
Scanline  5: PC=$0119  ...
```

At scanline 2, the CPU finishes `sz_runtime_init` (RET at `$0068`), returns to the runtime reset path, and executes `JP $0100` (`sdk_main`). But memory address `$0100` contains all zeros (NOP instructions). The CPU slides through NOPs indefinitely, never reaching the showcase initialization code.

## Root Cause

The ROM binary had a broken memory layout caused by sjasmplus `ORG` behavior in `--raw` output mode.

### Assembly structure

```
; reset.asm
ORG $0000                          ; Reset vector at $0000
  jp sz_runtime_reset
  defs SZ_IRQ_VECTOR - $, $00      ; Explicit padding to $0038
ORG $0038                          ; IRQ vector at $0038
  jp sz_runtime_irq_handler

; runtime_init.asm (continues sequentially)
sz_runtime_reset:                  ; At $003B
  di / ld sp, $FFFE / call sz_runtime_init / jp sdk_main
sz_runtime_init:                   ; At $0045
  ...port writes...
  ret                              ; At $0068
sz_runtime_irq_handler:            ; At $0069
  ...irq ack...
  reti                             ; At $0070
  ASSERT $ <= SZ_RUNTIME_MAIN_ORG  ; $ = $0072, passes

; main.asm
ORG SZ_RUNTIME_MAIN_ORG            ; ORG $0100  <-- THE BUG
sdk_main:
  call showcase_init
  jp showcase_main_loop
```

### What happened

The `ORG $0100` directive changed the assembler's address counter to `$0100`, so all labels (`sdk_main`, `showcase_init`, `showcase_main_loop`, etc.) were assigned addresses starting from `$0100`. The runtime's `JP sdk_main` correctly encoded as `JP $0100`.

However, in sjasmplus `--raw` output mode, `ORG` does **not** pad the output file to the target address. The binary bytes were emitted sequentially:

| File offset | Logical address | Content |
|---|---|---|
| `$0000` | `$0000` | Reset vector (`JP $003B`) |
| `$0038` | `$0038` | IRQ vector (padded by explicit `defs`) |
| `$003B` | `$003B` | Runtime code |
| `$0072` | `$0100` | `sdk_main` code (CALL $0106, JP $2324) |
| `$0078` | `$0106` | `showcase_init` body |

The `sdk_main` code was physically at file offset `$0072` but labeled as address `$0100`. When the emulator loaded the ROM into memory starting at address `$0000`, the code at file offset `$0072` mapped to memory address `$0072`, not `$0100`. Memory address `$0100` contained zeros.

The `JP $0100` instruction in the runtime jumped to memory address `$0100` (zeros/NOPs), completely bypassing the ROM's initialization code.

### Why the explicit padding at $0038 worked

The reset vector area used `defs SZ_IRQ_VECTOR - $, $00` to explicitly emit zero-fill bytes between the reset vector and the IRQ vector. This created real padding in the binary output, ensuring the IRQ vector was at the correct physical offset. No equivalent padding existed for the `$0072`-to-`$0100` gap.

## Fix

Replaced the bare `ORG` directive with explicit `DEFS` padding in both ROM sources:

**`rom/showcase/src/main.asm`:**
```diff
-    ORG SZ_RUNTIME_MAIN_ORG
+    DEFS SZ_RUNTIME_MAIN_ORG - $, $00
```

**`rom/starter/src/main.asm`:**
```diff
-    ORG SZ_RUNTIME_MAIN_ORG
+    DEFS SZ_RUNTIME_MAIN_ORG - $, $00
```

`DEFS N, V` emits N bytes of value V. `DEFS SZ_RUNTIME_MAIN_ORG - $, $00` fills the gap from the current address (`$0072`) to `$0100` with zeros, advancing both the address counter and the file offset. The `sdk_main` code then physically resides at file offset `$0100`, matching its logical address.

This is the same technique already used in `reset.asm` for the IRQ vector padding.

## Verification

After the fix:

**ROM binary layout confirmed:**
```
$0072-$00FF: Zero padding (142 bytes)
$0100: CD 06 01 C3 24 23 ...  (sdk_main: CALL $0106, JP $2324)
$0106: CD 5C 1C ...            (showcase_init body)
```

**Framebuffer diagnostic after fix:**
```
Palette[0] = 0x080C20FF  (R=8, G=12, B=32, A=255)
Palette[1] = 0xE8F0FFFF  (R=232, G=240, B=255, A=255)
Palette[2] = 0xFFC040FF  (R=255, G=192, B=64, A=255)
...
Non-zero RGBA pixels: 5300
Palette index distribution: 37 distinct indices used
```

**All 39 emulator tests pass. Headless execution produces valid CRCs.**

## Files Changed

| File | Change |
|---|---|
| `rom/showcase/src/main.asm` | `ORG` → `DEFS` padding |
| `rom/starter/src/main.asm` | `ORG` → `DEFS` padding |
| `rom/showcase/build/showcase.bin` | Rebuilt with correct layout |
