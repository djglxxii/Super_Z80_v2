# Showcase Programmer's Guide

## Purpose

`rom/showcase/` is the canonical reference ROM for Super_Z80 developers. It is
not a game engine. It is a small, deterministic example that shows how one ROM
can initialize the platform, compose a scene, read controller input, update
audio, and render a moving metasprite over scrolling background and foreground
layers.

## Location And Build

- Source tree: `rom/showcase/`
- Build command: `make -C rom/showcase`
- Output ROM: `rom/showcase/build/showcase.bin`

## ROM Structure Overview

- `rom/showcase/src/main.asm`: ROM entry point. Includes the other source files and defines `sdk_main`.
- `rom/showcase/src/init.asm`: top-level startup sequencing through `showcase_init`.
- `rom/showcase/src/vdp_init.asm`: VDP register setup, palette initialization, VRAM upload helpers, sprite rendering, and audio initialization.
- `rom/showcase/src/text.asm`: CPU-side tilemap composition and ASCII-to-tile text helpers for the background and foreground planes.
- `rom/showcase/src/main_loop.asm`: deterministic frame loop, controller polling, parallax updates, metasprite movement, PSG sound effects, and YM2151 music stepping.
- `rom/showcase/inc/constants.inc`: scene constants such as positions, animation timing, and audio constants.
- `rom/showcase/inc/memory_map.inc`: ROM-owned RAM layout for tilemap buffers, input state, scroll state, audio timers, and metasprite state.
- `rom/showcase/assets/`: checked-in tile and sprite assets used by the ROM.

## Boot And Initialization

The ROM boots through the SDK runtime and enters `sdk_main` in [main.asm](/home/djglxxii/src/Super_Z80_v2/rom/showcase/src/main.asm). `sdk_main` calls `showcase_init`, then jumps into the permanent frame loop.

`showcase_init` in [init.asm](/home/djglxxii/src/Super_Z80_v2/rom/showcase/src/init.asm) is the startup reference:

1. `showcase_vdp_init_registers`
2. `showcase_reset_scroll_state`
3. `showcase_reset_sprite_state`
4. `showcase_init_audio_state`
5. `showcase_init_palette`
6. `showcase_compose_parallax_scene`
7. `showcase_upload_scene_to_vram`

That order is intentional. It gives the ROM a fully prepared scene before the first frame update runs.

## Text Rendering Example

Text composition lives in [text.asm](/home/djglxxii/src/Super_Z80_v2/rom/showcase/src/text.asm).

- `showcase_text_write_line_at` clears a tilemap row and writes a zero-terminated ASCII string.
- `showcase_text_write_string_at` writes a multi-line string without hidden cursor state.
- `showcase_text_map_ascii_to_tile` maps printable ASCII `$20-$7F` onto the checked-in `8x8` font tiles.
- `showcase_bg_text_write_*` and `showcase_fg_text_write_*` select the background or foreground tilemap buffer before calling the shared writer.

The composed strings used in the final scene are declared at the bottom of [text.asm](/home/djglxxii/src/Super_Z80_v2/rom/showcase/src/text.asm).

## Background And Scrolling Example

The ROM keeps two CPU-side tilemap buffers in RAM:

- `SHOWCASE_BG_BUFFER`
- `SHOWCASE_FG_BUFFER`

These are defined in [memory_map.inc](/home/djglxxii/src/Super_Z80_v2/rom/showcase/inc/memory_map.inc). The initial scene is composed into those buffers in `showcase_compose_parallax_scene`, then uploaded once during boot by `showcase_upload_scene_to_vram` in [vdp_init.asm](/home/djglxxii/src/Super_Z80_v2/rom/showcase/src/vdp_init.asm).

Per-frame scrolling happens in `showcase_update` and `showcase_apply_scroll_registers`:

- `SHOWCASE_BACKGROUND_SCROLL_X` advances by `+1`
- `SHOWCASE_BACKGROUND_SCROLL_Y` advances by `+1`
- `SHOWCASE_FOREGROUND_SCROLL_X` advances by `+2`
- `SHOWCASE_FOREGROUND_SCROLL_Y` stays at `0`

This is the reference example for fixed-speed layer scrolling.

## Parallax Example

Parallax is created by composing different content into the two tilemap buffers and then moving them at different rates.

- Background content: splash tilemap plus scrolling pattern bands and a background label in [text.asm](/home/djglxxii/src/Super_Z80_v2/rom/showcase/src/text.asm)
- Foreground content: title, status text, and overlay banners in [text.asm](/home/djglxxii/src/Super_Z80_v2/rom/showcase/src/text.asm)
- Register writes: `showcase_apply_scroll_registers` in [vdp_init.asm](/home/djglxxii/src/Super_Z80_v2/rom/showcase/src/vdp_init.asm)

