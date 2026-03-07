# M22 Sprite Collision Report

## Status
Completed

## Files Created
- `emulator/tests/unit/test_sprite_collision.cpp`
- `artifacts/reports/M22_sprite_collision_report.md`

## Files Modified
- `emulator/include/vdp.h`
- `emulator/src/vdp.cpp`
- `emulator/CMakeLists.txt`
- `emulator/tests/unit/test_sprite_scanline_limits.cpp`
- `docs/state_snapshot.md`

## Collision Detection Algorithm
- Added a per-frame sprite-only mask buffer sized to the framebuffer.
- Cleared the mask at the start of each `render_frame()`.
- During sprite rendering, each non-zero sprite pixel checks the mask at its target screen coordinate.
- If the mask is already set, the VDP latches `sprite_collision_ = true`.
- The current sprite pixel then marks the mask and writes the framebuffer.
- Background and foreground rendering never touch the sprite mask, so only sprite-to-sprite overlap contributes.

## Status Register Semantics
- VDP status bit `0`: VBlank
- VDP status bit `1`: frame ready
- VDP status bit `2`: sprite overflow
- VDP status bit `3`: sprite collision
- Reading the status port clears `frame_ready_`, `sprite_overflow_`, and `sprite_collision_`.
- Collision detection remains deterministic because sprite evaluation order and sprite rendering order remain SAT-ordered within the existing two sprite priority passes.

## Build Output
- `cmake -S /home/djglxxii/src/Super_Z80_v2 -B /home/djglxxii/src/Super_Z80_v2/build`: passed
- `cmake --build /home/djglxxii/src/Super_Z80_v2/build`: passed

## Test Results
- `ctest --test-dir /home/djglxxii/src/Super_Z80_v2/build --output-on-failure`: passed, `100% tests passed, 0 tests failed out of 21`
- New `super_z80_test_sprite_collision` coverage:
- no collision
- simple collision
- transparency prevents collision
- collision flag clears on read
- collision detection independent of priority

## Notes
- `emulator/tests/unit/test_sprite_scanline_limits.cpp` was updated so the pre-existing overflow status test isolates overflow without also triggering the newly implemented collision flag.

## Commit Hash
- `17383e080e9bac2d8474e992f344ad41de90f3cb`
