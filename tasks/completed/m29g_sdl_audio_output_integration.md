# Task Packet

**Milestone:** M29g
**Title:** SDL Audio Output Integration

---

# Goal

Connect the already-validated internal mixed audio stream to SDL playback without changing internal timing ownership, synthesis behavior, or deterministic sample generation.

This milestone adds **host playback output only**. The emulator’s authoritative audio timing must remain owned by the emulator scheduler and internal sample production path.

---

# Milestone Context

Completed audio milestones:

* **M29d** — deterministic YM2151 internal sample generation
* **M29e** — deterministic audio mixer integration (`APU` + `YM2151`)
* **M29f** — deterministic audio validation harness

The internal audio pipeline is now:

```text
Scheduler
  ├─ APU.tick(cycles)
  ├─ YM2151.tick(cycles)
  └─ AudioMixer.tick()

EmulatorCore
  └─ produces scheduler-owned mixed int16 samples
```

This milestone must expose that mixed stream to SDL audio playback while preserving that architecture.

---

# Scope

Implement SDL audio output integration for the current mixed sample stream.

This includes:

1. SDL audio device initialization
2. a host-facing audio output path that consumes already-generated mixed samples
3. buffering between emulator-produced samples and SDL consumption
4. safe underflow handling
5. preservation of headless determinism and existing non-SDL behavior
6. test coverage for non-host logic where practical

---

# Non-Goals

Do **not** implement:

* changes to APU synthesis behavior
* changes to YM2151 synthesis behavior
* changes to mixer math
* host-driven emulator timing
* resampling redesign unless absolutely required by existing repository structure
* advanced audio latency tuning
* dynamic rate matching
* audio normalization redesign
* stereo expansion
* SDL ownership of scheduler progression

This milestone is output integration only.

---

# Architectural Rules

## 1. Scheduler Remains Authoritative

The scheduler must continue to own advancement of:

* `APU`
* `YM2151`
* `AudioMixer`

SDL must **not** drive audio generation timing.

Forbidden model:

```text
SDL callback asks emulator to generate samples on demand
```

Required model:

```text
Emulator generates samples under scheduler ownership
↓
samples are appended to an output buffer/queue
↓
SDL consumes buffered samples
```

---

## 2. EmulatorCore Remains the Source of Mixed Samples

SDL output must consume the same mixed sample stream already validated in M29f.

It must not bypass:

* `AudioMixer`
* `Scheduler`
* `EmulatorCore` mixed output path

---

## 3. Headless Behavior Must Remain Unchanged

Headless mode must remain deterministic and must not depend on SDL audio.

Do not introduce SDL requirements into headless execution or deterministic test paths.

---

# Required Design

## Audio Output Model

Implement a narrow SDL-facing audio output component that:

* accepts scheduler-produced `int16_t` mixed samples
* stores them in a FIFO/ring buffer
* feeds them to SDL when requested

Suggested location:

```text
emulator/src/sdl_audio_output.h
emulator/src/sdl_audio_output.cpp
```

or existing SDL shell location if repository structure already centralizes SDL-specific code there.

Use repository truth for exact placement.

---

## Buffering Requirements

A small audio buffer is required because SDL consumes samples asynchronously relative to the main emulation loop.

Requirements:

* buffer contains already-generated mixed samples only
* integer sample format
* deterministic write order from emulator side
* safe handling when SDL requests more samples than are available

Underflow behavior must be explicit and simple:

```text
output silence when buffer is empty
```

That is acceptable for this milestone.

Do not attempt sophisticated recovery logic.

---

## Sample Format

Use the repository’s current mixed output format:

```text
signed 16-bit integer sample
```

If SDL configuration requires a specific explicit format, configure it to match the internal sample representation as directly as possible.

Prefer:

```text
AUDIO_S16SYS
```

unless repository truth already uses a different compatible SDL signed 16-bit format.

---

## Channel Count

Unless the repository already has a conflicting SDL audio convention, use:

```text
mono output
```

because the current internal mixed path is a single mixed sample stream.

Do not invent stereo duplication logic unless required by existing SDL infrastructure. If SDL requires two-channel output in repository truth, duplicate the mono sample identically into both channels and document it clearly.

Default expectation for this milestone is **mono**.

---

## Sample Rate

Use the repository’s current internal audio output cadence if already defined.

If the SDL device requires explicit sample-rate configuration and the repository does not yet define one, add the smallest clear constant necessary in the SDL-facing layer and document it.

Do not redesign scheduler timing to match SDL. SDL configuration must adapt around the existing emulator-owned production path.

---

# Implementation Requirements

## 1. SDL Audio Device Initialization

Add SDL audio device open/close lifecycle in the SDL runner path.

Responsibilities:

* initialize SDL audio subsystem if not already done
* open an audio device with the selected format
* start playback
* cleanly shut down on exit

Keep SDL audio code isolated to SDL/frontend-specific code.

---

## 2. Audio Sample Queue / Ring Buffer

