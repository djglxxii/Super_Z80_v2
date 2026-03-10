Proceed to **M39 – PSG Sound Effects**.

Save as:

```text
tasks/active/m39_psg_sound_effects.md
```

---

# Task Packet — M39 PSG Sound Effects

## Goal

Demonstrate the Super_Z80 **PSG audio subsystem** by adding simple sound effects to the Showcase ROM.

This milestone validates:

* PSG register usage
* tone generation
* triggered sound effects
* coexistence with the current scrolling / metasprite Showcase scene

The implementation must preserve deterministic headless behavior.

---

# Milestone

**M39 – PSG Sound Effects**

---

# Preflight Requirement

Before implementation begins, perform and record the required milestone preflight:

```text
TASK_PACKET_MILESTONE: M39 – PSG Sound Effects
NEXT_EXPECTED_MILESTONE_FROM_PLAN: M39 – PSG Sound Effects
ALIGNMENT: PASS
```

If this does not pass, stop immediately and do not edit files.

---

# Scope

Extend the current Showcase ROM so a controller button press triggers a short PSG sound effect.

The ROM should:

* initialize PSG state
* remain silent at boot unless input triggers audio
* play a short deterministic tone burst when a chosen button is pressed
* continue running the existing metasprite/parallax scene unchanged aside from the added SFX behavior

This milestone is about **PSG sound effects only**.

---

# Non-Goals

Do not implement:

* YM2151 music playback
* background music
* audio mixing between PSG and YM2151
* generalized sound engines
* music sequencing
* scene manager changes
* emulator architecture changes unless a real defect is discovered
* additional Showcase features unrelated to PSG SFX

---

# Repository Truth

Before editing, verify:

* `docs/plan.md` defines `M39 – PSG Sound Effects` as the next official Showcase milestone.
* The current Showcase ROM already includes:

  * parallax scrolling
  * animated metasprite
  * controller-driven movement
* Deterministic headless verification remains a project requirement.

---

# Files Expected to Change

Likely files:

* `rom/showcase/src/init.asm`
* `rom/showcase/src/main_loop.asm`
* `rom/showcase/inc/constants.inc`
* `rom/showcase/inc/memory_map.inc`
* `rom/showcase/inc/audio_helpers.inc` (only if needed)
* `docs/showcase_state_snapshot.md`
* `docs/state_snapshot.md`
* `artifacts/reports/m39_psg_sound_effects.md`

Modify other files only if required and explain why.

---

# Implementation Requirements

## 1. PSG Initialization

Initialize PSG audio state during ROM startup.

Requirements:

* channels start silent
* no unintended tone/noise at boot
* initialization is deterministic

Keep the setup minimal and obvious for developers reading the Showcase ROM.

---

## 2. Sound Effect Trigger

Use one controller button to trigger a short PSG sound effect.

Suggested behavior:

* button press writes PSG registers for a simple tone
* tone has clearly audible pitch
* tone duration is short and simple

A basic tone burst is sufficient. Do not build a general-purpose sound driver.

---

## 3. Trigger Behavior

The effect should be triggerable during runtime while the Showcase scene continues.

You may use edge-triggered or level-triggered behavior, but keep it simple and document it in comments and the report.

Preferred behavior:

* trigger on button press edge
* do not continuously restart every frame while held unless that is the intentionally chosen design

---

## 4. Preserve Existing Scene

Do not break:

* background parallax
* metasprite movement
* sprite animation
* controller-driven movement
* deterministic headless execution

The sound effect should be additive to the current Showcase scene.

---

## 5. Deterministic Headless Behavior

When running headless with no input:

* no sound effect should be triggered
* output must remain deterministic across repeated runs

This is required for automated verification.

---

## 6. Developer-Reference Clarity

The resulting code should be easy for ROM developers to study.

Prefer:

* simple PSG helper writes
* obvious trigger path
* obvious init path
* minimal indirection

Do not introduce speculative abstractions.

---

# Verification Commands

Run repository verification:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Build the Showcase ROM:

```bash
make -C rom/showcase clean
make -C rom/showcase
```

Run deterministic headless verification twice:

```bash
./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 24
./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 24
```

If you have an existing way to run the ROM interactively with SDL, use it to confirm the sound effect is audible on button press, and include the exact command used in the report.

---

# Acceptance Criteria

This milestone is complete only if:

* a controller button triggers an audible PSG sound effect
* the sound effect uses PSG register writes
* the current Showcase scene continues functioning
* repository build succeeds
* tests pass
* repeated headless runs remain deterministic
* `docs/showcase_state_snapshot.md` is updated
* a completion report is written

---

# Deliverables

* updated Showcase ROM with PSG sound effects
* completion report:

```text
artifacts/reports/m39_psg_sound_effects.md
```

---

# Completion Report Requirements

The report must include:

* milestone preflight result
* summary
* files changed
* commands run
* verification results
* issues discovered
* next-step recommendation

---

# Next Milestone

After successful completion of M39, proceed to:

**M40 – YM2151 Music Playback**