Developers can treat this as the baseline example for "compose once, scroll every frame."

## Sprite And Metasprite Example

The metasprite logic is split between RAM state and SAT emission.

- Logical position: `SHOWCASE_META_X` and `SHOWCASE_META_Y` in [memory_map.inc](/home/djglxxii/src/Super_Z80_v2/rom/showcase/inc/memory_map.inc)
- Animation state: `SHOWCASE_SPRITE_FRAME` and `SHOWCASE_SPRITE_ANIM_COUNTER` in [memory_map.inc](/home/djglxxii/src/Super_Z80_v2/rom/showcase/inc/memory_map.inc)
- Tile asset: `rom/showcase/assets/demo_sprite.asm`
- SAT writer: `showcase_render_sprite` in [vdp_init.asm](/home/djglxxii/src/Super_Z80_v2/rom/showcase/src/vdp_init.asm)

`showcase_render_sprite` expands one logical origin into four hardware sprites using fixed `8`-pixel offsets. The tile block for each frame starts at `showcase_demo_sprite_tile_base + (SHOWCASE_SPRITE_FRAME * 4)`.

This is the reference answer for "How do I move a metasprite?" Update the logical origin in RAM during `showcase_update`, then rewrite the SAT entries during `showcase_render`.

## Controller Input Example

Controller polling happens once per frame in `showcase_poll_input` inside [main_loop.asm](/home/djglxxii/src/Super_Z80_v2/rom/showcase/src/main_loop.asm).

- `SHOWCASE_PAD1_STATE` stores the current active-low PAD1 sample.
- `SHOWCASE_PAD1_SYS_STATE` stores the current PAD1 system-button sample.
- `showcase_update` checks the directional bits directly and moves `SHOWCASE_META_X` / `SHOWCASE_META_Y` by one pixel per pressed direction.

This is the reference example for deterministic input handling: sample once, store to RAM, then let update code consume the stored values.

## PSG Sound Effect Example

The PSG example is intentionally narrow and lives in [main_loop.asm](/home/djglxxii/src/Super_Z80_v2/rom/showcase/src/main_loop.asm).

- `showcase_update_sfx` checks for an `A`-button press edge using `SHOWCASE_PAD1_STATE` and `SHOWCASE_PAD1_PREV_STATE`.
- `showcase_start_sfx` writes channel A tone period and volume directly to the PSG ports.
- `showcase_stop_sfx` silences channel A after `SHOWCASE_SFX_DURATION_FRAMES`.
- The related constants are in [constants.inc](/home/djglxxii/src/Super_Z80_v2/rom/showcase/inc/constants.inc).

This is the reference answer for "How do I trigger a PSG sound effect?" Detect an input edge in the update step, program the PSG registers, and stop the effect with explicit ROM-owned timing state.

## YM2151 Music Example

YM2151 setup and stepping are also ROM-local.

- `showcase_init_music` in [main_loop.asm](/home/djglxxii/src/Super_Z80_v2/rom/showcase/src/main_loop.asm) disables timers, keys off channels, and programs a minimal channel `0` voice.
- `showcase_update_music` advances the fixed note timer once per frame.
- `showcase_start_music_note` writes the next channel `0` frequency pair from `showcase_music_pattern`.
- `showcase_ym_write` is the direct register write helper for ports `0x70-0x71`.

This is the reference answer for "How do I start YM2151 playback?" Put the chip in a known state during initialization, keep the note/timer bytes in RAM, and advance them once per frame.

## Integrated Scene Overview

The final scene combines all major Showcase behaviors without a generalized engine:

- Background and foreground layers scroll at different speeds.
- Foreground text identifies the ROM and explains the controls.
- A 2x2 metasprite animates and moves from controller input.
- PSG sound effects trigger from button `A`.
- YM2151 music loops continuously on channel `0`.
- `showcase_render` only writes scroll registers and sprite attribute bytes each frame.

The core frame contract is visible in `showcase_main_loop` in [main_loop.asm](/home/djglxxii/src/Super_Z80_v2/rom/showcase/src/main_loop.asm): wait for vblank, poll input, update ROM state, render hardware-visible state.

## Deterministic Verification

Repository verification:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Showcase ROM build:

```bash
make -C rom/showcase clean
make -C rom/showcase
```

Deterministic headless verification:

```bash
./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 48
./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 48
```

The two headless runs should report identical `HEADLESS_ROM_RESULT` CRC values.
