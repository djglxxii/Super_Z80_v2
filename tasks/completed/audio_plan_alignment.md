# Task Packet

## Title

AUDIO_PLAN_ALIGNMENT — Record YM2151 as Planned Audio Expansion Without Rewriting Implemented Truth

## Goal

Update repository planning and documentation so the project explicitly distinguishes between:

* the **currently implemented and validated audio platform**
* the **intended target audio architecture** for future milestones

This task must **not** claim that YM2151 is already implemented.
It must instead record YM2151 as a **planned hardware expansion** beginning in a new milestone block after the current validated platform.

## Scope

Included:

* plan update
* state snapshot clarification
* hardware/doc clarification of implemented vs planned audio
* future milestone sequencing for YM2151 work
* milestone report

Excluded:

* emulator code changes
* changing the current validated PSG-only implementation
* claiming YM2151 ports/registers already exist in the emulator
* changing existing validation results
* altering current developer docs to falsely describe FM as implemented

## Files Expected to Change

* `docs/plan.md`
* `docs/state_snapshot.md`
* `docs/audio_spec.md`
* `docs/hardware_reference.md`
* `docs/programming_audio.md`
* `docs/developer_guide.md`

## Files to Create

* `artifacts/reports/audio_plan_alignment_report.md`

## Constraints

* Preserve repository truth.
* The docs must clearly distinguish:

  * **implemented/current**
  * **planned/future**
* Do not describe YM2151 behavior as available in the current emulator.
* Do not add fake port maps or programming instructions for current users as if FM already works.
* Do not modify emulator source code.
* Keep current PSG documentation intact.
* If any document currently states or implies that FM is implemented, correct it.

## Required Documentation Changes

### 1. `docs/plan.md`

Revise the plan so that after the currently completed milestones, a new future milestone block is added for FM expansion.

Add a new section after the current completed platform milestones:

```text
Future Audio Expansion — YM2151 FM Integration
```

Add milestones:

```text
M29a — YM2151 Register Interface
M29b — FM Operator and Channel State
M29c — YM2151 Timers and IRQ Behavior
M29d — FM Sample Generation
M29e — Mixer Integration (PSG + YM2151)
M29f — Deterministic FM Validation
```

Also add a short note that:

* the currently validated platform is PSG-only
* YM2151 is an approved planned expansion
* PCM remains intentionally excluded

### 2. `docs/state_snapshot.md`

Add a clear note stating:

```text
Current validated audio implementation:
- PSG-style APU only

Planned audio expansion:
- YM2151 FM subsystem
- scheduled for future M29 milestone block

PCM remains excluded from the platform design.
```

Do not mark YM2151 as implemented.

### 3. `docs/audio_spec.md`

Keep the current audio spec authoritative for the implemented PSG subsystem.

Append a clearly separated section titled something like:

```text
Planned Future Expansion: YM2151 FM Audio
```

That section must say:

* YM2151 is planned, not implemented
* this section is architectural intent only
* implementation will be defined and validated in M29 milestones
* current emulator behavior remains PSG-only

Do not define current active FM ports as if software can use them today.

### 4. `docs/hardware_reference.md`

Update the audio section so it explicitly distinguishes:

```text
Implemented hardware:
- PSG-style APU

Planned expansion:
- YM2151 FM synthesizer
```

Retain the statement that there is no implemented YM2151 or FM register block in the current hardware model, but add that FM is now an approved future milestone target.

### 5. `docs/programming_audio.md`

Keep the guide focused on implemented PSG behavior.

Add a short future note:

```text
Future FM support:
YM2151-based FM audio is planned but not yet available in the current emulator build.
Do not target FM hardware until the M29 milestone block is completed.
```

Do not add live programming examples for current users that imply FM works now.

### 6. `docs/developer_guide.md`

Update the project roadmap / platform summary to state:

```text
Current validated platform: PSG audio
Planned expansion: YM2151 FM audio in future milestones
```

Do not describe the current platform as already PSG+YM2151.

## Verification Commands

```bash
test -f docs/plan.md
test -f docs/state_snapshot.md
test -f docs/audio_spec.md
test -f docs/hardware_reference.md
test -f docs/programming_audio.md
test -f docs/developer_guide.md
```

Additionally verify by inspection that:

* current implementation is still described as PSG-only
* YM2151 appears only as planned/future work
* M29a–M29f appear in `docs/plan.md`

## Deliverables

Updated repository truth that:

* preserves the validated PSG-only current platform
* records YM2151 as an approved future expansion
* adds the future M29 FM milestone block
* does not falsely claim FM is already implemented

Report file:

```text
artifacts/reports/audio_plan_alignment_report.md
```

## Completion Criteria

The task is complete when:

* `docs/plan.md` includes the M29 FM milestone block
* current docs still truthfully describe the validated platform as PSG-only
* YM2151 is documented as planned future work only
* no emulator code changed
* report written

## Report Template

The completion report must include:

* summary
* files changed
* commands executed
* verification results
* issues encountered
* exact distinction added between implemented and planned audio
* recommended next step

## Recommended Next Step

After this documentation alignment:

```text
M29a — YM2151 Register Interface
```
