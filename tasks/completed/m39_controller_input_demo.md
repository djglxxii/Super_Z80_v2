M38 is valid and complete. The metasprite implementation satisfies the milestone objectives: coordinated SAT updates, shared origin state, and preservation of animation and parallax behavior with deterministic output.

Proceed to **M39 – Controller Input Demo**.

Save this as:

```
tasks/active/m39_controller_input_demo.md
```

---

# Task Packet — M39 Controller Input Demo

## Goal

Demonstrate controller input polling and use it to control the Showcase metasprite.

This milestone validates:

* controller input polling
* integration of input handling with the main loop
* deterministic behavior when no input is present
* sprite movement driven by controller input

The demo should extend the existing metasprite scene without breaking animation or parallax scrolling.

---

# Milestone

**M39 – Controller Input Demo**

---

# Scope

Extend the Showcase ROM so the metasprite can be moved using controller input.

The metasprite should:

* move horizontally and vertically
* continue animating while moving
* remain visible above the scrolling parallax scene

Input must be processed during the main loop.

---

# Non-Goals

Do not implement:

* gameplay logic
* collision detection
* menu systems
* scene manager
* audio
* input abstraction layers
* generalized controller frameworks
* SDK changes unless a real defect is discovered

---

# Files Expected to Change

Likely files:

```
rom/showcase/src/main_loop.asm
rom/showcase/src/init.asm
rom/showcase/inc/constants.inc
rom/showcase/inc/memory_map.inc
rom/showcase/inc/vdp_helpers.inc
rom/showcase/inc/input_helpers.inc (if created)
docs/showcase_state_snapshot.md
docs/state_snapshot.md
artifacts/reports/m39_controller_input_demo.md
```

Modify other files only if necessary.

---

# Implementation Requirements

## 1. Controller Polling

Add controller polling logic to the main loop.

The code should read the controller input register exposed by the hardware layer and store the current state.

Example inputs:

```
UP
DOWN
LEFT
RIGHT
```

Polling should occur once per frame.

---

## 2. Metasprite Movement

Modify the metasprite origin (`meta_x`, `meta_y`) based on controller input.

Example behavior:

```
LEFT  -> meta_x -= 1
RIGHT -> meta_x += 1
UP    -> meta_y -= 1
DOWN  -> meta_y += 1
```

Movement should be small and deterministic per frame.

---

## 3. Preserve Animation

The existing animation system from M37 must continue functioning while the sprite moves.

Animation cadence should remain unchanged.

---

## 4. Preserve Parallax Scene

The background and foreground scrolling implemented in M35 must remain active.

The metasprite must move independently of the scrolling layers.

---

## 5. Deterministic Headless Behavior

When running in headless mode:

* no input will be present
* the metasprite must remain stationary
* the ROM must produce identical output across repeated runs

This ensures deterministic automated verification.

---

# Verification Commands

Run repository verification:

```
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Build the Showcase ROM:

```
make -C rom/showcase clean
make -C rom/showcase
```

Run deterministic verification twice:

```
./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 24
./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 24
```

---

# Acceptance Criteria

The milestone is complete only if:

* controller input moves the metasprite
* the metasprite continues animating
* parallax scrolling remains functional
* repository builds successfully
* all tests pass
* repeated headless runs produce identical output
* `docs/showcase_state_snapshot.md` updated
* completion report written

---

# Deliverables

Updated Showcase ROM:

```
rom/showcase/
```

Completion report:

```
artifacts/reports/m39_controller_input_demo.md
```

---

# Completion Report Requirements

The report must include:

* summary
* files changed
* commands run
* verification results
* issues discovered
* next-step recommendation

---

# Next Milestone

After M39 completes:

**M40 – Showcase Scene Sequencer**

This milestone will introduce:

* timed scene transitions
* simple state machine
* automated demo playback across all features developed so far.
