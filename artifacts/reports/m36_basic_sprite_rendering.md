# M36 Basic Sprite Rendering Report

## Status
Completed.

## Summary
Implemented the first Showcase ROM sprite demonstration on top of the existing M35 parallax scene. The ROM now uploads a local `8x8` sprite tile, initializes deterministic sprite position state in RAM, advances the sprite horizontally once per frame, and rewrites SAT entry `0` during render so the sprite remains visible above the scrolling foreground layer.

This task also exposed a real VDP surface defect: the prior ROM contract could only stream VRAM sequentially from reset, which made per-frame SAT updates impossible. The fix was kept narrow by giving the existing VDP control port a low-byte then high-byte pointer-load behavior, updating the Showcase helper to use it, and adding unit coverage for the new pointer semantics.

## Files Changed
- `emulator/include/vdp.h`
- `emulator/src/vdp.cpp`
- `emulator/tests/unit/test_vdp.cpp`
- `rom/showcase/Makefile`
- `rom/showcase/assets/demo_sprite.asm`
- `rom/showcase/inc/constants.inc`
- `rom/showcase/inc/memory_map.inc`
- `rom/showcase/inc/vdp_helpers.inc`
- `rom/showcase/src/init.asm`
- `rom/showcase/src/main.asm`
- `rom/showcase/src/main_loop.asm`
- `rom/showcase/src/text.asm`
- `rom/showcase/src/vdp_init.asm`
- `docs/programming_graphics.md`
- `docs/showcase_state_snapshot.md`
- `docs/state_snapshot.md`

## Commands Run
- `cmake -S . -B build`
- `cmake --build build`
- `ctest --test-dir build --output-on-failure`
- `make -C rom/showcase clean`
- `make -C rom/showcase`
- `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 12`
- `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 12`

## Verification Results
- `cmake -S . -B build`: passed
- `cmake --build build`: passed
- `ctest --test-dir build --output-on-failure`: passed (`38/38` tests)
- `make -C rom/showcase clean`: passed
- `make -C rom/showcase`: passed
- Headless run 1: `HEADLESS_ROM_RESULT rom_crc32=0xDBDAA92D ram_crc32=0x9A494230 audio_crc32=0xD8F49994 frames=12 frame_counter=12 scanline_counter=0 audio_samples=4096`
- Headless run 2: `HEADLESS_ROM_RESULT rom_crc32=0xDBDAA92D ram_crc32=0x9A494230 audio_crc32=0xD8F49994 frames=12 frame_counter=12 scanline_counter=0 audio_samples=4096`
- Determinism check: passed, both headless runs were identical

## Issues Discovered
- The pre-M36 ROM-side VDP contract could not perform deterministic SAT updates after boot because the control port stored only the last byte written and could not load the VRAM pointer. This was resolved by implementing low-byte then high-byte pointer loads on the existing control port and documenting the corrected behavior.

## Next-Step Recommendation
Proceed to `M37 - Sprite Animation` from `docs/plan.md`. The next narrow step should reuse the new SAT update path and local sprite asset flow to introduce deterministic frame switching without expanding into metasprites or gameplay logic.
