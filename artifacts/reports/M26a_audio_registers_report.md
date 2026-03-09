# M26a Audio Registers Report

## Status
Completed

## Summary
Implemented a dedicated silent APU device that stores the CPU-visible audio register block defined in `docs/audio_spec.md`. The bus now routes ports `0xD0-0xDB` into that device, reset values match the spec, unused bits are masked on write/readback, and `AUD_CTRL` preserves only the defined readable bits while keeping the reset-state bit self-clearing.

## Files Changed
- `emulator/CMakeLists.txt`
- `emulator/include/bus.h`
- `emulator/src/bus.cpp`
- `docs/state_snapshot.md`

## Files Created
- `emulator/include/apu.h`
- `emulator/include/apu_registers.h`
- `emulator/src/apu.cpp`
- `emulator/tests/unit/test_apu.cpp`
- `artifacts/reports/M26a_audio_registers_report.md`

## Commands Executed
```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## Verification Results
- `cmake -S . -B build`: passed
- `cmake --build build`: passed
- `ctest --test-dir build --output-on-failure`: passed, 23/23 tests passing

## Issues Encountered
- The task packet examples referenced a stale FM latch model and example source paths. Resolved by following the authoritative `docs/audio_spec.md` register map and the repository's actual `emulator/include` and `emulator/src` layout.

## Recommended Next Step
Proceed to M26b and implement deterministic tone/noise generator state progression behind the existing APU register interface.
