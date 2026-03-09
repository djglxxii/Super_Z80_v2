# Task Completion Report

## Title
M29c - YM2151 Timers, Status Bits, and IRQ Behavior

## Status
Completed

## Summary
Implemented deterministic YM2151 Timer A and Timer B state on top of the existing register and channel/operator model. Timer-related register writes on `0x70-0x71` now update timer latches, control bits, overflow state, and IRQ-facing pending behavior while preserving raw register storage. The scheduler-owned `tick(uint32_t cycles)` hook now advances YM2151 timers with an explicit fixed cycle budget per scanline. No FM sample generation, PSG/YM mixing, or SDL audio changes were added.

## Files Changed
- `emulator/audio/ym2151/ym2151.h`
- `emulator/audio/ym2151/ym2151.cpp`
- `emulator/include/bus.h`
- `emulator/src/bus.cpp`
- `emulator/include/scheduler.h`
- `emulator/src/scheduler.cpp`
- `emulator/tests/unit/test_ym2151.cpp`
- `emulator/tests/unit/test_scheduler.cpp`
- `docs/state_snapshot.md`
- `artifacts/reports/m29c_ym2151_timers_irq.md`

## Commands Run
- `cmake -S . -B build`
- `cmake --build build`
- `ctest --test-dir build --output-on-failure --tests-regex "super_z80_test_(ym2151|scheduler|bus)"`
- `ctest --test-dir build --output-on-failure`

## Verification Results
- `super_z80_test_ym2151` passed.
- `super_z80_test_scheduler` passed.
- `super_z80_test_bus` passed.
- Full `ctest --test-dir build --output-on-failure` suite passed: `35/35` tests.

## Timer/Status Behavior Implemented
- Timer A latch uses YM2151 registers `0x10` and `0x11` as a deterministic 10-bit stored value.
- Timer B latch uses YM2151 register `0x12` as a deterministic 8-bit stored value.
- Control register `0x14` now updates timer start bits, timer IRQ-enable bits, and timer overflow-clear bits.
- Port `0x70` now reads YM2151 status bits, with bit `0` for Timer A overflow and bit `1` for Timer B overflow.
- Timer overflow flags remain set until cleared through the control-register clear path.
- `irq_pending()` now reports true when a timer has overflowed and that timer's IRQ-enable bit is set.
- `Bus::ym2151_irq_pending()` exposes the YM2151 pending state for future interrupt integration without routing it into CPU interrupt delivery in this milestone.
- `Scheduler::step_scanline()` now advances YM2151 timing with a fixed explicit cycle budget via `YM2151::tick(kYm2151CyclesPerScanline)`.

## Issues Encountered
- The repository does not yet expose instruction-accurate CPU cycle counts to the scheduler, so YM2151 timer cadence remains tied to the scheduler's existing fixed-step scanline model.
- Current repository truth did not fully define YM2151 timer countdown formulas, so this milestone adopted the minimal deterministic model required for future FM work: Timer A uses `1024 - latch`, and Timer B uses `(256 - latch) * 16`.

## Recommended Next Step
M29d - YM2151 FM Sample Generation
