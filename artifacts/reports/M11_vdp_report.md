# M11 VDP Report

## Status
Completed

## Files Created
- `artifacts/reports/M11_vdp_report.md`
- `emulator/include/vdp.h`
- `emulator/src/vdp.cpp`
- `emulator/tests/unit/test_vdp.cpp`

## Files Modified
- `docs/state_snapshot.md`
- `emulator/CMakeLists.txt`
- `emulator/include/bus.h`
- `emulator/include/scheduler.h`
- `emulator/src/bus.cpp`
- `emulator/src/scheduler.cpp`
- `emulator/tests/unit/test_bus.cpp`
- `emulator/tests/unit/test_dma.cpp`
- `emulator/tests/unit/test_scheduler.cpp`
- `emulator/tests/unit/test_vblank.cpp`

## VRAM Layout
- `64 KB` VRAM stored in `std::array<uint8_t, 65536>`.
- Sequential `VDP_DATA` reads and writes use `vram_ptr_` and auto-increment after each access.
- Reset clears VRAM, the framebuffer, the control latch, and the VRAM pointer.

## VDP Port Map
- `0xE0`: `VDP_STATUS`
  Bits: `bit0 = VBlank active`, `bit1 = frame ready`
- `0xE1`: `VDP_CONTROL`
  Stores the last written control byte for future register expansion.
- `0xE2`: `VDP_DATA`
  Reads or writes VRAM at the current pointer, then auto-increments.

## Commands Run
```text
cmake -S /home/djglxxii/src/Super_Z80_v2 -B /home/djglxxii/src/Super_Z80_v2/build
cmake --build /home/djglxxii/src/Super_Z80_v2/build
ctest --test-dir /home/djglxxii/src/Super_Z80_v2/build --output-on-failure
```

## Build Output
```text
Configure completed successfully.
Build completed successfully.
Targets built include:
- super_z80_vdp
- super_z80_bus
- super_z80_scheduler
- super_z80_test_scheduler
- super_z80_test_vdp
```

## Test Results
```text
10/10 tests passed
- super_z80_test_crc32
- super_z80_test_memory
- super_z80_test_io
- super_z80_test_bus
- super_z80_test_irq
- super_z80_test_cpu
- super_z80_test_scheduler
- super_z80_test_vblank
- super_z80_test_dma
- super_z80_test_vdp
```

## Verification Result
- `cmake -S . -B build`: Passed
- `cmake --build build`: Passed
- `ctest --test-dir build --output-on-failure`: Passed

## Commit Hash
- `17383e0`
