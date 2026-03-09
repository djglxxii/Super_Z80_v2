# AWOS Task Packet

## Title

AUDIO_ARCH_REVISION — Add YM2151 FM Audio to Platform Plan and Documentation

## Objective

Revise the repository’s **authoritative design documentation and milestone plan** so that the Super_Z80 platform audio system is formally defined as:

```
PSG (existing)
+
YM2151 FM synthesizer
```

The repository currently reflects a **PSG-only implementation**, but the intended hardware design includes **PSG + YM2151 FM audio** (with PCM intentionally excluded).

This task updates the **plan and documentation only** so future milestones can implement the YM2151 subsystem.

---

# Constraints

Strict rules:

1. **Do not modify emulator source code.**
2. **Do not change the PSG subsystem documentation except where necessary to describe coexistence with YM2151.**
3. **Do not remove existing audio milestones (M26).**
4. Add new FM milestones **after M28**.
5. Documentation must remain consistent with **period-correct hardware design goals**.
6. Do not invent speculative hardware not already discussed.

---

# Repository Files to Update

The following files must be reviewed and updated:

```
docs/plan.md
docs/hardware_reference.md
docs/programming_audio.md
docs/audio_spec.md
docs/developer_guide.md
docs/state_snapshot.md
```

If additional files reference the audio subsystem, update them as necessary.

---

# Required Documentation Changes

## 1. Update Platform Audio Description

Wherever the platform audio is described, change:

```
PSG-based APU
```

to

```
Dual audio subsystem:

• PSG-style tone/noise generator
• Yamaha YM2151 FM synthesizer
```

Clarify that:

```
PCM audio was intentionally removed from the design.
```

---

# 2. Update `docs/audio_spec.md`

Modify the audio specification so it describes **two hardware devices**.

New structure:

```
Audio Subsystem

1. PSG APU
   - ports 0xD0–0xDB
   - tone channels
   - noise channel

2. YM2151 FM synthesizer
   - ports 0x70–0x71
   - address/data register interface
   - 8 FM channels
```

Add section:

```
FM Audio (YM2151)

Communication model:
OUT (0x70) → register address
OUT (0x71) → register value
```

Clarify that:

```
YM2151 timing and synthesis are deterministic and scheduler-driven.
```

---

# 3. Update `docs/hardware_reference.md`

Add a section:

```
Audio Hardware

PSG
- deterministic tone/noise generator
- low-cost baseline audio

YM2151
- 8-channel FM synthesizer
- used in many arcade boards of the late 1980s
- provides complex music capability
```

Explain the combined mixer model:

```
final_sample = PSG_output + YM2151_output
```

---

# 4. Update `docs/programming_audio.md`

Add new sections:

```
Programming the YM2151
```

Describe:

* address/data register model
* channel structure
* operator configuration
* key-on/key-off behavior

Also include a short code example:

```
OUT (0x70), register
OUT (0x71), value
```

---

# 5. Update `docs/developer_guide.md`

Update the platform summary so the audio system is described as:

```
PSG + YM2151 FM audio
```

Also add a short description of why this combination was chosen:

```
PSG provides classic console-style sound effects,
while YM2151 enables complex arcade-style music.
```

---

# 6. Update `docs/plan.md`

Add a new milestone block after **M28**.

```
M29 – YM2151 FM Audio Subsystem
```

With sub-milestones:

```
M29a – YM2151 Register Interface
M29b – FM Operator and Channel State
M29c – YM2151 Timers and IRQ behavior
M29d – FM Sample Generation
M29e – Mixer Integration (PSG + YM2151)
M29f – Deterministic FM Validation
```

These milestones must follow the same deterministic testing model used for PSG audio.

---

# 7. Update `docs/state_snapshot.md`

Add an entry describing the architectural change:

```
Audio subsystem revised to include YM2151 FM synthesizer.
PSG implementation remains unchanged.
FM subsystem scheduled for milestone block M29.
```

---

# Verification Commands

Run these checks after edits:

```bash
test -f docs/plan.md
test -f docs/audio_spec.md
test -f docs/hardware_reference.md
test -f docs/programming_audio.md
test -f docs/developer_guide.md
test -f docs/state_snapshot.md
```

Ensure:

```
YM2151 appears in audio documentation
M29 milestone block exists in plan.md
```

---

# Deliverables

Updated documentation reflecting the **PSG + YM2151 architecture**.

Files modified:

```
docs/plan.md
docs/audio_spec.md
docs/hardware_reference.md
docs/programming_audio.md
docs/developer_guide.md
docs/state_snapshot.md
```

Report file:

```
artifacts/reports/audio_arch_revision_report.md
```

---

# Completion Criteria

The task is complete when:

* all documentation reflects **PSG + YM2151 audio hardware**
* `docs/plan.md` includes the **M29 milestone block**
* existing PSG documentation remains intact
* emulator code remains unchanged
* milestone report written

---

# Recommended Next Step

After this documentation revision:

```
M29a — YM2151 Register Interface
```

This milestone will implement the **FM chip communication path and register interface** in the emulator.
