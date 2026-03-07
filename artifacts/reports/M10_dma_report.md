# M10 DMA Report

## Status
Completed

## Files Created
- `artifacts/reports/M10_dma_report.md`
- `emulator/include/dma.h`
- `emulator/src/dma.cpp`
- `emulator/tests/unit/test_dma.cpp`

## Files Modified
- `docs/state_snapshot.md`
- `emulator/CMakeLists.txt`
- `emulator/include/bus.h`
- `emulator/include/scheduler.h`
- `emulator/src/bus.cpp`
- `emulator/src/scheduler.cpp`
- `emulator/tests/unit/test_bus.cpp`
- `emulator/tests/unit/test_scheduler.cpp`

## DMA Implementation Summary
- Added a deterministic `DMA` subsystem owned by `Bus`.
- Exposed DMA registers on ports `0xF4-0xF9` for source, destination, length, and control.
- Writing control bit `0` starts a transfer when `length > 0`.
- Reading the control register exposes busy state on bit `7`.
- Each `DMA::step()` copies one byte, increments source and destination, decrements length, and clears busy on completion.
- `Scheduler::step_scanline()` now advances DMA once per scanline after CPU stepping.

## Commands Run
```text
cmake -S /home/djglxxii/src/Super_Z80_v2 -B /home/djglxxii/src/Super_Z80_v2/build
cmake --build /home/djglxxii/src/Super_Z80_v2/build
ctest --test-dir /home/djglxxii/src/Super_Z80_v2/build --output-on-failure
```

## Build Output
```text
Build completed successfully.
Targets built include:
- super_z80_bus
- super_z80_scheduler
- super_z80_test_bus
- super_z80_test_scheduler
- super_z80_test_dma
```

## Test Results
```text
9/9 tests passed
- super_z80_test_crc32
- super_z80_test_memory
- super_z80_test_io
- super_z80_test_bus
- super_z80_test_irq
- super_z80_test_cpu
- super_z80_test_scheduler
- super_z80_test_vblank
- super_z80_test_dma
```

## Verification Result
- `cmake -S . -B build`: Passed
- `cmake --build build`: Passed
- `ctest --test-dir build --output-on-failure`: Passed

## Next-Step Recommendation
- Implement M11 video pipeline plumbing on top of the existing scheduler-owned hardware timing path.
