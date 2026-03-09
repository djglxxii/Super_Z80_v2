## Title

M26a — Audio Register State & Bus Semantics

## Goal

Implement the **bus-visible audio register storage and reset semantics** exactly as defined in `docs/audio_spec.md`, without implementing synthesis, mixing, or sound generation.

This milestone introduces the **internal APU register state model** so that CPU I/O operations behave correctly and deterministically.

## Scope

Included:

* audio register storage
* reset state initialization
* bus read/write behavior
* port decoding for audio subsystem
* register write validation
* register mirror rules (if defined in spec)

Excluded:

* PSG tone generation
* YM2151 synthesis
* mixer logic
* sample output
* SDL audio integration

The audio system remains **silent** after this milestone.

## Files Expected to Change

* `src/bus/Bus.cpp`
* `src/bus/Bus.h`
* `src/audio/APU.cpp`
* `src/audio/APU.h`

## Files to Create

* `src/audio/APURegisters.h`
* `artifacts/reports/M26a_audio_registers_report.md`

## Implementation Requirements

### 1. APU Core Object

Create a central audio subsystem object.

Example conceptual structure:

```
APU
 ├─ PSGRegisterState
 ├─ YM2151RegisterState
 └─ AudioControlState
```

The APU must not generate audio.

It only stores register state.

---

### 2. Register Storage

Implement storage structures matching `docs/audio_spec.md`.

Example:

```
struct PSGRegisterState
{
    uint8_t tone_period[3];
    uint8_t volume[4];
    uint8_t noise_control;
};
```

```
struct YM2151RegisterState
{
    uint8_t registers[256];
    uint8_t address_latch;
};
```

Exact register definitions must match the spec document.

---

### 3. Bus Port Integration

Add audio port handling in the bus.

Example conceptual logic:

```
switch(port)
{
    case 0xD0: APU.psgWrite(value); break;
    case 0xD1: APU.fmAddressWrite(value); break;
    case 0xD2: APU.fmDataWrite(value); break;
}
```

Do not implement synthesis behavior.

---

### 4. Address Latch Behavior

The FM interface must implement the two-step Yamaha register write model.

```
OUT (0xD1), register
OUT (0xD2), value
```

The register written by `0xD2` is determined by the current latch.

---

### 5. Reset Behavior

On system reset:

```
all PSG registers = 0
all FM registers = 0
address latch = 0
```

Reset behavior must match the spec.

---

### 6. Deterministic Behavior

Register writes must not depend on:

* host timing
* SDL
* audio buffers

They must depend **only on CPU I/O writes**.

---

## Verification

Basic validation tests:

```
OUT (0xD1), 0x20
OUT (0xD2), 0x7F
```

Expected:

```
APU.fmRegisters[0x20] == 0x7F
```

PSG example:

```
OUT (0xD0), 0x9F
```

Expected:

```
PSG register state updated
```

---

## Verification Commands

```
test -f src/audio/APU.cpp
test -f src/audio/APU.h
test -f src/audio/APURegisters.h
```

---

## Completion Criteria

The milestone is complete when:

* audio registers exist in emulator state
* bus writes update register state correctly
* reset initializes registers correctly
* no audio synthesis exists
* deterministic register behavior confirmed

---

## Report Template

The completion report must include:

```
summary
files created
files modified
verification commands
verification results
issues encountered
recommended next step
```

---

## Recommended Next Step

**M26b — PSG Tone Generator Implementation**

This milestone will implement the **PSG waveform generation** and integrate it with the scheduler-driven audio timing model.
