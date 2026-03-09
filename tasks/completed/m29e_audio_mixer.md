# Task Packet

**Milestone:** M29e
**Title:** Audio Mixer Integration (PSG + YM2151)

---

# Goal

Implement a deterministic **platform audio mixer** that combines the PSG and YM2151 internal audio outputs into a single mixed sample stream owned by the emulator scheduler.

The mixer must remain **entirely deterministic and scheduler-driven**, with no host-time coupling, buffering, or SDL audio playback in this milestone.

---

# Architectural Context

Current audio state:

| Device | Output                     |
| ------ | -------------------------- |
| PSG    | `int16_t current_sample()` |
| YM2151 | `int16_t current_sample()` |

Both devices already generate deterministic internal samples advanced via scheduler cycle input.

The mixer will combine these signals to produce the **platform audio sample**.

---

# Architecture

Create a new module:

```
emulator/audio/mixer/
```

Files:

```
emulator/audio/mixer/audio_mixer.h
emulator/audio/mixer/audio_mixer.cpp
```

---

# Class Design

### Header

```cpp
class AudioMixer
{
public:

    AudioMixer(PSG& psg, YM2151& ym);

    void tick();

    int16_t current_sample() const;

private:

    PSG& psg_;
    YM2151& ym_;

    int16_t last_sample_;
};
```

---

# Mixer Algorithm

Use deterministic integer additive mixing with clamping.

```
int32_t mixed = psg_sample + ym_sample;

if (mixed > 32767)
    mixed = 32767;

if (mixed < -32768)
    mixed = -32768;

last_sample_ = static_cast<int16_t>(mixed);
```

Floating point operations are **not permitted**.

---

# Scheduler Integration

Mixer execution must be controlled by the emulator scheduler.

Correct execution order:

```
scheduler_tick(cycles)

    psg.tick(cycles)

    ym2151.tick(cycles)

    audio_mixer.tick()
```

The mixer must **never advance independently of emulator cycles**.

---

# Functional Behavior

Mixer tick performs:

```
psg_sample = psg.current_sample()

ym_sample = ym2151.current_sample()

mixed = clamp(psg_sample + ym_sample)

store -> last_sample_
```

`current_sample()` returns the most recent mixed value.

---

# Determinism Rules

The mixer must:

* use integer arithmetic only
* avoid floating point
* avoid randomness
* avoid host timing
* avoid OS audio APIs
* produce identical results for identical inputs

---

# Tests

Create:

```
emulator/tests/unit/test_audio_mixer.cpp
```

Tests must verify deterministic mixing.

---

## Test 1 — Simple Sum

```
psg = 1000
ym = 2000
expected = 3000
```

---

## Test 2 — Negative Mixing

```
psg = -2000
ym = 500
expected = -1500
```

---

## Test 3 — Positive Clamp

```
psg = 30000
ym = 10000
expected = 32767
```

---

## Test 4 — Negative Clamp

```
psg = -30000
ym = -10000
expected = -32768
```

---

## Test 5 — Deterministic Repeatability

Repeated identical inputs must produce identical outputs.

---

# Files Expected to Change

New files:

```
emulator/audio/mixer/audio_mixer.h
emulator/audio/mixer/audio_mixer.cpp
emulator/tests/unit/test_audio_mixer.cpp
```

Possible integration updates:

```
emulator/audio/*
emulator/scheduler/*
docs/state_snapshot.md
```

Completion report:

```
artifacts/reports/m29e_audio_mixer.md
```

---

# Verification Commands

```
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Expected result:

```
All tests pass
```

---

# Deliverables

1. Deterministic audio mixer module
2. PSG + YM2151 mixed sample output
3. Unit tests validating mixing behavior
4. Updated `docs/state_snapshot.md`
5. Completion report:

```
artifacts/reports/m29e_audio_mixer.md
```

---

# Completion Criteria

This milestone is complete when:

* PSG and YM2151 samples are deterministically mixed
* unit tests pass
* the full test suite passes
* project state documentation is updated
* completion report is produced.
