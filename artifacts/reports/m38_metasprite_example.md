# M38 Metasprite Example Report

## Status
Completed

## Summary
- Extended the Showcase ROM sprite demo into a deterministic `2x2` metasprite composed of four hardware sprites.
- Replaced per-sprite position state with shared `meta_x` and `meta_y` origin bytes while preserving the existing animation cadence and parallax scene behavior.
- Updated the local sprite asset set so each animation frame now contains four explicit `8x8` tiles used by SAT entries `0` through `3`.

## Files Changed
- `rom/showcase/assets/demo_sprite.asm`
- `rom/showcase/inc/constants.inc`
- `rom/showcase/inc/memory_map.inc`
- `rom/showcase/src/main_loop.asm`
- `rom/showcase/src/vdp_init.asm`
- `docs/showcase_state_snapshot.md`
- `docs/state_snapshot.md`
- `artifacts/reports/m38_metasprite_example.md`

## Commands Run
- `cmake -S . -B build`
- `cmake --build build`
- `ctest --test-dir build --output-on-failure`
- `make -C rom/showcase clean`
- `make -C rom/showcase`
- `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 20`
- `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 20`

## Verification Results
- `cmake --build build`: passed
- `ctest --test-dir build --output-on-failure`: passed (`38/38` tests)
- `make -C rom/showcase`: passed
- Headless run 1: `HEADLESS_ROM_RESULT rom_crc32=0xBBF8B4A9 ram_crc32=0x9A494230 audio_crc32=0xD8F49994 frames=20 frame_counter=20 scanline_counter=0 audio_samples=4096`
- Headless run 2: identical to run 1

## Issues Discovered
- No new issues were discovered during M38 implementation or verification.

## Next-Step Recommendation
- Preserve any controller-driven metasprite follow-up as a non-milestone enhancement if needed, then proceed to `M39 - PSG Sound Effects` per `docs/plan.md`.
