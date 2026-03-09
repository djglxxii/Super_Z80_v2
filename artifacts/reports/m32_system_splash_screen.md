# M32 System Splash Screen Report

## Status
Completed.

## Summary
Implemented a deterministic Showcase ROM splash screen that boots directly into a composed startup presentation using the local splash tile/tilemap assets and the reusable M31 text path. Fixed the splash asset integration so the tilemap now targets the uploaded splash tile block instead of the font tile range.

## Files Changed
- `rom/showcase/inc/constants.inc`
- `rom/showcase/src/init.asm`
- `rom/showcase/src/text.asm`
- `rom/showcase/assets/splash_tiles.asm`
- `rom/showcase/assets/splash_tilemap.asm`
- `docs/showcase_state_snapshot.md`
- `docs/state_snapshot.md`
- `artifacts/reports/m32_system_splash_screen.md`

## Commands Run
- `cmake -S . -B build`
- `cmake --build build`
- `ctest --test-dir build --output-on-failure`
- `make -C rom/showcase clean`
- `make -C rom/showcase`
- `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 4`
- `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 4`

## Verification Results
- Repository configure/build completed successfully.
- Test suite passed.
- Showcase ROM rebuilt successfully.
- Repeated headless ROM execution produced identical `HEADLESS_ROM_RESULT` summaries, confirming deterministic splash boot behavior.

## Issues Discovered
- The pre-M32 splash tilemap used low raw tile indices that resolved into the font tile range because splash tiles are uploaded after the font block. This milestone corrected that asset-to-VRAM indexing mismatch.

## Next-Step Recommendation
Proceed to `M33 - Scrolling Tilemap Demo` and build scrolling behavior on top of the now-stable splash composition and asset upload flow.
