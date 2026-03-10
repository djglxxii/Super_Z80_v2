# M39 Controller Input Demo Report

Status: Completed

## Summary

Extended the Showcase ROM so the existing once-per-frame controller polling path drives the animated metasprite. Directional input now updates `meta_x` and `meta_y` by one pixel per frame while the validated parallax scrolling and fixed animation cadence continue unchanged. Headless runs remain deterministic because idle controller state stays at `0xFF`.

## Files Changed

- `rom/showcase/src/main_loop.asm`
- `docs/showcase_state_snapshot.md`
- `docs/state_snapshot.md`
- `artifacts/reports/m39_controller_input_demo.md`

## Commands Run

- `cmake -S . -B build`
- `cmake --build build`
- `ctest --test-dir build --output-on-failure`
- `make -C rom/showcase clean`
- `make -C rom/showcase`
- `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 24`
- `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 24`

## Verification Results

- Repository configuration completed successfully with CMake.
- Repository build completed successfully.
- `ctest` passed.
- The Showcase ROM rebuilt successfully.
- Both headless Showcase runs produced the same result:
  - `HEADLESS_ROM_RESULT rom_crc32=0xA48747B5 ram_crc32=0x9A494230 audio_crc32=0xD8F49994`

## Issues Discovered

- No new issues were discovered during M39 implementation or verification.

## Next-Step Recommendation

Advance to the next Showcase milestone in `docs/plan.md`, preferably one that builds on visible controller interaction while preserving the current deterministic single-scene structure.
