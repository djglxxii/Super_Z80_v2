# M15 Scrolling Report

Status: Completed

## Files Created
- `emulator/tests/unit/test_scrolling.cpp`
- `artifacts/reports/M15_scrolling_report.md`

## Files Modified
- `emulator/include/vdp.h`
- `emulator/src/vdp.cpp`
- `emulator/include/bus.h`
- `emulator/src/bus.cpp`
- `emulator/CMakeLists.txt`
- `docs/state_snapshot.md`

## Scroll Register Map
- `0xE8` `BG_SCROLL_X`
- `0xE9` `BG_SCROLL_Y`

## Behavior Implemented
- Added `8-bit` background scroll registers with reset-to-zero behavior.
- VDP port reads and writes now expose the current scroll values directly.
- Background rendering now samples from `world_x = (screen_x + bg_scroll_x) mod 256` and `world_y = (screen_y + bg_scroll_y) mod 192`.
- Tilemap lookup wraps over the current `32 x 24` tilemap without out-of-bounds reads.
- Bus VDP port routing now forwards `0xE0-0xE9` so the scroll registers are CPU-visible.

## Unit Test Coverage
- Zero scroll preserves existing tile placement.
- Horizontal scroll shifts the viewport by one tile.
- Vertical scroll shifts the viewport by one tile row.
- Partial pixel scroll shifts intra-tile sampling correctly.
- Wraparound at the framebuffer edges resolves to wrapped tilemap entries.
- Scroll registers read back correctly and reset through the bus.

## Commands Run
- `cmake -S /home/djglxxii/src/Super_Z80_v2 -B /home/djglxxii/src/Super_Z80_v2/build`
- `cmake --build /home/djglxxii/src/Super_Z80_v2/build`
- `ctest --test-dir /home/djglxxii/src/Super_Z80_v2/build --output-on-failure`

## Build Output
- CMake configure completed successfully.
- Build completed successfully, including `super_z80_test_scrolling`.

## Test Results
- `ctest` passed.
- Result: `100% tests passed, 0 tests failed out of 14`

## Commit Hash
- `17383e080e9bac2d8474e992f344ad41de90f3cb`

## Next-Step Recommendation
- Select and execute the next task packet after `M15`.
