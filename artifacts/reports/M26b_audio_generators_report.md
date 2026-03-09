# M26b Audio Generators Report

## Status
Completed

## Summary
Implemented deterministic APU tone and noise generator state progression behind the existing audio register interface. The APU now exposes an explicit `advance_audio_ticks()` API, maintains internal divider/phase/LFSR runtime state, couples register writes to future generator behavior, and remains silent with no mixer or SDL output work added.

## Files Changed
- `emulator/include/apu.h`
- `emulator/include/apu_registers.h`
- `emulator/src/apu.cpp`
- `emulator/CMakeLists.txt`
- `docs/state_snapshot.md`

## Files Created
- `emulator/tests/unit/test_apu_generators.cpp`
- `artifacts/reports/M26b_audio_generators_report.md`

## Commands Executed
- `cmake -S . -B build`
- `cmake --build build`
- `ctest --test-dir build --output-on-failure`

## Verification Results
- `cmake -S . -B build`: passed
- `cmake --build build`: passed
- `ctest --test-dir build --output-on-failure`: passed
- Full test suite result: `24/24` tests passed

## Issues Encountered
- The audio spec left the exact periodic-noise reset-on-cycle behavior to M26b. This milestone resolves that by using deterministic shift-toward-zero periodic noise that reseeds the 15-bit register to `0x4000` when the cycle completes.

## Recommended Next Step
M26c — Audio Mixer and Sample Output State
