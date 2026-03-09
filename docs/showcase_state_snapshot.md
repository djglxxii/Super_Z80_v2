# Showcase State Snapshot

## Current Milestone
M34

## Status
Completed.

## SDK Runtime Surface
- `sdk/inc/` now contains real public include files for the base platform, VDP, and controller input contracts.
- `sdk/runtime/inc/runtime.inc` now injects a minimal reset path, stack setup, deterministic hardware init, and an IRQ acknowledge stub before jumping to `sdk_main`.
- `sdk/assets/` now contains deterministic checked-in font and splash assets using the platform's `8 x 8` / `64-byte` tile format.
- `rom/starter/src/main.asm` is the minimal starter ROM source proving that ROM code can assemble directly against the checked-in SDK surface.

## Showcase ROM Scaffold
- `rom/showcase/` now follows the canonical project layout with `src/`, `assets/`, `inc/`, and `build/` directories plus a local `Makefile`.
- `src/main.asm` boots through the checked-in SDK runtime, calls a dedicated `showcase_init`, and enters a persistent `showcase_main_loop`.
- Initialization is split across focused modules for VDP register setup, palette initialization, tilemap preparation, sequential VRAM upload helpers, and scaffolded text handling.
- The ROM now owns its local font, splash tiles, and splash tilemap assets under `rom/showcase/assets/` instead of referencing SDK asset files directly.
- `rom/showcase/Makefile` now emits `rom/showcase/build/showcase.bin` through `make` / `make clean` using `sjasmplus -I . -I ../../sdk`.
- The main loop now follows the expected scaffold contract: `wait_vblank`, `poll_input`, `update`, and `render`, with deterministic input polling and placeholder update/render hooks for future milestones.

## Font and Text Rendering
- `rom/showcase/src/text.asm` now provides a reusable tile-text API for the Showcase ROM with explicit tile-coordinate entry points, deterministic row clearing, and zero-terminated ASCII string writes into the background tilemap buffer.
- Printable ASCII bytes map directly onto the checked-in local `8x8` font asset, while unsupported characters map deterministically to the blank tile.
- The string writer now supports multi-line text through newline handling without introducing implicit cursor state.
- The splash scene now renders `SUPER Z80 SHOWCASE` and `SYSTEM SPLASH OK` through the reusable text path at intentional tile positions.

## System Splash Screen
- `showcase_init` now boots directly into a dedicated system splash composition step before the initial VRAM upload.
- The local splash tilemap now targets the correct splash tile block in VRAM instead of the font tile range, preserving the intended asset upload order while fixing the previous indexing mismatch.
- The splash assets now form a centered framed logo block so the ROM presents a clearer startup identity before later scene work exists.

## Scrolling Demo
- The Showcase ROM now maintains explicit `scroll_x` and `scroll_y` state bytes in RAM and resets them deterministically during initialization.
- The main loop advances both horizontal and vertical background scroll by one pixel per frame during `showcase_update`, with register writes applied during `showcase_render`.
- Background movement continues to use the existing splash tilemap and reusable text composition path, so the demo now validates both VDP background scroll axes without adding scene management or changing the asset pipeline.

## Result
M34 extends the first dynamic Showcase visual behavior into a combined X/Y scroll reference. The ROM now boots into the existing splash composition, then continuously scrolls the background horizontally and vertically through deterministic per-frame VDP register updates while preserving stable repeated headless execution.

## Recommendation
Proceed to `M35 - Parallax Scrolling Demo`. The next step is validating independent background and foreground motion on top of the now-established deterministic scroll update path.
