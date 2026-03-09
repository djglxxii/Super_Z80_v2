# Task Packet

## Title

M29b — YM2151 Operator and Channel State Model

## Objective

Implement the **internal state model of the YM2151** so register writes begin modifying channel and operator structures.

This milestone **does not yet generate audio samples**.

Instead, it builds the **internal FM engine state representation** required for later synthesis milestones.

---

# Scope

Included

* YM2151 channel structures
* operator structures
* register decode logic
* mapping register writes to internal state
* deterministic channel tick progression

Excluded

* waveform generation
* envelope synthesis
* operator algorithms
* audio mixing
* timers
* IRQ behavior

---

# YM2151 Architecture Overview

The YM2151 consists of:

| Component             | Count |
| --------------------- | ----- |
| Channels              | 8     |
| Operators per channel | 4     |
| Total operators       | 32    |

Operator pipeline:

```text
Operator → Operator → Operator → Operator → Channel Output
```

Channels select from several **operator algorithms** controlling modulation paths.

---

# Internal Data Structures

## Operator

Create an operator structure similar to:

```cpp
struct YM2151Operator
{
    uint8_t detune;
    uint8_t multiple;

    uint8_t total_level;

    uint8_t attack_rate;
    uint8_t decay_rate;
    uint8_t sustain_rate;
    uint8_t release_rate;

    uint8_t sustain_level;

    uint8_t key_scale;

    bool key_on;
};
```

---

## Channel

Each channel contains four operators.

```cpp
struct YM2151Channel
{
    YM2151Operator operators[4];

    uint16_t frequency;
    uint8_t block;

    uint8_t algorithm;
    uint8_t feedback;
};
```

---

## Device State

Extend the existing device state:

```cpp
class YM2151
{
    uint8_t selected_register;

    uint8_t registers[256];

    YM2151Channel channels[8];
};
```

---

# Register Decode

Register writes must begin modifying internal structures.

Examples:

| Register Range | Meaning                      |
| -------------- | ---------------------------- |
| 0x20–0x27      | channel algorithm / feedback |
| 0x28–0x2F      | key on/off                   |
| 0x30–0x3F      | frequency data               |

For now:

* decode minimal fields
* update channel/operator state
* store register values

Example logic:

```cpp
if (reg >= 0x20 && reg <= 0x27)
{
    channel.algorithm = value & 0x07;
    channel.feedback  = (value >> 3) & 0x07;
}
```

---

# Key On Handling

When key-on bits are written:

```cpp
channel.operators[i].key_on = true
```

When cleared:

```cpp
channel.operators[i].key_on = false
```

No envelope behavior yet.

---

# Scheduler Tick

Expand the existing scheduler hook:

```cpp
void YM2151::tick(uint32_t cycles)
```

For now:

* iterate channels
* update placeholder phase counters

Example:

```cpp
for each channel
    for each operator
        if key_on
            advance phase counter
```

No synthesis yet.

---

# Logging

Optional debug output when:

* algorithm changes
* key-on events occur

Example:

```text
YM2151: CH3 KEY ON
YM2151: CH2 ALG=4 FB=2
```

---

# Tests

Extend the YM2151 unit test.

Test sequence:

```text
OUT 0x70, 0x20
OUT 0x71, 0x05
```

Verify:

```text
channels[0].algorithm == 5
```

Test key-on register:

```text
OUT 0x70, 0x28
OUT 0x71, 0xF0
```

Verify operators set `key_on`.

---

# Documentation Update

Update:

```text
docs/state_snapshot.md
```

Add entry:

```text
M29b complete
YM2151 internal channel/operator state model implemented
FM synthesis not yet implemented
```

---

# Deliverables

Source:

```
emulator/audio/ym2151/*
```

Tests:

```
emulator/tests/unit/test_ym2151.cpp
```

Documentation:

```
docs/state_snapshot.md
```

Report:

```
artifacts/reports/m29b_ym2151_channel_state.md
```

---

# Completion Criteria

The milestone is complete when:

* YM2151 register writes update internal state
* 8 channels with 4 operators exist
* key-on register affects operator state
* unit tests pass
* emulator still runs normally
* no audio output yet

---

# Next Milestone

After this milestone:

```
M29c — YM2151 Timers and IRQ Behavior
```

That milestone adds the **hardware timers used by the YM2151**.

---
