# Showcase State Snapshot

## Current Milestone
M32

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

## Result
M32 establishes the first intentional Showcase boot presentation. The ROM now boots into a deterministic static splash that combines local splash art with the reusable text layer, clearly demonstrates the asset upload flow, and preserves stable repeated headless execution.

## Recommendation
Proceed to `M33 - Scrolling Tilemap Demo`. The next step is adding controlled background movement on top of the now-stable splash composition path without expanding into menus or scene management yet.
