# M34 Vertical Scrolling Demo Report

## Status
Completed.

## Summary
Extended the Showcase ROM's deterministic scrolling demo so the main loop now advances `scroll_y` alongside the existing horizontal motion and applies both axes through the VDP background scroll registers during render. The ROM continues to use the existing splash tilemap and reusable text composition path, making the result a simple combined X/Y scrolling reference for ROM developers.

## Files Changed
- `rom/showcase/src/main_loop.asm`
- `rom/showcase/src/text.asm`
- `docs/showcase_state_snapshot.md`
- `docs/state_snapshot.md`
- `artifacts/reports/m34_vertical_scrolling_demo.md`

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
- Repeated headless runs were deterministic and produced identical output:
  - `HEADLESS_ROM_RESULT rom_crc32=0x4D664F47 ram_crc32=0x9A494230 audio_crc32=0xD8F49994 frames=8 frame_counter=8 scanline_counter=0 audio_samples=4096`

## Issues Discovered
- Corrected stale Showcase documentation from the prior milestone that still described M34 as a sprite demo even though `docs/plan.md` defines M34 as vertical scrolling.

## Next-Step Recommendation
Proceed to `M35 - Parallax Scrolling Demo` and reuse the now-explicit per-frame X/Y scroll update path to demonstrate independent layer motion.
