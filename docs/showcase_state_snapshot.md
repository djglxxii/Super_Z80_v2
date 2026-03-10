# Showcase State Snapshot

## Current Milestone
M43

## Status
`M42 - Integrated Showcase Scene` is complete. The official next Showcase milestone is `M43 - Developer Reference ROM Finalization`.

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

## Sprite Animation
- The Showcase sprite asset now contains two local `8x8` frames stored in adjacent tile slots, keeping the frame-to-tile relationship explicit inside the ROM source.
- The ROM now owns deterministic `sprite_frame` and `sprite_anim_counter` bytes in RAM, both initialized during boot and advanced only from the main loop update step.
- `showcase_update` increments the animation counter every frame, toggles `sprite_frame` on a fixed `8`-frame cadence, and preserves the existing `+1` horizontal sprite motion from M36.
- `showcase_render_sprite` now rewrites SAT entry `0` with the current Y/X position and `showcase_demo_sprite_tile_base + sprite_frame`, reusing the validated M36 SAT update path without changing layer composition.
- Repeated `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 16` runs remain byte-identical with `HEADLESS_ROM_RESULT rom_crc32=0xCAEAE31F ram_crc32=0x9A494230 audio_crc32=0xD8F49994`.

## Metasprite Example
- The Showcase ROM now stores a single logical metasprite origin in deterministic `meta_x` and `meta_y` RAM bytes, keeping all hardware-sprite positioning derived from one shared base coordinate.
- `rom/showcase/assets/demo_sprite.asm` now contains two `2x2` animation frames laid out as four `8x8` tiles per frame, ordered explicitly for top-left, top-right, bottom-left, and bottom-right SAT composition.
- `showcase_render_sprite` now rewrites SAT entries `0` through `3` every frame, deriving each component sprite's X/Y position from the shared origin plus constant `8`-pixel offsets and selecting the correct tile block from the current animation frame.
- The existing M37 frame toggle cadence remains intact, but tile selection now advances the whole `2x2` metasprite in sync while the parallax scene and `+1` horizontal motion continue unchanged.
- Repeated `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 20` runs remain byte-identical with `HEADLESS_ROM_RESULT rom_crc32=0xBBF8B4A9 ram_crc32=0x9A494230 audio_crc32=0xD8F49994`.

## M38a - Controller Interaction Enhancement
- The main loop's existing per-frame `showcase_poll_input` step now directly drives Showcase movement through the stored active-low `SHOWCASE_PAD1_STATE` byte, keeping the input integration explicit and ROM-local.
- `showcase_update` now applies `UP`, `DOWN`, `LEFT`, and `RIGHT` independently to `meta_y` and `meta_x`, moving the logical metasprite origin by exactly `1` pixel per pressed direction each frame and allowing deterministic diagonal motion.
- The parallax layer updates and the fixed `8`-frame metasprite animation cadence remain unchanged, so movement is now controller-driven without altering scene composition or animation timing.
- Headless execution still sees idle controller state (`0xFF`), so repeated `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 24` runs remain byte-identical with `HEADLESS_ROM_RESULT rom_crc32=0xA48747B5 ram_crc32=0x9A494230 audio_crc32=0xD8F49994`.
- This work is preserved as `M38a - Controller Interaction Enhancement` and is not the official `M39` milestone.

## M39 - PSG Sound Effects
- `showcase_init` now includes an explicit ROM-local PSG setup step that initializes channel periods to zero, silences all PSG outputs, enables the PSG control register, and seeds deterministic `SHOWCASE_PAD1_PREV_STATE` and `SHOWCASE_SFX_TIMER` RAM bytes.
- `showcase_update` now runs a narrow sound-effect path before the existing parallax, movement, and animation work, checking `PAD1` button `A` on an active-low press edge so the tone starts once per press instead of restarting every frame while held.
- The sound effect uses direct PSG register writes on tone channel A only: period `$0060`, volume `$02`, and a fixed `10`-frame duration before the ROM restores channel A to `SZ_AUD_VOL_SILENT`.
- Headless execution with idle input still produces identical repeated output because no trigger edge occurs: `HEADLESS_ROM_RESULT rom_crc32=0xCDD4CAD9 ram_crc32=0x9A494230 audio_crc32=0xD8F49994`.

## M40 - YM2151 Music Playback
- `showcase_init_audio_state` now initializes deterministic FM playback state alongside the existing PSG setup, then calls a ROM-local YM2151 setup routine that disables timers, keys off every channel, and programs a minimal channel `0` voice through explicit register writes only.
- The Showcase ROM now keeps `SHOWCASE_MUSIC_NOTE_INDEX` and `SHOWCASE_MUSIC_NOTE_TIMER` in RAM and advances them once per frame in `showcase_update_music`, preserving the existing parallax scene, controller movement, sprite animation, and PSG trigger path.
- A small four-note YM2151 pattern loops continuously by rewriting channel `0` frequency/block registers and retriggering all four operators every `16` frames, providing clear continuous FM playback without introducing a song engine.
- Repeated `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 32` runs remain byte-identical with `HEADLESS_ROM_RESULT rom_crc32=0xFCEDF42B ram_crc32=0x9A494230 audio_crc32=0xD8F49994`.

## M41 - Audio Mixing Demonstration
- The Showcase ROM's frame update now demonstrably preserves independent PSG and YM state progression: `showcase_update_sfx` handles PAD1 `A` edge-triggered tone playback while `showcase_update_music` continues advancing the four-note YM2151 loop in the same deterministic frame step.
- The ROM-local PSG writes remain confined to the PSG ports and timer byte, while YM2151 note updates remain confined to `0x70-0x71` register writes and the two music state bytes, so neither path resets or silences the other.
- Repeated `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 32` runs remain byte-identical with `HEADLESS_ROM_RESULT rom_crc32=0xFCEDF42B ram_crc32=0x9A494230 audio_crc32=0xD8F49994`.

## M42 - Integrated Showcase Scene
- The Showcase ROM now presents the already integrated parallax, controller-driven metasprite, YM2151 loop, and PSG sound-effect paths as one coherent scene through an explicit foreground text overlay instead of milestone-specific labels.
- The on-screen overlay now identifies the ROM as the platform showcase, documents `D-PAD` movement and `A` for sound effects, and states that YM and PSG playback are active together while the scene continues to use the validated deterministic frame order: input, SFX, music, scroll, sprite animation, and render.
- The foreground banner text now reinforces the integrated scene purpose by calling out metasprite, FM music, PSG sound effects, and parallax behavior without introducing any new hardware or runtime abstraction.
- Repeated `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 48` runs remain byte-identical with `HEADLESS_ROM_RESULT rom_crc32=0xD7F53636 ram_crc32=0x9A494230 audio_crc32=0xD8F49994`.

## Result
The Showcase ROM now runs as a complete interactive platform demo: parallax scrolling stays active, the metasprite remains controller-driven and animated, YM2151 music loops continuously, PSG sound effects trigger on `PAD1 A` without resetting music, and the foreground text overlay documents the controls and integrated audio scene. `M42 - Integrated Showcase Scene` is complete, and the next official Showcase milestone is `M43 - Developer Reference ROM Finalization`.

## Recommendation
Proceed to `M43 - Developer Reference ROM Finalization` per `docs/plan.md`.
