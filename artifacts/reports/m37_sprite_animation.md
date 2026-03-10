# M37 Sprite Animation Report

## Status
Completed.

## Summary
Extended the Showcase ROM sprite demo to animate a single moving sprite with two deterministic local frames. The ROM now keeps explicit `sprite_frame` and `sprite_anim_counter` state in RAM, toggles frames every `8` updates, and rewrites SAT entry `0` with the current frame tile while preserving the M35 parallax scene and the M36 SAT update path.

## Files Changed
- `rom/showcase/assets/demo_sprite.asm`
- `rom/showcase/inc/constants.inc`
- `rom/showcase/inc/memory_map.inc`
- `rom/showcase/src/main_loop.asm`
- `rom/showcase/src/vdp_init.asm`
- `docs/showcase_state_snapshot.md`
- `docs/state_snapshot.md`
- `artifacts/reports/m37_sprite_animation.md`

## Commands Run
- `cmake -S . -B build`
- `cmake --build build`
- `ctest --test-dir build --output-on-failure`
- `make -C rom/showcase clean`
- `make -C rom/showcase`
- `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 16`
- `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 16`

## Verification Results
- `cmake -S . -B build`: passed
- `cmake --build build`: passed
- `ctest --test-dir build --output-on-failure`: passed (`38/38` tests)
- `make -C rom/showcase clean`: passed
- `make -C rom/showcase`: passed
- Headless run 1: `HEADLESS_ROM_RESULT rom_crc32=0xCAEAE31F ram_crc32=0x9A494230 audio_crc32=0xD8F49994 frames=16 frame_counter=16 scanline_counter=0 audio_samples=4096`
- Headless run 2: `HEADLESS_ROM_RESULT rom_crc32=0xCAEAE31F ram_crc32=0x9A494230 audio_crc32=0xD8F49994 frames=16 frame_counter=16 scanline_counter=0 audio_samples=4096`
- Determinism check: passed, both headless runs were identical

## Issues Discovered
- No new emulator or ROM contract defects were discovered during this milestone.

## Next-Step Recommendation
Proceed to `M38 - Metasprite Example`. Reuse the current explicit sprite state and SAT rewrite pattern, but expand it into a small multi-sprite composition without introducing generalized gameplay or scene-management systems.
