# M18 Sprite Scanline Limits Report

Status: Completed

## Files Created
- `emulator/tests/unit/test_sprite_scanline_limits.cpp`
- `artifacts/reports/M18_sprite_scanline_limits_report.md`

## Files Modified
- `emulator/include/vdp.h`
- `emulator/src/vdp.cpp`
- `emulator/CMakeLists.txt`
- `docs/state_snapshot.md`

## Behavior Changes
- The VDP now evaluates the sprite attribute table once per visible scanline.
- Only the first 16 sprites intersecting a scanline are rendered for that scanline.
- SAT ordering remains deterministic within the selected 16 sprites.
- VDP status bit `2` now reports sprite overflow and clears on status read.

## Commands Run
- `cmake -S /home/djglxxii/src/Super_Z80_v2 -B /home/djglxxii/src/Super_Z80_v2/build`
- `cmake --build /home/djglxxii/src/Super_Z80_v2/build`
- `ctest --test-dir /home/djglxxii/src/Super_Z80_v2/build --output-on-failure`

## Verification Results
- CMake configure completed successfully.
- Build completed successfully, including `super_z80_test_sprite_scanline_limits`.
- `ctest` passed with `100% tests passed, 0 tests failed out of 17`.

## Next-Step Recommendation
- Wait for the next task packet before extending sprite hardware semantics further.