Implement a bounded queue or ring buffer for mixed `int16_t` samples.

Requirements:

* emulator main loop appends mixed samples after they are generated
* SDL callback/device-consumption path reads queued samples
* preserve FIFO ordering
* if empty, emit silence
* if full, use a simple explicit policy

Preferred full-buffer policy:

```text
drop oldest unread samples only if necessary
```

or

```text
stop appending new samples when full
```

Choose the simpler option that best matches current repository structure, but document it in the report.

Do not let buffer logic mutate emulation timing.

---

## 3. Main Loop Integration

In SDL/windowed execution path:

* continue running emulator normally
* after scheduler-owned mixed samples are produced, append them to the SDL audio output buffer
* do not generate audio from the SDL callback itself

This is the critical architecture rule for the milestone.

---

## 4. Silence Handling

If the SDL device requests samples while the buffer is empty:

* output zeros
* do not advance emulator state
* do not attempt to synthesize missing samples inside the callback

---

## 5. Logging / Debugging

Only add minimal logging if the repository already has a debug/log convention for SDL/audio.

Avoid noisy runtime logging.

If a debug counter for underflows exists or can be added narrowly without polluting production paths, that is acceptable.

---

# Tests

Because SDL host playback itself is not ideal for deterministic automated tests, focus tests on the **non-host logic** introduced for playback support.

Add tests where practical for:

1. FIFO/ring-buffer ordering
2. silence-on-underflow behavior
3. sample write/read correctness
4. mono sample preservation through the output buffer path
5. preservation of existing deterministic non-SDL tests

---

# Preferred Test Coverage

Likely files to create or extend:

```text
emulator/tests/unit/test_sdl_audio_output.cpp
emulator/tests/unit/test_audio_output_integration.cpp
emulator/tests/unit/test_audio_determinism.cpp
docs/state_snapshot.md
artifacts/reports/m29g_sdl_audio_output_integration.md
```

Use repository truth for actual naming and existing test layout.

If SDL-specific code is difficult to unit-test directly, isolate the buffer logic into testable non-SDL code and test that piece thoroughly.

---

# Files Expected to Change

Likely files:

```text
emulator/src/sdl_audio_output.cpp
emulator/src/sdl_audio_output.h
emulator/src/sdl_shell.cpp
emulator/src/emulator_core.cpp
emulator/include/emulator_core.h
emulator/tests/unit/test_sdl_audio_output.cpp
emulator/tests/unit/test_audio_output_integration.cpp
docs/state_snapshot.md
artifacts/reports/m29g_sdl_audio_output_integration.md
```

Adjust file paths to repository truth if SDL/frontend code already lives elsewhere.

---

# Production Code Constraints

Allowed:

* SDL output device setup
* bounded audio queue/ring buffer
* narrow read/write buffer helpers
* narrow emulator-to-SDL handoff plumbing
* small observability hooks if required for testing

Not allowed:

* moving audio generation into SDL callback
* redesigning scheduler ownership
* changing synthesis semantics
* changing mixer semantics
* changing headless behavior
* introducing host-clock authority over emulation

---

# Verification Commands

Run these commands:

```bash
cmake -S . -B build
cmake --build build --target \
  super_z80_test_audio_output_integration \
  super_z80_test_audio_determinism \
  super_z80_test_scheduler

ctest --test-dir build --output-on-failure --tests-regex "super_z80_test_(audio_output_integration|audio_determinism|scheduler)"

cmake --build build
ctest --test-dir build --output-on-failure
```

If the repository contains a specific SDL/audio-output unit target, include it in the focused build/test run as well.

Expected result:

```text
Focused SDL/audio-output-related tests pass
Full test suite passes
```

---

# Documentation Update

Update:

```text
docs/state_snapshot.md
```

Document that:

* the internal mixed audio stream is now connected to SDL playback
* scheduler ownership of timing remains unchanged
* SDL consumes buffered mixed samples only
* headless deterministic behavior remains unchanged

---

# Completion Report

Create:

```text
artifacts/reports/m29g_sdl_audio_output_integration.md
```

The report must include:

* summary of SDL playback integration
* exact files changed
* commands run
* focused test results
* full-suite results
* buffering policy used
* underflow handling behavior
* confirmation that scheduler ownership was preserved
* confirmation that headless behavior was not changed
* next-step recommendation

---

# Completion Criteria

This milestone is complete when:

* SDL playback consumes the scheduler-owned mixed sample stream
* SDL does not drive emulator timing
* a bounded output buffer/queue exists
* underflow produces silence
* focused related tests pass
* full test suite passes
* `docs/state_snapshot.md` is updated
* the completion report is written

---

# Recommended Next Milestone After Completion

**M29h — Audio Playback Stabilization and Validation**

That milestone should tighten the host playback path with practical validation around:

* buffer stability
* underflow/overflow behavior
* sample-rate configuration sanity
* any narrow cleanup needed after first real SDL playback integration

It should still avoid redesigning the internal synthesis and scheduler-owned audio architecture.
