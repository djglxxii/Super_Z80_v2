# M14 Palette Report

Status: Completed

## Files Created
- `emulator/tests/unit/test_palette.cpp`
- `artifacts/reports/M14_palette_report.md`

## Files Modified
- `emulator/include/vdp.h`
- `emulator/src/vdp.cpp`
- `emulator/include/bus.h`
- `emulator/src/bus.cpp`
- `emulator/CMakeLists.txt`
- `docs/state_snapshot.md`

## Palette Register Map
- `0xE3` `PAL_INDEX`
- `0xE4` `PAL_DATA_R`
- `0xE5` `PAL_DATA_G`
- `0xE6` `PAL_DATA_B`
- `0xE7` `PAL_DATA_A`

## Palette Storage Format
- Entry count: `256`
- Entry type: `uint32_t`
- Packing order: `0xRRGGBBAA`

## Default Reset State
- `palette[i] = 0x00000000` for all `0 <= i < 256`
- Active palette index resets to `0x00`

## Color Resolution Behavior
- Tile and sprite rendering still write `8-bit` palette indices into the framebuffer.
- `VDP::framebuffer_color(pixel_index)` resolves `framebuffer[pixel_index]` through the palette table.
- `VDP::resolve_pixel(palette_index)` returns the packed RGBA value for that palette entry.
- Bus routing now forwards `0xE0-0xE7` to the VDP so the new palette ports are CPU-visible.

## Commands Run
- `cmake -S /home/djglxxii/src/Super_Z80_v2 -B /home/djglxxii/src/Super_Z80_v2/build`
- `cmake --build /home/djglxxii/src/Super_Z80_v2/build`
- `ctest --test-dir /home/djglxxii/src/Super_Z80_v2/build --output-on-failure`

## Build Output
- CMake configure completed successfully.
- Build completed successfully, including `super_z80_test_palette`.

## Test Results
- `ctest` passed.
- Result: `100% tests passed, 0 tests failed out of 13`

## Commit Hash
- `17383e080e9bac2d8474e992f344ad41de90f3cb`

## Next-Step Recommendation
- Proceed to `M15 -- Scrolling and Viewport Movement`.
