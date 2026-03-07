# M16 Foreground Plane Report

Status: Completed

## Files Created
- `emulator/tests/unit/test_foreground_plane.cpp`
- `artifacts/reports/M16_foreground_plane_report.md`

## Files Modified
- `emulator/include/vdp.h`
- `emulator/src/vdp.cpp`
- `emulator/include/bus.h`
- `emulator/src/bus.cpp`
- `emulator/CMakeLists.txt`
- `emulator/tests/unit/test_tile_renderer.cpp`
- `docs/state_snapshot.md`

## FG Tilemap Base And Register Map
- Foreground tilemap base: `0x5000`
- `0xEA` `FG_SCROLL_X`
- `0xEB` `FG_SCROLL_Y`
- Bus VDP routing span: `0xE0-0xEB`

## Composition Rules
- Render order is `BG -> FG -> sprites`.
- Background rendering remains opaque and unchanged in behavior.
- Foreground tilemap entries share the existing pattern table at `0x0000`.
- Foreground pixel value `0` is transparent.
- Non-zero foreground pixels overwrite the current framebuffer value before sprites render.

## Scrolling Behavior
- Background uses `BG_SCROLL_X` and `BG_SCROLL_Y`.
- Foreground uses `FG_SCROLL_X` and `FG_SCROLL_Y`.
- Each plane samples its own world position independently with wrapped `256 x 192` addressing.

## Commands Run
- `cmake -S /home/djglxxii/src/Super_Z80_v2 -B /home/djglxxii/src/Super_Z80_v2/build`
- `cmake --build /home/djglxxii/src/Super_Z80_v2/build`
- `ctest --test-dir /home/djglxxii/src/Super_Z80_v2/build --output-on-failure`

## Build Output
- CMake configure completed successfully.
- Build completed successfully, including `super_z80_test_foreground_plane`.

## Test Results
- `ctest` passed.
- Result: `100% tests passed, 0 tests failed out of 15`

## Commit Hash
- `17383e080e9bac2d8474e992f344ad41de90f3cb`

## Next-Step Recommendation
- Proceed to `M17 - Sprite Priority / Layer Ordering Refinement`.
