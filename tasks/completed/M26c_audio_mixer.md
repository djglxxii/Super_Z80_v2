# Task Packet

## Title

M26c — Audio Mixer and Sample Output State

## Goal

Implement deterministic audio mixing and internal sample-output state for the baseline Super_Z80 audio system using the existing APU register and generator model, without adding SDL playback or host-audio device integration.

This milestone must convert generator state into a deterministic mixed sample stream owned by the emulator, while preserving scheduler-owned timing.

## Scope

Included:

* per-channel amplitude evaluation
* baseline mixer behavior
* fixed volume lookup application
* deterministic sample generation API
* internal sample format/state
* unit tests for mixed output behavior
* milestone report
* state snapshot update

Excluded:

* SDL audio device creation
* wall-clock pacing
* host callback integration
* resampling
* platform validation
* ROM audio content
* non-baseline audio extensions

## Files Expected to Change

* `emulator/include/apu.h`
* `emulator/include/apu_registers.h`
* `emulator/src/apu.cpp`
* `emulator/CMakeLists.txt`
* `docs/state_snapshot.md`

## Files to Create

* `emulator/tests/unit/test_apu_mixer.cpp`
* `artifacts/reports/M26c_audio_mixer_report.md`

## Constraints

* Follow `docs/audio_spec.md` as the authoritative source.
* Do not invent mixer rules beyond the spec.
* Do not add SDL or host-clock dependencies.
* Do not make audio own time.
* Sample generation must be driven only by explicit advancement/sample requests.
* Keep implementation deterministic and unit-testable.
* Do not modify unrelated VDP, DMA, IRQ, controller, or scheduler behavior.
* If the audio spec is ambiguous on any baseline mixer rule, stop and report the ambiguity instead of inventing behavior.

## Implementation Requirements

### 1. Internal Sample Output Model

Extend the APU with internal sample output generation for the baseline audio contract.

The implementation must define and use a deterministic internal sample format exactly as specified in `docs/audio_spec.md`, for example:

* signed integer sample
* mono or stereo baseline, depending on spec
* fixed-width deterministic representation

No host playback is allowed in this milestone.

### 2. Explicit Sample Generation API

Add an explicit API for producing mixed samples from current generator state, conceptually equivalent to one or both of:

```text
generate_current_sample()
advance_and_generate_sample()
generate_samples(count)
```

The exact shape may follow the repository style, but it must:

* depend only on current APU state plus explicit tick/sample requests
* not depend on SDL callbacks
* not depend on wall-clock time

### 3. Volume Lookup Application

Apply the fixed volume behavior defined in `docs/audio_spec.md`.

This must include:

* correct lookup-table usage
* deterministic conversion from generator state to channel amplitude
* correct handling of mute/silence states
* correct handling of all baseline channels

### 4. Mixer Behavior

Implement the baseline mixer exactly as specified.

This must include:

* combining tone and noise channel contributions
* normalization/scaling behavior from the spec
* clipping or saturation behavior from the spec
* deterministic output for identical input state

### 5. Register and Generator Coupling

The mixed output must reflect:

* current APU register values from M26a
* current generator runtime state from M26b

Changing frequency, volume, or control state must affect future sample output correctly.

### 6. Silent/Muted Baseline Cases

Ensure deterministic behavior when:

* all channels are muted
* channels are disabled
* reset state is active

Silence must be represented consistently according to the audio spec.

### 7. No Host Output Yet

This milestone must not:

* open SDL audio devices
* push samples to SDL
* schedule real-time playback
* bind audio generation to host pacing

## Verification Commands

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## Unit Test Requirements

Create tests covering at minimum:

1. **Reset silence**

   * reset state produces the correct silent sample output

2. **Single tone channel contribution**

   * one enabled tone channel produces deterministic non-silent mixed output

3. **Volume effect**

   * changing volume register changes output amplitude exactly as expected

4. **Noise contribution**

   * noise channel affects mixed output deterministically

5. **Channel combination**

   * multiple active channels mix according to the baseline formula

6. **Clipping / saturation**

   * output near or at maximum behaves exactly as defined by the spec

7. **Repeatability**

   * same register state + same generator progression + same sample request sequence yields identical output

8. **Mute behavior**

   * muting channels restores correct silence or reduced amplitude

## Deliverables

* deterministic mixed sample generation in the APU
* passing unit tests
* updated `docs/state_snapshot.md`
* completion report at `artifacts/reports/M26c_audio_mixer_report.md`

## Completion Criteria

The task is complete when:

* build succeeds
* all tests pass
* APU can produce deterministic mixed output from internal state
* register writes and generator progression influence output correctly
* no SDL or host playback logic is added
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

M26d — SDL Audio Output Integration
