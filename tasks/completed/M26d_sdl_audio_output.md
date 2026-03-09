# Task Packet

## Title

M26d — SDL Audio Output Integration

## Goal

Integrate the existing deterministic internal APU sample stream with SDL real-time audio playback while preserving the repository’s audio contract:

* scheduler-owned timing remains authoritative
* host wall-clock time remains non-authoritative
* no changes to the defined register/generator/mixer behavior
* no speculative audio feature expansion

This milestone is about **playback integration only**.

## Scope

Included:

* SDL audio device initialization
* playback path from existing APU mixed sample output into SDL
* buffering/queueing strategy consistent with `docs/audio_spec.md`
* deterministic emulator-to-host audio handoff
* unit/integration coverage where practical
* milestone report
* state snapshot update

Excluded:

* changes to audio register map
* changes to tone/noise generation behavior
* changes to mixer formula
* FM/PCM feature expansion beyond current audio spec
* ROM content work
* platform-wide validation beyond audio playback hookup

## Files Expected to Change

* `emulator/src/main.cpp` or current SDL shell / windowed frontend file
* `emulator/include/emulator_core.h`
* `emulator/src/emulator_core.cpp`
* `emulator/include/apu.h`
* `emulator/src/apu.cpp`
* `emulator/CMakeLists.txt`
* `docs/state_snapshot.md`

## Files to Create

* `emulator/tests/unit/test_audio_output_integration.cpp`
* `artifacts/reports/M26d_sdl_audio_output_report.md`

## Constraints

* Follow `docs/audio_spec.md` as the authoritative audio contract.
* Do not change the semantics already implemented in M26a, M26b, or M26c.
* Do not make SDL callback timing authoritative over emulation timing.
* Do not make audio own time.
* Do not introduce new dependencies.
* Keep host playback integration narrowly scoped.
* Preserve deterministic headless behavior.
* If current code already contains partial SDL audio scaffolding, extend it rather than replacing it wholesale.
* If the repository truth is ambiguous, stop and report instead of inventing behavior.

## Implementation Requirements

### 1. SDL Audio Initialization

Add SDL audio device setup in the windowed/runtime shell.

The implementation must:

* initialize SDL audio cleanly
* request the format defined or permitted by `docs/audio_spec.md`
* handle device-open failure gracefully
* keep headless mode unaffected

### 2. Playback Integration

Connect the current APU mixed sample output to SDL playback.

The implementation must:

* use the existing internal sample-generation path from M26c
* move produced samples into SDL using the repository’s chosen strategy
* keep the emulator’s internal timing authoritative
* avoid generating audio directly from host callback pacing unless the callback only consumes already-produced samples

### 3. Buffering Rules

Implement a bounded, deterministic buffering strategy.

Requirements:

* no unbounded queue growth
* avoid crackle/underrun under normal operation
* avoid letting SDL timing redefine emulator timing
* preserve stable output under normal 60 Hz execution

### 4. Runtime Controls

Honor the current audio control state already defined in the APU.

Examples:

* global enable
* mute state
* silence after reset where applicable

### 5. Headless Preservation

Headless mode must remain deterministic and unchanged in behavior.

Specifically:

* no SDL audio dependency in headless execution
* no regression in current audio-related tests
* no change to existing deterministic sample generation APIs except what is strictly necessary for integration

### 6. Error Handling

If SDL audio cannot initialize:

* emulator must fail clearly in SDL mode, or
* cleanly continue with audio disabled if that behavior is already consistent with repository truth

Do not silently produce undefined runtime behavior.

## Verification Commands

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## Unit / Integration Test Requirements

Create practical coverage for at minimum:

1. **Initialization path**

   * audio output object/device path can be constructed or initialized without corrupting emulator state

2. **Sample handoff**

   * generated samples are handed to the SDL-facing output path in the correct format/size

3. **Muted / disabled state**

   * muted or disabled audio produces the correct silent playback payload behavior

4. **Headless isolation**

   * headless/test paths remain functional and do not require SDL playback

5. **Repeatability of produced sample blocks**

   * identical emulator state and stepping produce identical outgoing sample blocks prior to SDL consumption

Where direct SDL device assertions are impractical in CI, isolate and test the non-device portions of the output pipeline.

## Deliverables

* SDL audio playback integrated into the emulator runtime
* passing build and test suite
* updated `docs/state_snapshot.md`
* completion report at `artifacts/reports/M26d_sdl_audio_output_report.md`

## Completion Criteria

The task is complete when:

* build succeeds
* all tests pass
* SDL mode can play the APU’s generated audio
* headless mode remains unaffected
* no timing ownership moved from scheduler/emulator to SDL
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

M26e — Audio Deterministic Validation and Tests
