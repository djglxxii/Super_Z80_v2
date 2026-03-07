# M20 Sprite Pattern Banks Report

## Status
Completed

## Summary
Implemented sprite pattern bank selection through SAT flags bit `3` and extended sprite pattern addressing to fetch from bank `0` at `0x0000` or bank `1` at `0x2000`.

## Files Changed
- `emulator/include/vdp.h`
- `emulator/src/vdp.cpp`
- `emulator/CMakeLists.txt`
- `emulator/tests/unit/test_sprite_pattern_banks.cpp`
- `docs/state_snapshot.md`
- `artifacts/reports/M20_sprite_pattern_banks_report.md`

## Commands Run
- `cmake -S . -B build`
- `cmake --build build`
- `ctest --test-dir build --output-on-failure`

## Verification Results
- `cmake -S . -B build`: passed
- `cmake --build build`: passed
- `ctest --test-dir build --output-on-failure`: passed, `19/19` tests successful including `super_z80_test_sprite_pattern_banks`

## Notes
- Sprite rendering behavior outside of pattern address resolution remained unchanged.
- Repository state snapshot advanced to M20 and records M21 as the next milestone.

## Next-Step Recommendations
- Execute M21 to extend banked pattern addressing to tile planes without changing sprite timing behavior.
