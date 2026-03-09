# M33 Scrolling Tilemap Demo Report

## Status
Completed.

## Summary
Added deterministic background scrolling to the Showcase ROM by introducing RAM-backed scroll state, incrementing horizontal scroll once per frame in the existing main loop, and writing the current values to the VDP background scroll registers during render. The demo reuses the existing splash tilemap and text composition path, so the ROM now shows continuous visual motion without changing the asset pipeline or adding scene management.

## Files Changed
- `rom/showcase/inc/memory_map.inc`
- `rom/showcase/src/init.asm`
- `rom/showcase/src/vdp_init.asm`
- `rom/showcase/src/main_loop.asm`
- `rom/showcase/src/text.asm`
- `docs/showcase_state_snapshot.md`
- `docs/state_snapshot.md`
- `artifacts/reports/m33_scrolling_tilemap_demo.md`

## Commands Run
- `cmake -S . -B build`
- `cmake --build build`
- `ctest --test-dir build --output-on-failure`
- `make -C rom/showcase clean`
- `make -C rom/showcase`
- `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 8`
- `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 8`

## Verification Results
- Repository configure/build succeeded.
- `ctest` passed: `38/38` tests.
- Showcase ROM assembled successfully with `sjasmplus`.
- Repeated headless runs were deterministic and produced identical results:
  - `HEADLESS_ROM_RESULT rom_crc32=0xA0F7C5B ram_crc32=0x9A494230 audio_crc32=0xD8F49994 frames=8 frame_counter=8 scanline_counter=0 audio_samples=4096`

## Issues Discovered
- None.

## Next-Step Recommendation
Proceed to `M34 - Sprite Rendering Demo` and layer sprite upload/render validation onto the now-deterministic scrolling Showcase background.
