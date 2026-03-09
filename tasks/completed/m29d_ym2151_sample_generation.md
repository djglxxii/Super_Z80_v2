# Task Packet

## Title

M29d — YM2151 FM Sample Generation

## Goal

Implement deterministic **FM sample generation** for the YM2151 using the internal operator/channel state model created in M29b and the timer infrastructure from M29c.

This milestone introduces **actual FM synthesis output**, but it remains **internal to the YM2151 device**.
No PSG mixing and no SDL audio playback are introduced yet.

---

# Scope

Included

* operator phase generation
* sine waveform lookup
* operator modulation chain
* channel algorithm routing
* envelope generator progression
* channel output accumulation
* deterministic mono sample generation

Excluded

* PSG + YM mixer
* SDL audio device output
* host-time pacing
* stereo panning behavior
* external audio buffering
* ROM/demo usage of FM

---

# Files Expected to Change

```
emulator/audio/ym2151/ym2151.h
emulator/audio/ym2151/ym2151.cpp
emulator/tests/unit/test_ym2151.cpp
docs/state_snapshot.md
```

---

# Files to Create

```
artifacts/reports/m29d_ym2151_sample_generation.md
```

---

# Constraints

* Output must be **deterministic** for identical tick sequences.
* No dependency on wall-clock time.
* Do not change PSG behavior.
* FM samples must remain **internal to the YM2151 device**.
* All synthesis must occur during the scheduler-driven `tick()` calls.
* Use fixed-point math where possible.

---

# Implementation Requirements

## 1. Sine Lookup Table

Create a deterministic sine table used by all operators.

Recommended size:

```
1024 entries
```

Range:

```
[-1.0, +1.0] scaled to int16
```

Example structure:

```cpp
static int16_t sine_table[1024];
```

Populate once during initialization.

---

# 2. Operator Phase Generation

Extend the operator state:

```
phase
phase_increment
```

Phase progression per tick:

```
phase += phase_increment
phase &= PHASE_MASK
```

Phase increment derived from:

```
frequency
block
multiple
```

Exact YM2151 frequency mapping is not required yet; use a deterministic placeholder formula consistent across runs.

---

# 3. Envelope Generator

Introduce basic envelope progression.

States:

```
ATTACK
DECAY
SUSTAIN
RELEASE
OFF
```

Operator state additions:

```
envelope_level
envelope_state
```

Minimal deterministic implementation is acceptable for this milestone.

Key-on behavior should transition operators to ATTACK state.

---

# 4. Operator Output

Compute operator output:

```
sample = sine_table[phase + modulation]
sample *= envelope_level
```

Operator output feeds the next operator depending on channel algorithm.

---

# 5. Channel Algorithms

Implement deterministic routing based on `channel.algorithm`.

Only a subset of routing is required initially, but the structure must support all algorithms.

Example routing:

```
op1 → op2 → op3 → op4 → output
```

Output accumulation per channel:

```
channel_output = final_operator_output
```

---

# 6. Channel Summation

Sum outputs of all active channels:

```
fm_sample = Σ(channel_output)
```

Clamp to int16 range.

---

# 7. Internal Sample Buffer

Store the generated sample internally.

Add device state:

```
int16_t last_sample;
```

Expose accessor:

```
int16_t YM2151::current_sample()
```

This will later be used by the mixer milestone.

---

# Scheduler Integration

Update the tick loop so synthesis advances deterministically.

Example:

```
for each cycle block
    update envelopes
    update phase
    compute operator output
    compute channel output
    update last_sample
```

Tick cadence must remain tied to the scheduler.

---

# Unit Test Requirements

Extend `test_ym2151.cpp`.

Tests required:

### 1. Deterministic Output

Run identical register setup and tick sequence twice.

Verify:

```
sample_sequence_run1 == sample_sequence_run2
```

### 2. Key-On Produces Output

After key-on register write and ticks:

```
current_sample() != 0
```

### 3. Channel Isolation

Key-on only channel 2.

Verify output changes only when that channel progresses.

### 4. Operator Disable

Key-off clears envelope progression.

Verify output decays toward zero.

---

# Documentation Update

Append milestone entry in:

```
docs/state_snapshot.md
```

Add:

```
M29d complete
YM2151 FM synthesis implemented
Internal deterministic sample generation available
No PSG/FM mixing yet
```

---

# Verification Commands

```
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure --tests-regex "super_z80_test_(ym2151|scheduler|bus)"
ctest --test-dir build --output-on-failure
```

---

# Deliverables

* deterministic YM2151 synthesis
* internal FM sample generation
* extended unit tests
* state snapshot update
* milestone report

Report file:

```
artifacts/reports/m29d_ym2151_sample_generation.md
```

---

# Completion Criteria

The milestone is complete when:

* FM synthesis produces deterministic samples
* key-on events generate audible internal output values
* synthesis remains scheduler-driven
* unit tests pass
* PSG behavior unchanged
* mixer not yet implemented
* milestone report written

---

# Next Milestone

```
M29e — Mixer Integration (PSG + YM2151)
```

This will combine PSG and FM output into a unified audio stream before SDL playback integration.
