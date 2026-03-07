# M8 IRQ Controller Report

## Status
Completed

## Files Created
- `emulator/include/irq_controller.h`
- `emulator/src/irq_controller.cpp`
- `emulator/tests/unit/test_irq.cpp`
- `artifacts/reports/M8_irq_controller_report.md`

## Files Modified
- `docs/state_snapshot.md`
- `emulator/CMakeLists.txt`
- `emulator/include/bus.h`
- `emulator/src/bus.cpp`
- `emulator/include/cpu.h`
- `emulator/src/cpu.cpp`

## IRQ Subsystem Design
- Added `IRQController` with deterministic `status`, `enable`, `acknowledge`, `reset`, and `irq_line` behavior.
- `request()` ORs source bits into the status register.
- `irq_line()` asserts when `(status & enable) != 0`.
- `acknowledge()` clears only the masked status bits.

## Register Map
- `0xF0`: `IRQ_STATUS` read-only status register
- `0xF1`: `IRQ_ENABLE` read/write enable mask register
- `0xF2`: `IRQ_ACK` write-only acknowledge register

## CPU Wiring
- Added `CPU::set_int_line(bool)` and `CPU::int_line() const`.
- `CPU::step()` now refreshes the INT line from `Bus::irq_line()` and presents a maskable interrupt to Z80ex before executing the next instruction.

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
- super_z80_irq_controller
- super_z80_bus
- super_z80_cpu
- super_z80_scheduler
- super_z80_test_irq
```

## Test Results
```text
7/7 tests passed
- super_z80_test_crc32
- super_z80_test_memory
- super_z80_test_io
- super_z80_test_bus
- super_z80_test_irq
- super_z80_test_cpu
- super_z80_test_scheduler
```

## Verification Result
- `cmake -S . -B build`: Passed
- `cmake --build build`: Passed
- `ctest --test-dir build --output-on-failure`: Passed

## Commit Hash
- `17383e080e9bac2d8474e992f344ad41de90f3cb`

## Next-Step Recommendation
- Implement M9 VBlank timing and connect it as the first concrete IRQ source.
