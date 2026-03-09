# Task Packet

**Milestone:** M29f
**Title:** Deterministic Audio Validation Harness

---

# Goal

Implement deterministic validation coverage for the mixed audio path so the emulator can prove that:

* APU progression is deterministic
* YM2151 progression is deterministic
* mixer output is deterministic
* scheduler-owned audio stepping is deterministic
* emulator-level produced audio sample streams are repeatable across identical runs

This milestone is about **validation and test coverage only**. It does **not** add new audio features, new synthesis behavior, or SDL playback.

---

# Milestone Context

M29d established deterministic internal YM2151 sample generation.

M29e established deterministic platform mixing of:

* `APU` output
* `YM2151` output

The current audio pipeline is now:

```text
Scheduler
  ├─ APU.tick(cycles)
  ├─ YM2151.tick(cycles)
  └─ AudioMixer.tick()

EmulatorCore
  └─ consumes mixed scheduler-owned sample output
```

This milestone locks that behavior down with deterministic validation so future work cannot silently break the internal audio path.

---

# Scope

Implement a deterministic audio validation harness and supporting tests for:

1. repeated identical sample streams
2. cross-run repeatability
3. mixed-output stability under fixed scripted advancement
4. silence/mute edge cases where applicable
5. scheduler/audio integration correctness
6. regression protection for the current internal audio pipeline

This work must remain **fully internal** to the emulator test harness.

---

# Non-Goals

Do **not** implement:

* SDL audio playback
* host audio callbacks
* buffering or queues
* resampling
* volume balancing or normalization redesign
* chip-accurate YM2151 analog refinement
* new APU features
* new mixer features

This milestone validates existing behavior. It does not redesign it.

---

# Required Validation Strategy

The validation must prove determinism at multiple levels.

## Level 1 — Device Determinism

For fixed initial conditions and fixed cycle stepping:

* `APU` must produce the same sample sequence every run
* `YM2151` must produce the same sample sequence every run

## Level 2 — Mixer Determinism

For fixed device sample sequences:

* `AudioMixer` must produce the exact same mixed sequence every run

## Level 3 — Scheduler Integration Determinism

For fixed cycle schedules:

* the scheduler must advance audio devices and mixer in the same order every run
* resulting sample sequence must be identical every run

## Level 4 — Emulator-Level Determinism

For identical emulator setup and identical advancement:

* emulator-produced mixed audio output stream must be byte-for-byte identical across repeated runs in-process

---

# Architecture Requirements

Keep the current ownership model intact:

```text
Scheduler owns advancement
AudioMixer owns combining
EmulatorCore consumes mixed samples
Tests verify output
```

Do not move timing ownership out of the scheduler.

Do not introduce alternate timing paths.

Do not allow `EmulatorCore` to independently advance audio state.

---

# Implementation Requirements

## 1. Add Deterministic Audio Sequence Test Helpers

Create reusable test helpers for collecting a sequence of mixed samples from fixed scheduler stepping.

Suggested helper behavior:

* initialize device state
* apply known register writes / configuration
* advance by explicit fixed cycle counts
* collect `N` output samples into a vector
* compare vectors for exact equality

These helpers may live in test code only.

---

## 2. Add Mixed Output Regression Tests

Add tests that collect a deterministic stream of mixed samples and assert exact equality between two repeated runs.

The assertion must be **exact sequence equality**, not approximate equality.

Use integer sample vectors such as:

```cpp
std::vector<int16_t>
```

---

## 3. Add Silence / Baseline Cases

Where valid for current repository behavior, validate:

* both sources inactive => stable baseline output
* APU-only path => deterministic output
* YM2151-only path => deterministic output
* APU + YM2151 together => deterministic mixed output

Do not invent unsupported mute APIs. Use existing repository truth and current control/register behavior.

---

## 4. Add Fixed-Step Scheduler Validation

Explicitly test that a fixed sequence of scheduler cycle steps yields the same mixed audio stream across repeated runs.

Example pattern:

```text
step 64 cycles
capture sample
step 64 cycles
capture sample
...
```

The exact cycle value may differ if repository truth suggests a better existing scheduler cadence. Use current repository timing conventions.

---

## 5. Add EmulatorCore Mixed Output Validation

Add coverage that validates the current `EmulatorCore` audio-facing output path now emits deterministic mixed output rather than raw APU-only output.

This test should ensure:

