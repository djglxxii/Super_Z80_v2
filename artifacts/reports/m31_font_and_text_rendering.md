# M31 Font and Text Rendering

## Status
Completed.

## Summary
Implemented the first reusable text rendering layer for `rom/showcase/`. The Showcase ROM now writes zero-terminated ASCII strings into the background tilemap by tile coordinates, supports newline-driven multi-line text writes, clears rows deterministically before line updates, and renders `SUPER Z80 SHOWCASE` plus `FONT AND TEXT OK` using the local `8x8` font asset.

## Files Changed
- `rom/showcase/inc/constants.inc`
- `rom/showcase/src/text.asm`
- `docs/showcase_state_snapshot.md`
- `docs/state_snapshot.md`
- `artifacts/reports/m31_font_and_text_rendering.md`

## Commands Run
- `cmake -S . -B build`
- `cmake --build build`
- `ctest --test-dir build --output-on-failure`
- `make -C rom/showcase clean`
- `make -C rom/showcase`
- `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 4`
- `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 4`

## Verification Results
- `cmake -S . -B build`: passed
- `cmake --build build`: passed
- `ctest --test-dir build --output-on-failure`: passed, `38/38` tests
- `make -C rom/showcase clean`: passed
- `make -C rom/showcase`: passed
- First headless run: passed
  `HEADLESS_ROM_RESULT rom_crc32=0x865E5845 ram_crc32=0x9A494230 audio_crc32=0x1F3134B frames=4 frame_counter=4 scanline_counter=0 audio_samples=3200`
- Second headless run: passed with identical output
  `HEADLESS_ROM_RESULT rom_crc32=0x865E5845 ram_crc32=0x9A494230 audio_crc32=0x1F3134B frames=4 frame_counter=4 scanline_counter=0 audio_samples=3200`

## Issues Discovered
- The first implementation pass allowed the row-clear helper to clobber the caller's X coordinate in register `B`; this was fixed before final verification by preserving `BC` across the clear call.
- No remaining known issues were introduced by M31.

## Next-Step Recommendation
Proceed to `M32 - System Splash Screen`, using the new text helpers as the stable text path for splash composition.
