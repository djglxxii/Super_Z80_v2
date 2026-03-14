# Display Distorted Graphics Fix

## Problem

After the ROM binary layout fix (ORG gap), the emulator displayed a screen with distorted, corrupted graphics. The background color was correct (palette entry 0 visible), scrolling was active, but tile patterns were garbled and text was absent. The distortion pattern was consistent and did not change.

## Root Causes

Two independent bugs in the showcase ROM assembly code prevented correct VDP initialization.

### Bug 1: VDP control port address latch desync

**File:** `rom/showcase/src/vdp_init.asm`

The `showcase_vdp_init_registers` function wrote a single byte to the VDP control port:

```asm
xor a
out (SZ_PORT_VDP_CONTROL), a   ; <-- the bug
```

The VDP control port uses a two-byte address latch: the first write stores the low byte, the second combines both and sets `vram_ptr_`. A single write leaves the latch in the "waiting for high byte" state.

Every subsequent `vdp_set_address` call then has its bytes interpreted one phase off:

| Call | Intended address | Actual `vram_ptr_` | Result |
|---|---|---|---|
| `vdp_set_address($0000)` | `$0000` | `$0000` | Correct by coincidence |
| `vdp_set_address($4000)` | `$4000` | `$0000` | BG tilemap overwrites pattern bank 0 |
| `vdp_set_address($5000)` | `$5000` | `$0040` | FG tilemap written to wrong offset |
| `vdp_set_address($6000)` | `$6000` | `$0050` | SAT written to wrong offset |

Font tiles uploaded correctly (first call, address $0000 works by coincidence), but the BG tilemap data then overwrote the font patterns at $0000, producing the garbled tile appearance.

**Fix:** Removed the `out (SZ_PORT_VDP_CONTROL), a` instruction. The `control_reg_` field is a storage register with no effect on VDP rendering behavior, so the write was unnecessary.

### Bug 2: Text string pointer destroyed during composition

**File:** `rom/showcase/src/text.asm`

The text write functions (`showcase_bg_text_write_string_at`, etc.) called `showcase_select_bg_buffer` which overwrites HL with the buffer address. HL was supposed to hold the text string pointer:

```asm
; In compose_parallax_scene:
ld hl, showcase_bg_band_top_text   ; HL = string pointer
call showcase_bg_text_write_string_at
  ; Inside:
  call showcase_select_bg_buffer     ; HL = $C000, string pointer LOST
  jp showcase_text_write_string_at
    call showcase_text_get_xy_address  ; HL overwritten AGAIN
    .write_loop
    ld a, (hl)                         ; reads from address 0, not the string!
```

The write loop read from ROM address $0000 (the reset vector `JP $003B` = bytes `C3 3B 00`). Byte `$3B` mapped to tile `$1B` (the only non-zero tile index that appeared in the corrupted tilemap). The NUL byte at address 2 terminated the loop immediately.

**Fix:** Added `push hl` / `pop hl` around both the `showcase_select_*_buffer` calls in the wrapper functions AND the `showcase_text_get_xy_address` calls in `showcase_text_write_string_at` (including the `.newline` handler path).

## Verification

After both fixes, a 500-frame diagnostic confirms correct VDP state:

**BG tilemap ($4000):** Band text patterns (tile $0E = '.', tile $0B = '+') at rows 0, 11, 22. Splash tile data at rows 11-12. Background label text at row 20.

**FG tilemap ($5000):** Title "SUPER Z80 PLATFORM SHOWCASE" at row 2. Status text at rows 4-5. Banner text at rows 8 and 14. Foreground label at rows 20-21.

**Pattern bank 0:** 1872 non-zero bytes (font character strokes in 8bpp indexed format).

**Palette:** 6 entries correctly programmed (indices 0-5).

**Framebuffer:** Non-zero pixels using palette indices 0, 1, 2, 5.

**All 39 emulator tests pass.**

## Files Changed

| File | Change |
|---|---|
| `rom/showcase/src/vdp_init.asm` | Removed stray single-byte control port write |
| `rom/showcase/src/text.asm` | Preserved HL (string pointer) across buffer selection and address calculation calls |
| `rom/showcase/build/showcase.bin` | Rebuilt with both fixes |
