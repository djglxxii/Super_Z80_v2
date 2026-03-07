# M21 Tile Pattern Banks Report

## Status
Completed

## Summary
Implemented deterministic background and foreground tile pattern bank selection through VDP ports `0xEC` and `0xED`, updated tile plane pattern fetch to resolve bank-aware addresses, extended bus VDP routing through `0xED`, and added focused unit coverage for the new behavior.

## Files Changed
- `emulator/include/vdp.h`
- `emulator/src/vdp.cpp`
- `emulator/include/bus.h`
- `emulator/src/bus.cpp`
- `emulator/CMakeLists.txt`
- `emulator/tests/unit/test_bus.cpp`
- `emulator/tests/unit/test_tile_pattern_banks.cpp`
- `docs/state_snapshot.md`
- `artifacts/reports/M21_tile_pattern_banks_report.md`

## Commands Run
- `cmake -S /home/djglxxii/src/Super_Z80_v2 -B /home/djglxxii/src/Super_Z80_v2/build`
- `cmake --build /home/djglxxii/src/Super_Z80_v2/build --target super_z80_test_tile_pattern_banks super_z80_test_bus`
- `ctest --test-dir /home/djglxxii/src/Super_Z80_v2/build --output-on-failure -R 'super_z80_test_(tile_pattern_banks|bus)$'`

## Verification Results
- Configure succeeded.
- Build succeeded for `super_z80_test_tile_pattern_banks` and `super_z80_test_bus`.
- `ctest` passed both `super_z80_test_bus` and `super_z80_test_tile_pattern_banks`.

## Next-Step Recommendations
- Add the next task packet before changing milestone scope further.
