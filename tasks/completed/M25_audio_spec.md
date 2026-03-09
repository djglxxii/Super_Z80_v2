# Task Packet

## Title

M25 — Audio System Contract Definition

## Goal

Define the **complete audio hardware contract** for the Super_Z80 platform and store it in repository truth before any audio implementation begins.

This milestone produces a deterministic specification describing the audio subsystem architecture, register map, timing interaction, and emulator integration rules.

No audio emulation code will be written in this milestone.

## Scope

Included:

* audio subsystem architecture specification
* register map definition
* channel model definition
* mixer design description
* scheduler interaction model
* bus / I-O integration contract
* SDL audio output strategy
* deterministic testing strategy

Excluded:

* audio synthesis implementation
* audio mixing code
* SDL audio device creation
* emulator runtime audio output
* ROM audio tests

## Files Expected to Change

* `docs/state_snapshot.md`

## Files to Create

* `docs/audio_spec.md`
* `artifacts/reports/M25_audio_spec_report.md`

## Constraints

* This milestone **defines the audio contract only**.
* No emulator logic should be implemented.
* The audio design must align with existing architecture boundaries.
* The design must integrate cleanly with:

  * Scheduler
  * Bus
  * EmulatorCoordinator
  * SDL shell
* Do not modify any completed VDP systems.
* Do not introduce new dependencies.
* If existing documentation already partially defines audio behavior, extend it rather than replacing it.

## Implementation Requirements

The audio specification document must include the following sections.

### 1. Audio Hardware Overview

Define:

* number of audio channels
* supported waveform types
* tone generation model
* noise generation model
* mixing behavior

### 2. Audio Register Map

Define the CPU-visible I/O ports controlling audio.

Include:

* frequency registers
* volume registers
* noise control
* global enable/mute

Registers must include:

```
port address
bit layout
read/write behavior
reset value
```

### 3. Channel Architecture

Define channel types:

Example structure:

* Tone channels
* Noise channel

Each must specify:

```
frequency control
volume control
phase generation
output sample behavior
```

### 4. Timing Model

Define how audio interacts with the scheduler.

Specify:

* sample rate
* update cadence
* frame boundaries
* determinism rules

Audio must **not own time**.

### 5. Mixer Design

Define:

* number of input channels
* mixing formula
* volume normalization
* clipping behavior
* output format

### 6. Emulator Integration

Define interaction with:

* Bus
* Scheduler
* Emulator core
* SDL output device

### 7. Deterministic Testing Strategy

Define how audio behavior can be tested without speakers.

Examples:

* sample buffer inspection
* frequency validation
* noise generator reproducibility

### 8. Future Extension Notes

Describe optional future extensions such as:

* FM synthesis
* PCM playback
* stereo mixing

These must be explicitly marked **non-baseline**.

## Verification Commands

```
test -f docs/audio_spec.md
test -f artifacts/reports/M25_audio_spec_report.md
```

## Deliverables

* authoritative audio hardware specification
* updated state snapshot
* milestone completion report

## Completion Criteria

The task is complete when:

* `docs/audio_spec.md` exists and fully defines the audio subsystem
* repository state snapshot references the new milestone
* completion report exists
* no emulator logic was modified

## Report Template

The completion report must include:

```
summary
files created
files modified
commands executed
verification results
issues encountered
recommended next step
```

## Recommended Next Step

**M26a — Audio Registers & State Implementation**
