# Showcase State Snapshot

## Current Milestone
M36

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

## Parallax Scrolling Demo
- The Showcase ROM now owns separate background and foreground tilemap buffers in RAM plus explicit `background_scroll_x`, `background_scroll_y`, `foreground_scroll_x`, and `foreground_scroll_y` state bytes.
- Scene composition is split across both planes: the background keeps the splash/logo composition and repeated pattern bands, while the foreground uses transparent text banners and labels so the two layers remain easy to distinguish.
- `showcase_upload_scene_to_vram` now uploads both tilemaps in sequence to the existing background and foreground VDP tilemap regions instead of leaving the foreground plane blank.
- The main loop advances the background by `+1/+1` per frame and the foreground by `+2/+0` per frame, then applies both planes' scroll registers during `showcase_render`.

## Basic Sprite Rendering
- The Showcase ROM now includes a local `8x8` sprite tile asset plus deterministic `sprite_x` and `sprite_y` RAM state initialized during boot.
- `showcase_upload_scene_to_vram` now uploads that sprite tile into pattern memory, and `showcase_render` rewrites SAT entry `0` each frame with the current Y/X/tile/flags bytes.
- The sprite uses high priority so it remains visible above the moving foreground text while the background and foreground parallax motion continue unchanged.
- `showcase_update` now advances the sprite horizontally by `+1` pixel per frame, producing visible screen-space motion independent from the layer scroll registers.
- The Showcase VDP helper now loads the VRAM pointer through the control port, fixing the prior ROM-side defect that prevented deterministic SAT updates after boot.

## Result
M36 turns the Showcase ROM into a sprite-capable reference scene. The ROM now demonstrates a moving high-priority sprite on top of the validated parallax background, using deterministic SAT updates and a minimal real VRAM pointer-load contract.

## Recommendation
Proceed to `M37 - Sprite Animation`. The next step is adding animation frame switching on top of the now-validated single-sprite SAT and motion path.
