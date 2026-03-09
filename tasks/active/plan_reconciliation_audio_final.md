The agent is correct to stop.

The active packet is stale. Repository truth has moved past it:

* **M29d** is already implemented
* **M29e** is already implemented
* **M29f** is already implemented
* **M29g** is already implemented

So the reconciliation task now needs to document the **actual completed M29 block**, not an older intermediate state.

Use this replacement AWOS task packet.

---

# Task Packet

## Title

PLAN_RECONCILIATION_AUDIO_FINAL — Align M29 Audio Milestones With Implemented Repository Truth

## Goal

Update repository truth so the audio milestone sequence accurately reflects the **implemented and validated** state of the repository.

This task is **documentation-only**.

It must:

* reconcile the M29 milestone drift
* record the actual completed YM2151/FM/audio milestones
* distinguish hardware milestones from host integration milestones
* retain `docs/ym2151_reference.md` as the canonical FM implementation reference if it does not yet exist, or create it if missing

This task must **not modify emulator source code**.

---

## Scope

Included:

* update `docs/plan.md`
* update `docs/state_snapshot.md`
* create or update `docs/ym2151_reference.md`
* write completion report

Excluded:

* emulator code changes
* test changes
* new synthesis behavior
* bus changes
* SDL changes

---

## Files to Update

```text
docs/plan.md
docs/state_snapshot.md
```

## Files to Create or Update

```text
docs/ym2151_reference.md
artifacts/reports/plan_reconciliation_audio_final.md
```

---

## Constraints

* Preserve repository truth.
* Do not mark incomplete work as complete.
* Do not describe host integration as hardware behavior.
* Do not rewrite earlier milestone history; reconcile it.
* Documentation must reflect the actual codebase and tests.
* No emulator source code changes.

---

## Required Changes

### 1. Update `docs/plan.md`

Revise the M29 section so it reflects the actual completed milestone ordering.

Use this structure:

#### FM / Audio Hardware and Validation

```text
M29a — YM2151 Register Interface
M29b — YM2151 Operator and Channel State Model
M29c — YM2151 Timers, Status Bits, and IRQ Behavior
M29d — YM2151 FM Sample Generation
M29e — PSG + YM2151 Mixer Integration
M29f — Deterministic FM / Audio Validation
```

#### Host Platform Integration

```text
M29g — SDL Audio Output Integration
```

Add a short note:

```text
M29g is a host-platform milestone.
It exposes the already-produced emulator audio stream to SDL and does not redefine emulator hardware behavior.
```

Also retain:

```text
PCM remains excluded from the platform design.
```

Mark the full M29 block as completed if the repository truth supports that.

---

### 2. Update `docs/state_snapshot.md`

Add or revise the audio progress block so it records the actual completed state.

Record:

```text
Audio Progress

Completed:
- M26a — Audio Registers & State
- M26b — Audio Tone and Noise Generator State Progression
- M26c — Audio Mixer and Internal Sample Output State
- M26d — SDL Audio Output Integration
- M26e — Audio Deterministic Validation and Tests

YM2151 / FM Expansion Completed:
- M29a — YM2151 Register Interface
- M29b — YM2151 Operator and Channel State Model
- M29c — YM2151 Timers, Status Bits, and IRQ Behavior
- M29d — YM2151 FM Sample Generation
- M29e — PSG + YM2151 Mixer Integration
- M29f — Deterministic FM / Audio Validation
- M29g — SDL Audio Output Integration
```

Also clarify:

```text
M29g is host integration only and does not change emulator hardware semantics.
```

If there is overlap or redundancy between M26 and M29 host/output milestones, document that clearly rather than hiding it.

---

### 3. Create or Update `docs/ym2151_reference.md`

This document must be present and must reflect the now-implemented FM architecture.

If it does not exist, create it. If it exists, refine it.

It must include:

#### A. Purpose

```text
Authoritative YM2151 implementation and programming reference for the Super_Z80 project.
```

#### B. CPU Interface

```text
Port 0x70 — address register
Port 0x71 — data register
```

#### C. Internal Model

* 8 channels
* 4 operators per channel
* algorithms
* feedback
* phase progression
* timer A / timer B
* status bits
* IRQ-facing behavior

#### D. Register Categories

Group and describe:

* algorithm / feedback
* key on/off
* frequency / block
* operator parameter registers
* timer registers
* status / control behavior

#### E. FM Sample Generation

Document the implemented high-level behavior:

* operator phase advancement
* algorithm evaluation
* channel output generation
* deterministic scheduler-driven progression

#### F. Mixer Integration

Document:

* PSG output + YM2151 output
* deterministic mixed sample production

#### G. Determinism Requirements

Document:

* scheduler owns time
* SDL does not own time
* identical initial state + identical tick sequence must produce identical output

Do not invent features that are not present in code.

---

## Verification Commands

```bash
test -f docs/plan.md
test -f docs/state_snapshot.md
test -f docs/ym2151_reference.md
rg "M29a|M29b|M29c|M29d|M29e|M29f|M29g" docs/plan.md docs/state_snapshot.md
rg "YM2151|algorithm|operator|Timer A|Timer B|0x70|0x71|mixer|deterministic" docs/ym2151_reference.md
```

---

## Deliverables

Updated:

```text
docs/plan.md
docs/state_snapshot.md
```

Created or updated:

```text
docs/ym2151_reference.md
artifacts/reports/plan_reconciliation_audio_final.md
```

---

## Completion Criteria

The task is complete when:

* `docs/plan.md` reflects the actual completed M29 ordering
* `docs/state_snapshot.md` reflects actual implemented audio/FM progress
* `docs/ym2151_reference.md` exists and is implementation-useful
* no emulator code changed
* completion report written

---

## Report Template

The report must include:

* summary
* files changed
* files created
* commands executed
* verification results
* issues encountered
* corrected final M29 ordering
* note on hardware vs host-integration distinction
* recommended next step

---

## Recommended Next Step

After this reconciliation, stop authoring FM milestones and move to the next non-audio milestone in `docs/plan.md`.

---

This is the correct resolution to the blocker.