* mixed samples are actually sourced from the scheduler-owned mixed path
* repeated identical runs produce identical results

Do not involve SDL or wall clock.

---

# Test Cases

Create or extend tests to cover at least the following.

## Test 1 — APU Deterministic Sequence

Given fixed APU initialization and fixed cycle stepping, two collected sample sequences must be exactly equal.

## Test 2 — YM2151 Deterministic Sequence

Given fixed YM2151 register state and fixed cycle stepping, two collected sample sequences must be exactly equal.

## Test 3 — Mixer Deterministic Sequence

Given identical APU and YM2151 source conditions, two mixed sample sequences must be exactly equal.

## Test 4 — APU-Only Mixed Path

With YM2151 inactive or effectively silent under current repo behavior, the mixed path must remain deterministic and stable.

## Test 5 — YM2151-Only Mixed Path

With APU inactive or effectively silent under current repo behavior, the mixed path must remain deterministic and stable.

## Test 6 — Combined Mixed Path

With both sources active, the mixed output sequence must be exactly reproducible.

## Test 7 — Scheduler Repeated Run Equality

Two independent scheduler-driven runs using identical cycle advancement must produce identical mixed sample vectors.

## Test 8 — EmulatorCore Output Path Determinism

Two emulator-core runs with identical setup must produce identical emitted mixed audio sample sequences.

---

# Preferred Test Organization

Use existing repository conventions. Likely files to create or extend:

```text
emulator/tests/unit/test_audio_determinism.cpp
emulator/tests/unit/test_audio_mixer.cpp
emulator/tests/unit/test_scheduler.cpp
emulator/tests/unit/test_ym2151.cpp
emulator/tests/integration/test_audio_output_integration.cpp
```

If a dedicated deterministic audio test file already exists, prefer extending it rather than fragmenting coverage.

---

# Files Expected to Change

Likely files:

```text
emulator/tests/unit/test_audio_determinism.cpp
emulator/tests/unit/test_audio_mixer.cpp
emulator/tests/unit/test_scheduler.cpp
emulator/tests/unit/test_ym2151.cpp
emulator/tests/integration/test_audio_output_integration.cpp
docs/state_snapshot.md
artifacts/reports/m29f_audio_deterministic_validation.md
```

Additional test helper files are acceptable only if justified and narrowly scoped.

Avoid broad production-code changes unless a small correction is necessary to make current deterministic behavior observable in tests.

---

# Production Code Constraints

Production code changes must be minimal.

Allowed:

* small testability hooks
* read-only accessors needed for deterministic validation
* narrow fixes required to expose current mixed output path correctly

Not allowed:

* redesigning scheduler ownership
* redesigning mixer behavior
* redesigning APU or YM2151 synthesis
* SDL integration
* introducing host-time concepts

---

# Verification Commands

Run these commands:

```bash
cmake -S . -B build
cmake --build build --target \
  super_z80_test_audio_mixer \
  super_z80_test_scheduler \
  super_z80_test_audio_output_integration \
  super_z80_test_audio_determinism \
  super_z80_test_ym2151

ctest --test-dir build --output-on-failure --tests-regex "super_z80_test_(audio_mixer|scheduler|audio_output_integration|audio_determinism|ym2151)"

cmake --build build
ctest --test-dir build --output-on-failure
```

Expected result:

```text
Focused audio validation tests pass
Full test suite passes
```

---

# Documentation Update

Update:

```text
docs/state_snapshot.md
```

Document that:

* mixed internal audio path now has deterministic validation coverage
* current audio pipeline is locked down for future SDL/output integration work
* next milestone is host playback integration, not internal audio redesign

---

# Completion Report

Create:

```text
artifacts/reports/m29f_audio_deterministic_validation.md
```

The report must include:

* summary of validation coverage added
* files changed
* commands run
* focused test results
* full-suite results
* any small production-code adjustments required for observability
* next-step recommendation

---

# Completion Criteria

This milestone is complete when:

* deterministic validation exists for APU, YM2151, mixer, scheduler path, and emulator output path
* repeated identical runs produce identical sample sequences
* focused audio tests pass
* full test suite passes
* `docs/state_snapshot.md` is updated
* the completion report is written

---

# Recommended Next Milestone After Completion

**M29g — SDL Audio Output Integration**

That milestone should connect the already-validated internal mixed sample stream to SDL playback without changing scheduler ownership or deterministic internal audio behavior.
