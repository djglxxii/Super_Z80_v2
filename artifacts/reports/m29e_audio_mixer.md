# Task Completion Report

## Title
M29e - Audio Mixer Integration (PSG + YM2151)

## Status
Completed

## Summary
Implemented a deterministic platform audio mixer that combines the existing PSG-style `APU` output and the YM2151 internal sample into a single scheduler-owned mixed `int16_t` stream. The mixer uses integer additive mixing with signed 16-bit clamping only. `EmulatorCore` now consumes scheduler-owned mixed samples for the existing headless/SDL-facing output path. The task packet referenced a `PSG` class, but repository truth uses the existing `APU` for the PSG path, so the mixer was integrated against `APU` without changing device behavior.

## Files Changed
- `emulator/CMakeLists.txt`
- `emulator/audio/mixer/audio_mixer.h`
- `emulator/audio/mixer/audio_mixer.cpp`
- `emulator/audio/ym2151/ym2151.h`
- `emulator/include/apu.h`
- `emulator/include/scheduler.h`
- `emulator/src/apu.cpp`
- `emulator/src/emulator_core.cpp`
- `emulator/src/scheduler.cpp`
- `emulator/tests/integration/test_cpu_dma_irq_integration.cpp`
- `emulator/tests/integration/test_vdp_vblank_irq.cpp`
- `emulator/tests/unit/test_audio_mixer.cpp`
- `emulator/tests/unit/test_dma.cpp`
- `emulator/tests/unit/test_scheduler.cpp`
- `emulator/tests/unit/test_ym2151.cpp`
- `docs/state_snapshot.md`
- `artifacts/reports/m29e_audio_mixer.md`

## Commands Run
- `cmake -S . -B build`
- `cmake --build build --target super_z80_test_audio_mixer super_z80_test_scheduler super_z80_test_audio_output_integration super_z80_test_audio_determinism super_z80_test_ym2151`
- `ctest --test-dir build --output-on-failure --tests-regex "super_z80_test_(audio_mixer|scheduler|audio_output_integration|audio_determinism|ym2151)"`
- `cmake --build build`
- `ctest --test-dir build --output-on-failure`

## Verification Results
- Focused mixer/audio/scheduler/YM2151 regression run passed: `5/5` tests.
- Full deterministic `ctest --test-dir build --output-on-failure` suite passed.

## Mixer Behavior Implemented
- Added `AudioMixer` as a deterministic integer-only additive mixer with signed 16-bit clamp behavior.
- Exposed the APU’s current sample as a read-only sample source and treated both `APU` and `YM2151` as mixer inputs.
- Extended the scheduler with explicit audio-sample stepping so it owns PSG advancement plus mixer updates, while scanline stepping continues to own YM2151 advancement.
- Switched `EmulatorCore` sample production to push scheduler-owned mixed samples instead of raw PSG output.
- Added direct unit coverage for sum, negative mixing, positive clamp, negative clamp, and repeatability.

## Next-Step Recommendations
- Execute `M29f` to broaden deterministic validation around the mixed PSG plus YM2151 output path.
