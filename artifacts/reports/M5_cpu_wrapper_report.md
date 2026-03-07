# M5 CPU Wrapper Report

## Status
Completed

## Summary
- Vendored Z80ex into `emulator/vendor/z80ex/`.
- Added a `CPU` wrapper that connects Z80ex memory callbacks to `Bus::read()` and `Bus::write()`.
- Added deterministic reset and single-instruction stepping.
- Added a unit test covering `LD A,0x42` and `HALT`.
- Corrected milestone tracking in repository truth docs to align with the active M5 task packet.

## Files Changed
- `CMakeLists.txt`
- `emulator/CMakeLists.txt`
- `emulator/include/cpu.h`
- `emulator/src/cpu.cpp`
- `emulator/tests/unit/test_cpu.cpp`
- `emulator/vendor/z80ex/README.md`
- `emulator/vendor/z80ex/README`
- `emulator/vendor/z80ex/COPYING`
- `emulator/vendor/z80ex/include/z80ex.h`
- `emulator/vendor/z80ex/include/z80ex_common.h`
- `emulator/vendor/z80ex/macros.h`
- `emulator/vendor/z80ex/opcodes/opcodes_base.c`
- `emulator/vendor/z80ex/opcodes/opcodes_cb.c`
- `emulator/vendor/z80ex/opcodes/opcodes_dasm.c`
- `emulator/vendor/z80ex/opcodes/opcodes_dd.c`
- `emulator/vendor/z80ex/opcodes/opcodes_ddcb.c`
- `emulator/vendor/z80ex/opcodes/opcodes_ed.c`
- `emulator/vendor/z80ex/opcodes/opcodes_fd.c`
- `emulator/vendor/z80ex/opcodes/opcodes_fdcb.c`
- `emulator/vendor/z80ex/ptables.c`
- `emulator/vendor/z80ex/typedefs.h`
- `emulator/vendor/z80ex/z80ex.c`
- `docs/state_snapshot.md`
- `docs/plan.md`
- `artifacts/reports/M5_cpu_wrapper_report.md`

## Commands Run
- `cmake -S . -B build`
- `cmake --build build`
- `ctest --test-dir build --output-on-failure`

## Verification Results
- `cmake -S . -B build`: Passed
- `cmake --build build`: Passed
- `ctest --test-dir build --output-on-failure`: Passed (`4/4` tests)

## Next-Step Recommendations
- Wait for the next task packet before expanding CPU integration beyond reset, stepping, and basic state validation.
