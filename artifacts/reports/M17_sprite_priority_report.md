# M17 Sprite Priority Report

Status: Completed

## Files Created
- `emulator/tests/unit/test_sprite_priority.cpp`
- `artifacts/reports/M17_sprite_priority_report.md`

## Files Modified
- `emulator/include/vdp.h`
- `emulator/src/vdp.cpp`
- `emulator/CMakeLists.txt`
- `docs/state_snapshot.md`

## Sprite Attribute Rules
- Sprite flags bit `0` is the priority flag.
- `0` renders the sprite in front of the foreground plane.
- `1` renders the sprite behind the foreground plane.
- Bits `1-7` remain reserved.

## Composition Behavior
- Render order is `BG -> sprites(priority=1) -> FG -> sprites(priority=0)`.
- Sprite pixels remain transparent only when their pattern value is `0`.
- Non-zero sprite pixels overwrite the current framebuffer value in their active pass.
- SAT ordering remains stable: lower indices draw first, higher indices draw later and can overwrite them.

## Commands Run
- `cmake -S /home/djglxxii/src/Super_Z80_v2 -B /home/djglxxii/src/Super_Z80_v2/build`
- `cmake --build /home/djglxxii/src/Super_Z80_v2/build`
- `ctest --test-dir /home/djglxxii/src/Super_Z80_v2/build --output-on-failure`

## Verification Results
- CMake configure completed successfully.
- Build completed successfully, including `super_z80_test_sprite_priority`.
- `ctest` passed with `100% tests passed, 0 tests failed out of 16`.

## Next-Step Recommendation
- Wait for the next milestone packet before extending sprite hardware behavior further.
