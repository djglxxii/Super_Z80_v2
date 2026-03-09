# Task Packet

## Title

M26b — Audio Tone and Noise Generator State Progression

## Goal

Implement deterministic internal audio state progression for the baseline Super_Z80 audio system behind the existing APU register interface, without adding mixer output or SDL playback.

This milestone must make the APU capable of advancing tone/noise generator state according to the scheduler-owned timing model defined in `docs/audio_spec.md`.

## Scope

Included:

* tone generator state progression
* noise generator state progression
* phase/counter advancement
* register-to-generator coupling
* deterministic per-step advancement API
* unit tests for generator evolution
* milestone report
* state snapshot update

Excluded:

* sample mixing
* host audio output
* SDL audio device work
* FM synthesis if not defined as baseline in `docs/audio_spec.md`
* platform validation
* ROM audio content

## Files Expected to Change

* `emulator/include/apu.h`
* `emulator/include/apu_registers.h`
* `emulator/src/apu.cpp`
* `emulator/CMakeLists.txt`
* `docs/state_snapshot.md`

## Files to Create

* `emulator/tests/unit/test_apu_generators.cpp`
* `artifacts/reports/M26b_audio_generators_report.md`

## Constraints

* Follow `docs/audio_spec.md` as the authoritative source.
* Do not invent register meanings beyond the spec.
* Do not add SDL or host-clock dependencies.
* Do not make audio own time; progression must be driven by an explicit advancement call.
* Keep the implementation deterministic and unit-testable.
* Do not modify unrelated VDP, DMA, IRQ, or controller code.
* If the spec leaves a behavior ambiguous, stop and report the ambiguity rather than silently inventing behavior.

## Implementation Requirements

### 1. Generator State Model

Extend the silent APU to include internal runtime state for the baseline generators defined in `docs/audio_spec.md`.

Examples of acceptable internal state categories:

* tone period counters
* tone phase bits
* noise period counter
* noise shift register / LFSR state
* channel enabled / muted state if defined by spec

The exact set must match the audio spec.

### 2. Explicit Time Advancement API

Add an explicit progression entry point on the APU, for example a function conceptually equivalent to:

```text
advance_audio_ticks(count)
```

This function must:

* advance generator state deterministically
* consume only explicit tick counts passed in
* not query wall-clock time
* not depend on SDL callback timing

### 3. Tone Progression

Implement deterministic progression for all baseline tone channels defined in the spec.

Behavior must include:

* period/countdown handling
* phase toggling at the correct interval
* reset behavior from current register state
* stable behavior for boundary values

### 4. Noise Progression

Implement deterministic progression for the baseline noise channel defined in the spec.

Behavior must include:

* countdown handling
* LFSR or equivalent noise-state evolution exactly as defined in the spec
* deterministic reset seed/state
* correct coupling to control registers

### 5. Register Coupling

Changes already written into APU registers in M26a must affect progression correctly.

Examples:

* changing period registers changes future tone stepping
* changing noise control changes future noise stepping
* reset reinitializes progression state consistently

### 6. No Output Mixing Yet

This milestone must not:

* produce host audio
* create PCM sample buffers for playback
* implement final mixer behavior

Internal observable state for tests is sufficient.

## Verification Commands

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## Unit Test Requirements

Create tests covering at minimum:

1. **Reset state**

   * generator runtime state initializes exactly as defined

2. **Tone counter progression**

   * a tone channel advances predictably over a known number of ticks

3. **Tone phase toggle**

   * phase/output bit toggles at the expected cadence

4. **Noise progression**

   * noise state evolves deterministically over known tick counts

5. **Register update effect**

   * changing period/control registers changes subsequent progression as expected

6. **Repeatability**

   * same initial register state + same tick sequence yields identical final runtime state

7. **Boundary behavior**

   * minimum/maximum legal register values do not produce undefined behavior

## Deliverables

* deterministic tone/noise generator progression in the APU
* passing unit tests
* updated `docs/state_snapshot.md`
* completion report at `artifacts/reports/M26b_audio_generators_report.md`

## Completion Criteria

The task is complete when:

* build succeeds
* all tests pass
* APU generator state advances deterministically through an explicit API
* register writes from M26a influence progression correctly
* no SDL or mixer/output logic is added
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

M26c — Audio Mixer and Sample Output State
