# Task Packet — M43 Developer Reference ROM Finalization

## Goal

Finalize the Showcase ROM as the official **developer reference implementation** for the Super_Z80 platform.

This milestone must convert the completed Showcase ROM from “working demo” into a **clear, maintainable, documented reference ROM** for future game developers.

---

# Milestone

**M43 – Developer Reference ROM Finalization** 

---

# Preflight Requirement

Before implementation begins, perform and record the required milestone preflight:

```text
TASK_PACKET_MILESTONE: M43 – Developer Reference ROM Finalization
NEXT_EXPECTED_MILESTONE_FROM_PLAN: M43 – Developer Reference ROM Finalization
ALIGNMENT: PASS
```

If this does not pass, stop immediately and do not edit files.

---

# Scope

Perform the final cleanup and documentation pass for the Showcase ROM.

This milestone should:

* improve source clarity
* add explanatory comments where needed
* ensure the ROM structure is understandable
* create the programmer-facing reference guide

The Showcase ROM should remain functionally equivalent unless a small correction is required for clarity or correctness.

---

# Non-Goals

Do not implement:

* new hardware features
* new showcase scenes
* emulator architecture changes
* SDK redesign
* generalized engines or abstractions not already justified by the ROM
* plan rewrites unless explicitly required by repository truth

This is a **finalization and documentation milestone**, not a new feature milestone.

---

# Repository Truth

Before editing, verify:

* `docs/plan.md` defines `M43 – Developer Reference ROM Finalization` as the next official Showcase milestone. 
* The Showcase ROM already demonstrates:

  * text rendering
  * splash / initialization flow
  * scrolling
  * parallax
  * sprite rendering
  * sprite animation
  * metasprite construction
  * controller interaction enhancement
  * PSG sound effects
  * YM2151 music playback
  * simultaneous audio mixing
  * integrated showcase scene
* AWOS requires repository documentation to hold durable project truth and completion reports to capture final state. 

---

# Files Expected to Change

Likely files:

* `rom/showcase/src/*.asm`
* `rom/showcase/inc/*.inc`
* `rom/showcase/Makefile` (only if cleanup is needed)
* `docs/showcase_programmers_guide.md`
* `docs/showcase_state_snapshot.md`
* `docs/state_snapshot.md`
* `artifacts/reports/m43_developer_reference_rom_finalization.md`

Modify other files only if required and explain why.

---

# Implementation Requirements

## 1. Source Cleanup

Review the Showcase ROM source for developer-facing clarity.

Allowed cleanup:

* improve comments
* clarify labels / section headers
* make file responsibilities clearer
* remove obvious dead or confusing code if safe

Do **not** perform broad refactors without necessity.

---

## 2. Programmer’s Guide

Create:

```text
docs/showcase_programmers_guide.md
```

This document should explain how the Showcase ROM demonstrates the platform and where developers should look in the source tree.

At minimum, include sections for:

* purpose of the Showcase ROM
* repository location and build command
* ROM structure overview
* text rendering example
* background / scrolling example
* parallax example
* sprite and metasprite example
* controller input example
* PSG sound effect example
* YM2151 music example
* integrated scene overview
* deterministic verification command

---

## 3. Code-to-Guide Alignment

The guide must point to the actual files used by the ROM.

Prefer concrete references such as:

* `rom/showcase/src/text.asm`
* `rom/showcase/src/main_loop.asm`
* `rom/showcase/src/init.asm`
* `rom/showcase/src/vdp_init.asm`

Use the real repository structure, not abstract descriptions.

---

## 4. Preserve Deterministic Behavior

The finalization pass must not break determinism.

Repeated headless runs must still produce identical output.

---

## 5. Developer-Reference Quality

The final result should make it easy for a developer to answer questions like:

* How do I print text?
* How do I set up scrolling?
* How do I move a metasprite?
* How do I trigger a PSG sound effect?
* How do I start YM2151 playback?
* How do I verify my ROM deterministically?

That is the real acceptance target for this milestone.

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
./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 48
./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 48
```

---

# Acceptance Criteria

This milestone is complete only if:

* `docs/showcase_programmers_guide.md` exists
* the guide accurately describes the implemented Showcase ROM
* Showcase ROM source is cleaned up for readability
* the ROM still builds successfully
* repository tests pass
* repeated headless runs remain deterministic
* `docs/showcase_state_snapshot.md` is updated
* a completion report is written

---

# Deliverables

* finalized Showcase ROM source/comments
* programmer guide:

```text
docs/showcase_programmers_guide.md
```

* completion report:

```text
artifacts/reports/m43_developer_reference_rom_finalization.md
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
* final project-state recommendation

---

# Expected Result

After M43, the Showcase ROM should be considered:

* a complete platform demonstration
* a canonical example ROM for developers
* a deterministic validation workload for the emulator

This completes the planned Showcase ROM milestone sequence. 
