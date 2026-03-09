# Task Packet — M35 Parallax Scrolling Demo

## Goal

Demonstrate **independent parallax scrolling layers** using the Super_Z80 background rendering capabilities.

This milestone validates that multiple background layers can move at different rates to produce a depth effect.

---

# Milestone

**M35 – Parallax Scrolling Demo**

---

# Scope

Extend the Showcase ROM scrolling demonstration so that **two visual layers move at different scroll rates**.

Typical example:

* background layer scrolls slowly
* foreground layer scrolls faster

The purpose is to demonstrate the console’s ability to produce parallax depth.

---

# Non-Goals

Do not implement:

* sprites
* sprite animation
* gameplay logic
* controller input
* scene manager
* menu systems
* camera systems
* audio
* complex rendering abstractions
* SDK changes unless an integration defect is discovered

---

# Files Expected to Change

Likely files:

```
rom/showcase/src/main_loop.asm
rom/showcase/src/init.asm
rom/showcase/src/vdp_init.asm
rom/showcase/src/text.asm
rom/showcase/assets/
rom/showcase/inc/constants.inc
rom/showcase/inc/memory_map.inc
docs/showcase_state_snapshot.md
docs/state_snapshot.md
artifacts/reports/m35_parallax_scrolling_demo.md
```

Modify other files only if necessary and explain why.

---

# Implementation Requirements

## 1. Two Scroll Layers

Implement two logical layers:

```
foreground_scroll_x
foreground_scroll_y
background_scroll_x
background_scroll_y
```

Each layer should move at a different rate.

Example:

```
foreground_scroll_x += 2
background_scroll_x += 1
```

---

## 2. Layer Composition

The ROM must display a foreground and background that clearly show independent motion.

Possible approaches:

* separate tilemaps for foreground and background
* repeated background pattern behind foreground tiles
* slower-moving background pattern

The visual difference must be clear.

---

## 3. Deterministic Frame Updates

Both scroll layers must update deterministically each frame.

Avoid:

* timing-based transitions
* hidden state
* nondeterministic behavior

---

## 4. Preserve Existing Systems

Do not break:

* splash composition
* text rendering helpers
* ROM scaffold layout
* deterministic execution

---

## 5. Developer Reference Clarity

The implementation should remain easy to study:

* clear scroll variables
* obvious per-frame updates
* obvious VDP register writes

The Showcase ROM acts as a reference implementation.

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
./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 10
./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 10
```

---

# Acceptance Criteria

The milestone is complete only if:

* two visual layers scroll independently
* the parallax effect is clearly visible
* repository builds successfully
* all tests pass
* repeated headless runs are deterministic
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
artifacts/reports/m35_parallax_scrolling_demo.md
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

After M35 completes:

**M36 – Basic Sprite Rendering**

This milestone will introduce:

* sprite attribute memory
* sprite tile uploads
* independent sprite motion on top of the scrolling background.
