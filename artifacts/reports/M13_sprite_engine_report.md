# M13 Sprite Engine Report

Status: Completed

## Files Created
- `emulator/tests/unit/test_sprite_renderer.cpp`
- `artifacts/reports/M13_sprite_engine_report.md`

## Files Modified
- `emulator/include/vdp.h`
- `emulator/src/vdp.cpp`
- `emulator/CMakeLists.txt`
- `docs/state_snapshot.md`

## Sprite Format and VRAM Layout
- Sprite size: `8 x 8`
- Sprite attribute table base: `0x6000`
- Sprite entry size: `4` bytes
- Supported sprite count: `64`
- Sprite patterns base: `0x0000`
- Pattern address formula: `tile_index * 64`

## Rendering Behavior Summary
- Added `VDP::render_sprites()` and called it after the existing background pass in `VDP::render_frame()`.
- Each sprite entry reads `Y`, `X`, `tile index`, and an unused flags byte from the sprite attribute table.
- Sprites with `Y >= 192` or `X >= 256` are skipped, and partially off-screen sprites are clipped against the framebuffer bounds.
- Sprite pixel value `0` is treated as transparent.
- Non-zero sprite pixels overwrite the existing framebuffer contents from the background pass.

## Commands Run
- `cmake -S . -B build`
- `cmake --build build`
- `ctest --test-dir build --output-on-failure`

## Build Output
- CMake configure completed successfully and reused/generated `build/`.
- Build completed successfully, including `super_z80_test_sprite_renderer`.

## Test Results
- `ctest` passed.
- Result: `100% tests passed, 0 tests failed out of 12`

## Commit Hash
- `17383e080e9bac2d8474e992f344ad41de90f3cb`

## Next-Step Recommendation
- Proceed to `M14` once the next task packet is defined.
