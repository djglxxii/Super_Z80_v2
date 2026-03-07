# Super_Z80_v2 State Snapshot

## Current Milestone
M23

## Recent Changes
- VDP baseline documentation added in `docs/vdp_baseline_v1.md` as the authoritative graphics reference for ROM developers.
- Graphics behavior contract frozen at milestone M23 with no new hardware features added.
- VDP behavior is now considered stable and must not change without a bug fix, ROM-driven requirement, or explicit hardware contract revision.
- Hardware sprite collision detection now latches when two visible sprites render non-zero pixels on the same screen coordinate.
- VDP status bit `3` now reports sprite collision and clears on status read, matching the existing latched-status behavior.
- Sprite collision detection uses a sprite-only per-frame mask during rendering, so BG/FG pixels never trigger collisions and detection remains deterministic across priority passes.
- Sprite collision unit tests added and passing for no-collision, overlap detection, transparent overlap, clear-on-read semantics, and mixed-priority overlap.
- Background and foreground planes now use dedicated pattern bank registers on VDP ports `0xEC` and `0xED`.
- Tile plane pattern fetch now resolves bank-aware addresses with bank `0` at `0x0000` and bank `1` at `0x2000`.
- Bus VDP port routing now forwards `0xE0-0xED`.
- Tile pattern bank unit tests added and passing for BG bank `0`, BG bank `1`, FG bank selection, render changes across bank switches, and bus register routing.
- Sprite SAT flags now use bit `3` as a deterministic sprite pattern bank select, with bit `4-7` still reserved.
- Sprite pattern fetch now resolves bank-aware addresses with bank `0` at `0x0000` and bank `1` at `0x2000`.
- Sprite pattern bank unit tests added and passing for bank `0`, bank `1`, same-index cross-bank lookup, transparency preservation, and preserved priority/flip behavior.
- Sprite attribute decoding now treats SAT flags bit `1` as horizontal flip, bit `2` as vertical flip, bit `3` as sprite pattern bank select, and bits `4-7` as reserved for future expansion.
- Sprite renderer now mirrors sprite pattern sampling horizontally, vertically, or both before applying the existing transparency rules.
- Sprite flipping unit tests added and passing for horizontal flip, vertical flip, combined flip, background isolation, and preserved priority behavior.
- Scanline sprite evaluation added to the VDP for deterministic per-scanline SAT selection.
- Hardware sprite limits now enforce a maximum of 16 sprites per scanline, keeping the first 16 visible entries in SAT order.
- VDP status bit `2` now latches sprite overflow and clears on status read.
- Sprite rendering now evaluates SAT per scanline for both sprite priority passes instead of scanning the full SAT during each pass.
- Sprite scanline limit unit tests added and passing for under-limit rendering, limit clipping, overflow status behavior, and SAT-order selection.
- Sprite priority flag implemented in SAT flags bit `0`.
- VDP sprite composition now renders in two deterministic passes: `BG -> sprites(priority=1) -> FG -> sprites(priority=0)`.
- Sprite ordering remains stable by SAT order, with later entries overwriting earlier ones within the same pass.
- Sprite priority unit tests added and passing for FG occlusion, BG visibility, stable SAT overwrite order, and mixed-priority composition.
- Foreground plane implemented as a second tile layer using tilemap base `0x5000`.
- VDP now renders `BG -> FG -> sprites`, with foreground pixel value `0` treated as transparent.
- Foreground scroll registers added on ports `0xEA-0xEB`, with independent read/write/reset behavior from the background scroll registers.
- Bus VDP port routing now forwards `0xE0-0xEB`.
- Foreground plane unit tests added and passing for overwrite, transparency, placement, FG scrolling, independent BG/FG scroll sampling, and bus register read/write behavior.
- Background scrolling registers implemented for the VDP on ports `0xE8-0xE9`.
- Background rendering now samples framebuffer pixels from scrolled world positions with wrapped `32 x 24` tilemap addressing.
- Scroll unit tests added and passing for zero scroll, horizontal and vertical shifts, partial pixel offsets, wraparound, and register read/write behavior.
- Palette and color system implemented for the VDP.
- VDP now exposes palette register ports on `0xE3-0xE7` for palette index and RGBA component access.
- Indexed framebuffer pixels now resolve deterministically through 256 palette entries packed as `0xRRGGBBAA`.
- Palette unit tests added and passing for register reads/writes, overwrite behavior, color resolution, and reset defaults.
- Sprite renderer skeleton added to the VDP framebuffer pipeline.
- VDP now reads sprite attributes from `0x6000` and sprite patterns from the shared pattern table at `0x0000`.
- Sprite rendering now runs after background tiles, with non-zero sprite pixels overwriting the framebuffer.
- Sprite renderer unit tests added and passing for single-sprite rendering, transparency behavior, positioning, and multiple sprites.
- CRC32 utility module added.
- First unit test executable added for deterministic known-answer checks.
- CMake test wiring added for CRC32 validation.
- Deterministic 64KB memory subsystem added with reset, read, and write primitives.
- Memory unit tests added and passing.
- Bus layer added with deterministic ROM, open-bus, and RAM routing behavior.
- Initial CPU-visible memory map implemented: ROM at `0x0000-0x7FFF`, open bus at `0x8000-0xBFFF`, RAM at `0xC000-0xFFFF`.
- Bus unit tests added and passing.
- Z80ex vendored into `emulator/vendor/z80ex/` for deterministic local builds.
- CPU wrapper added with Bus-backed Z80ex memory callbacks, reset, and single-instruction stepping.
- CPU unit test added for `LD A,0x42` followed by `HALT`.
- Deterministic I/O register subsystem added for port space `0x00-0xFF` with reset, read, and write behavior.
- Bus port routing added through `read_port()` and `write_port()`.
- CPU wrapper now connects Z80ex `IN` and `OUT` callbacks to the Bus I/O subsystem.
- I/O unit test added and CPU test expanded to validate port I/O.
- Scheduler subsystem added with deterministic frame and scanline counters.
- Scheduler advances CPU execution by a fixed placeholder instruction count per scanline.
- Scheduler unit tests added for reset, scanline increment, frame wrap, and CPU stepping side effects.
- IRQ controller infrastructure added with status, enable, and acknowledge behavior.
- Bus port routing now exposes IRQ registers at `0xF0-0xF2`.
- CPU interrupt line wiring now follows the bus IRQ controller state.
- VBlank timing added with scanline-driven begin/end transitions.
- Bus port `0xF3` now exposes VBlank active state through the VDP status register.
- Scheduler now generates the first hardware timing event by driving VBlank scanline updates.
- VBlank begin requests IRQ bit 0 through the IRQ controller.
- DMA engine subsystem added with source, destination, length, and control registers on ports `0xF4-0xF9`.
- Bus port routing now exposes DMA registers and the DMA busy flag.
- Scheduler now advances DMA by one byte per scanline for deterministic transfer timing.
- DMA unit tests added and passing for register behavior, transfer progression, and scheduler integration.
- VDP skeleton implemented with deterministic 64KB VRAM storage and an internal `256 x 192` framebuffer allocation.
- Bus port routing now exposes VDP status, control, and data registers on `0xE0-0xE2`.
- Scheduler scanline stepping now drives the VDP scanline callback for VBlank status and frame-ready timing.
- VDP unit tests added and passing for VRAM access, pointer incrementing, reset behavior, and frame-ready status handling.
- Background tile renderer implemented.
- Framebuffer now populated from VRAM tile and tilemap data using the temporary M12 format.
- Tile renderer unit tests added and passing for single-tile output, placement, zeroed defaults, and row stride correctness.

## Known Issues
None yet.

## Verification Status
M23 documentation verification passing.

## Next Step
Await the next explicit task packet before changing VDP scope.
