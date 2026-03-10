# Task Packet — M40 YM2151 Music Playback

## Goal

Demonstrate **YM2151 FM music playback** in the Showcase ROM.

This milestone validates:

* YM2151 initialization
* register writes from ROM code
* simple continuous FM playback
* coexistence with the existing Showcase scene and PSG sound-effects milestone

The implementation must preserve deterministic headless behavior and keep the ROM understandable as a developer reference. The current plan defines M40 specifically as simple FM music playback with continuous playback. 

## Milestone

**M40 – YM2151 Music Playback** 

## Preflight Requirement

Before implementation begins, perform and record the required milestone preflight:

```text
TASK_PACKET_MILESTONE: M40 – YM2151 Music Playback
NEXT_EXPECTED_MILESTONE_FROM_PLAN: M40 – YM2151 Music Playback
ALIGNMENT: PASS
```

If this does not pass, stop immediately and do not edit files.

## Scope

Extend the current Showcase ROM so it starts a simple YM2151 music pattern and keeps it playing continuously during the existing scene.

The ROM should:

* initialize YM2151 state at startup
* program a minimal FM instrument/voice setup
* start a simple repeating note pattern or sustained musical phrase
* continue running the existing parallax / metasprite / controller / PSG-SFX scene

This milestone is about **FM music playback only**.

## Non-Goals

Do not implement:

* PSG + YM mixing logic beyond whatever already exists in the platform
* complex music engines or trackers
* song scripting frameworks
* advanced instrument banks
* scene manager changes
* new emulator architecture unless a real defect is discovered
* additional Showcase features unrelated to FM music playback

## Repository Truth

Before editing, verify:

* `docs/plan.md` defines `M40 – YM2151 Music Playback` as the next official Showcase milestone after M39. 
* The emulator platform already contains completed YM2151 implementation and deterministic validation in Phase 3, so this milestone is using existing platform capability rather than inventing new audio hardware behavior. 
* Deterministic headless verification remains a project requirement.

## Files Expected to Change

Likely files:

* `rom/showcase/src/init.asm`
* `rom/showcase/src/main_loop.asm`
* `rom/showcase/inc/constants.inc`
* `rom/showcase/inc/memory_map.inc`
* `rom/showcase/inc/audio_helpers.inc` (only if needed)
* `docs/showcase_state_snapshot.md`
* `docs/state_snapshot.md`
* `artifacts/reports/m40_ym2151_music_playback.md`

Modify other files only if required and explain why.

## Implementation Requirements

## 1. YM2151 Initialization

Initialize the YM2151 during ROM startup.

Requirements:

* chip enters a known state
* register writes are explicit and deterministic
* initialization is readable as a reference example

Keep the setup minimal and obvious for ROM developers.

## 2. Continuous FM Playback

Program a simple FM playback example.

Acceptable examples:

* a sustained repeating tone
* a short looped melodic phrase
* a repeating two- or four-note pattern

The pattern should be simple enough to understand from the source.

## 3. Minimal Register Interface Usage

Use direct YM2151 register writes appropriate to the existing hardware contract.

Prefer:

* small number of registers
* clear comments
* explicit sequencing

Do not build a generalized music engine.

## 4. Preserve Existing Scene

Do not break:

* parallax scrolling
* metasprite movement
* sprite animation
* controller-driven movement
* PSG sound-effect trigger path

The FM playback should be additive to the current Showcase scene.

## 5. Deterministic Headless Behavior

Headless execution must remain deterministic.

Even if the headless result includes changing audio output due to FM playback, repeated runs under the same command must produce identical output.

## 6. Developer-Reference Clarity

The resulting code should be easy for ROM developers to study.

Prefer:

* obvious init path
* obvious music-start path
* obvious loop/update path if periodic note changes are used
* minimal indirection

Avoid speculative abstractions.

## Verification Commands

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
./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 32
./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 32
```

If you have a repository-supported way to run the ROM interactively with SDL audio for external ROM execution, use it and include the exact command in the report. If not, note that limitation explicitly.

## Acceptance Criteria

This milestone is complete only if:

* the Showcase ROM starts YM2151 music playback
* FM playback is implemented using explicit YM2151 register writes
* the current Showcase scene continues functioning
* repository build succeeds
* tests pass
* repeated headless runs remain deterministic
* `docs/showcase_state_snapshot.md` is updated
* a completion report is written

## Deliverables

* updated Showcase ROM with YM2151 music playback
* completion report:

```text
artifacts/reports/m40_ym2151_music_playback.md
```

## Completion Report Requirements

The report must include:

* milestone preflight result
* summary
* files changed
* commands run
* verification results
* issues discovered
* next-step recommendation

## Next Milestone

After successful completion of M40, proceed to:

**M41 – Audio Mixing Demonstration** 
