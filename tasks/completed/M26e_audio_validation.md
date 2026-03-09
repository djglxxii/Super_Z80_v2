# Task Packet

## Title

M26e — Audio Deterministic Validation and Tests

## Goal

Establish a comprehensive deterministic validation layer for the Super_Z80 audio subsystem. This milestone verifies that the full audio pipeline—from register writes through generator progression, mixing, and SDL output handoff—behaves deterministically and conforms to the audio contract defined in `docs/audio_spec.md`.

This milestone adds **tests and validation only**. It must not change audio behavior.

## Scope

Included:

* deterministic unit tests for audio registers, generators, mixer, and output path
* repeatability validation across identical execution runs
* deterministic sample sequence verification
* audio boundary condition tests
* headless validation tests
* milestone report
* repository state snapshot update

Excluded:

* changes to APU behavior
* new audio features
* changes to the mixer formula
* changes to generator logic
* changes to SDL integration behavior
* ROM audio content

## Files Expected to Change

* `emulator/CMakeLists.txt`
* `docs/state_snapshot.md`

## Files to Create

* `emulator/tests/unit/test_audio_determinism.cpp`
* `emulator/tests/unit/test_audio_register_behavior.cpp`
* `emulator/tests/unit/test_audio_mixing_behavior.cpp`
* `emulator/tests/unit/test_audio_noise_behavior.cpp`
* `artifacts/reports/M26e_audio_validation_report.md`

## Constraints

* `docs/audio_spec.md` remains the authoritative audio contract.
* Do not modify implementation logic to “force” tests to pass.
* Tests must validate existing behavior, not redefine it.
* Tests must be deterministic and host-independent.
* SDL device access must not be required for validation.
* Tests must run successfully under CI environments and dummy SDL drivers.
* The milestone must not introduce runtime performance regressions.

## Implementation Requirements

### 1. Register Validation Tests

Create tests verifying the CPU-visible audio register behavior.

Test cases must include:

* register write/readback correctness
* masking of undefined bits
* reset-state correctness
* global audio enable and mute handling
* behavior when writing boundary register values

These tests confirm that the register interface implemented in **M26a** remains correct.

---

### 2. Generator Determinism Tests

Create deterministic tests validating the generator state progression implemented in **M26b**.

Tests must verify:

* tone generator frequency progression
* tone phase toggle cadence
* noise generator LFSR evolution
* periodic noise reseed behavior
* deterministic state evolution across identical tick sequences

Two independent runs of identical advancement sequences must produce identical internal generator states.

---

### 3. Mixer Behavior Tests

Validate the mixer logic implemented in **M26c**.

Test coverage must include:

* single channel tone output
* multiple channel mixing
* noise contribution
* correct application of the fixed volume lookup table
* clipping or saturation behavior defined in the audio spec
* mute behavior
* silence when all channels disabled

Expected sample values must match deterministic calculations.

---

### 4. Sample Sequence Repeatability

Create tests verifying repeatability of generated sample sequences.

Procedure:

1. initialize identical emulator state
2. perform identical register writes
3. advance identical tick/sample sequences
4. capture generated sample blocks
5. compare sequences

The output sequences must match exactly.

---

### 5. Headless Output Validation

Validate that the non-SDL audio path functions correctly.

Tests must verify:

* sample generation without SDL initialization
* correct buffer handoff behavior
* no dependency on SDL audio device

This ensures deterministic CI behavior.

---

### 6. SDL Output Boundary Tests

Where possible without opening a real device, validate:

* sample buffer formatting
* correct PCM format
* correct sample sizes
* correct queueing interface usage

Tests should rely on SDL dummy drivers when SDL functionality is required.

---

## Verification Commands

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Optional runtime validation:

```bash
env SDL_AUDIODRIVER=dummy SDL_VIDEODRIVER=dummy ./build/super_z80 --sdl-audio
```

## Deliverables

* expanded deterministic audio test coverage
* passing build and test suite
* updated `docs/state_snapshot.md`
* completion report at `artifacts/reports/M26e_audio_validation_report.md`

## Completion Criteria

The milestone is complete when:

* build succeeds
* all tests pass
* audio behavior is verified to be deterministic
* register, generator, mixer, and output paths are validated
* no functional changes were introduced to the audio implementation
* milestone report is written
* state snapshot is updated

## Report Template

The completion report must include:

* summary
* files changed
* files created
* commands executed
* verification results
* issues encountered
* recommended next step

## Recommended Next Step

Proceed to the next subsystem milestone defined in `docs/plan.md`, continuing emulator completion beyond the audio subsystem.
