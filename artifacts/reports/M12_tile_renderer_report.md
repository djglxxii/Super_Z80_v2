# M12 Tile Renderer Report

Status: Completed

## Files Created
- `emulator/tests/unit/test_tile_renderer.cpp`
- `artifacts/reports/M12_tile_renderer_report.md`

## Files Modified
- `emulator/include/vdp.h`
- `emulator/src/vdp.cpp`
- `emulator/CMakeLists.txt`
- `docs/state_snapshot.md`

## Temporary Tile and Tilemap Format
- Screen: `256 x 192`
- Tile size: `8 x 8`
- Tile grid: `32 x 24`
- Tile patterns base: `0x0000`
- Tilemap base: `0x4000`
- Pattern format: `64` bytes per tile, `1` byte per pixel
- Tilemap format: `1` byte per entry containing only the tile index

## Rendering Behavior Summary
- Added `VDP::render_frame()` to generate the internal framebuffer from VRAM.
- The renderer iterates all `32 x 24` tilemap entries.
- Each tile index is read from `0x4000 + tile_y * 32 + tile_x`.
- Each tile pattern is read from `0x0000 + tile_index * 64`.
- Tile pixels are copied directly into the `256 x 192` framebuffer as `8-bit` pixel indices.
- Added framebuffer pixel access and direct VRAM writes for deterministic test coverage.

## Commands Run
- `cmake -S . -B build`
- `cmake --build build`
- `ctest --test-dir build --output-on-failure`

## Build Output
- CMake configure completed successfully and generated `build/`.
- Build completed successfully, including `super_z80_test_tile_renderer`.

## Test Results
- `ctest` passed.
- Result: `100% tests passed, 0 tests failed out of 11`

## Commit Hash
- `17383e080e9bac2d8474e992f344ad41de90f3cb`

## Next-Step Recommendation
- Proceed to `M13 -- Sprite Engine Skeleton`.
