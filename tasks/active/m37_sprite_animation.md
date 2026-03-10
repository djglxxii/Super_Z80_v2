# Task Packet — M37 Sprite Animation

## Goal

Extend the existing Showcase ROM sprite demo to demonstrate **deterministic sprite animation**.

This milestone must prove that sprite tile indices can be switched over time to create visible animation while preserving:

* deterministic execution
* the parallax background demo
* the working SAT update path established in M36

## Milestone

**M37 – Sprite Animation** 

## Scope

Enhance the current Showcase ROM so the existing sprite uses multiple animation frames and switches between them during the main loop.

This is a **single-sprite animation milestone**.

The sprite should:

* remain visible above the scrolling layers
* continue moving independently of background scroll
* switch between at least two visually distinct frames on a deterministic cadence

## Non-Goals

Do not implement:

* metasprites / multi-sprite objects
* multiple independently animated sprites
* controller input
* collision detection
* gameplay logic
* scene manager
* menus
* audio
* generalized sprite engines
* SDK changes unless a real defect is discovered

## Constraints

* Follow `docs/plan.md` milestone ordering and repository truth
* Keep scope narrow and deterministic, consistent with AWOS small-horizon execution rules
* Do not skip ahead to M38 metasprites
* Preserve the M36 SAT update path and the M35 parallax composition
* Do not refactor unrelated ROM or emulator systems

## Files Expected to Change

Likely files:

* `rom/showcase/assets/demo_sprite.asm`
* `rom/showcase/src/init.asm`
* `rom/showcase/src/main_loop.asm`
* `rom/showcase/inc/constants.inc`
* `rom/showcase/inc/memory_map.inc`
* `rom/showcase/inc/vdp_helpers.inc`
* `docs/showcase_state_snapshot.md`
* `docs/state_snapshot.md`
* `artifacts/reports/m37_sprite_animation.md`

Modify other files only if required and explain why.

## Implementation Requirements

### 1. Animation Frames

Extend the existing local sprite asset so it contains at least **two frames**.

Example structure:

* frame 0 tile data
* frame 1 tile data

The two frames must be visually distinguishable.

A simple blink, leg position change, or geometric variation is sufficient.

### 2. Sprite Animation State

Add deterministic animation state in RAM.

At minimum:

* `sprite_frame`
* `sprite_anim_counter`

Example logic:

* increment `sprite_anim_counter` each frame
* after a fixed number of frames, toggle `sprite_frame`
* reset the counter

The cadence must be fixed and deterministic.

### 3. SAT Tile Index Switching

Update the sprite’s tile index in the SAT entry based on the current animation frame.

Example pattern:

* base tile = frame 0
* base tile + 1 = frame 1

The SAT entry must be rewritten consistently during render/update, reusing the M36 SAT update path.

### 4. Preserve Existing Motion

The sprite must continue moving as in M36.

Animation is additive to motion, not a replacement for it.

The result should be:

* moving sprite
* animated frames
* parallax background still functioning

### 5. Developer-Reference Clarity

Keep the code readable as a reference example:

* clear animation state variables
* clear frame-switch rules
* obvious relationship between frame state and SAT tile index

Avoid over-engineering or introducing a future-proof animation framework.

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
./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 16
./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 16
```

## Acceptance Criteria

This milestone is complete only if all of the following are true:

* the Showcase ROM sprite visibly animates using multiple frames
* the sprite continues moving across the screen
* animation frame switching is deterministic
* parallax scrolling continues functioning
* repository builds successfully
* all tests pass
* repeated headless runs produce identical results
* `docs/showcase_state_snapshot.md` is updated
* a completion report is written

## Deliverables

* updated Showcase ROM with single-sprite animation
* completion report:

```text
artifacts/reports/m37_sprite_animation.md
```

## Completion Report Requirements

The report must include:

* summary
* files changed
* commands run
* verification results
* issues discovered
* next-step recommendation

## Next Milestone

After successful completion of M37, proceed to:

**M38 – Metasprite Example**
