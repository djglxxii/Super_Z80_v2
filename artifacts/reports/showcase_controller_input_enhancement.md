# Showcase Controller Input Enhancement Report

Status: Completed

## Summary

Requested milestone in stale packet/report: M39 Controller Input Demo
Authoritative next milestone in docs/plan.md: M39 PSG Sound Effects
Alignment result: CONFLICT
Resolution: Reclassified controller-input work as non-milestone enhancement; preserved official M39 for PSG Sound Effects

Extended the Showcase ROM so the existing once-per-frame controller polling path drives the animated metasprite as a post-M38 Showcase enhancement. Directional input updates `meta_x` and `meta_y` by one pixel per frame while the validated parallax scrolling and fixed animation cadence continue unchanged. Headless runs remain deterministic because idle controller state stays at `0xFF`.

## Files Changed

- `rom/showcase/src/main_loop.asm`
- `docs/showcase_state_snapshot.md`
- `docs/state_snapshot.md`
- `artifacts/reports/showcase_controller_input_enhancement.md`

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

## Exact Reconciliation Performed

- Renamed the stale `M39` controller-input report so it no longer claims the official `M39` milestone.
- Reclassified controller-driven metasprite movement as a non-milestone Showcase enhancement layered on top of completed `M38`.
- Updated repository snapshots so the next official Showcase milestone remains `M39 - PSG Sound Effects`.

## Issues Discovered

- No new issues were discovered during the controller-input enhancement implementation or verification.

## Next-Step Recommendation

Advance to `M39 - PSG Sound Effects` as defined in `docs/plan.md`.
