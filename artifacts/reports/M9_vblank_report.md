# M9 VBlank Report

## Status
Completed

## Files Created
- `emulator/include/vblank.h`
- `emulator/src/vblank.cpp`
- `emulator/tests/unit/test_vblank.cpp`
- `artifacts/reports/M9_vblank_report.md`

## Files Modified
- `docs/plan.md`
- `docs/state_snapshot.md`
- `emulator/CMakeLists.txt`
- `emulator/include/bus.h`
- `emulator/include/scheduler.h`
- `emulator/src/bus.cpp`
- `emulator/src/scheduler.cpp`
- `emulator/tests/unit/test_scheduler.cpp`

## VBlank Timing Model
- Frame timing remains `262` scanlines per frame.
- Visible scanlines are `0-191`.
- VBlank begins when the scheduler advances to scanline `192`.
- VBlank ends when the scheduler wraps to scanline `0`.

## IRQ Integration
- Added `VBlank` as a deterministic scanline-driven subsystem.
- `VBlank` requests IRQ bit `0x01` exactly once when entering VBlank.
- `Bus` owns the canonical `VBlank` instance so port `0xF3` can expose the VBlank-active status bit.
- `Scheduler` now drives `VBlank::update_scanline()` after each scanline advance.
- The existing bus-to-CPU IRQ line path now receives a concrete hardware source.

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
- super_z80_vblank
- super_z80_bus
- super_z80_scheduler
- super_z80_test_scheduler
- super_z80_test_vblank
```

## Test Results
```text
8/8 tests passed
- super_z80_test_crc32
- super_z80_test_memory
- super_z80_test_io
- super_z80_test_bus
- super_z80_test_irq
- super_z80_test_cpu
- super_z80_test_scheduler
- super_z80_test_vblank
```

## Verification Result
- `cmake -S . -B build`: Passed
- `cmake --build build`: Passed
- `ctest --test-dir build --output-on-failure`: Passed

## Commit Hash
- `17383e080e9bac2d8474e992f344ad41de90f3cb`

## Next-Step Recommendation
- Implement M10 DMA Engine using the established scheduler-to-hardware event path.
