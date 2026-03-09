# Task Packet — M33 Scrolling Tilemap Demo

## Goal

Demonstrate background tilemap scrolling using the VDP scroll registers and the existing Showcase asset pipeline.

This milestone introduces the first **dynamic visual behavior** in the Showcase ROM.

---

# Milestone

**M33 – Scrolling Tilemap Demo**

---

# Scope

Extend the Showcase ROM to include a deterministic background scrolling demonstration.

The scrolling should:

* use the splash tilemap or a derived tilemap
* scroll smoothly across frames
* run continuously inside the existing main loop

The demonstration should clearly validate that the VDP scroll registers function correctly.

---

# Non-Goals

Do not implement:

* sprites
* parallax
* scene management
* timed scene transitions
* input-driven camera movement
* audio synchronization
* complex rendering engines
* SDK changes unless an integration defect is discovered

---

# Files Expected to Change

Likely files:

```
rom/showcase/src/main_loop.asm
rom/showcase/src/init.asm
rom/showcase/src/vdp_init.asm
rom/showcase/inc/constants.inc
rom/showcase/inc/vdp_helpers.inc
docs/showcase_state_snapshot.md
docs/state_snapshot.md
artifacts/reports/m33_scrolling_tilemap_demo.md
```

Modify other files only if required.

---

# Implementation Requirements

## 1. Scroll State

Maintain scroll position variables in RAM:

```
scroll_x
scroll_y
```

Increment at least one axis each frame.

Example behavior:

```
scroll_x += 1 each frame
```

Wrap naturally when reaching the tilemap boundary.

---

## 2. VDP Scroll Register Usage

Write updated scroll values each frame through the VDP scroll registers.

This must occur during the main loop's render phase.

Prefer a helper routine if one does not exist.

---

## 3. Visible Demonstration

The background should visibly move across frames.

Use the existing splash tilemap as the initial scroll surface unless a slightly larger demo tilemap is needed.

---

## 4. Deterministic Execution

The scroll update must be deterministic.

Avoid frame-timing dependencies beyond the main loop cadence.

The ROM must produce identical headless execution results across repeated runs.

---

## 5. Maintain Text Layer

The system splash text may remain visible or be repositioned.

The scrolling demonstration should not break the reusable text path introduced in M31.

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

Run deterministic headless validation twice:

```
./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 8
./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 8
```

---

# Acceptance Criteria

The milestone is complete only if:

* the background tilemap scrolls continuously
* scroll registers are updated through the main loop
* repository builds successfully
* all tests pass
* headless ROM execution remains deterministic
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
artifacts/reports/m33_scrolling_tilemap_demo.md
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

After M33:

**M34 – Sprite Rendering Demo**

This milestone will validate:

* sprite attribute memory
* sprite tile upload
* sprite rendering pipeline
* moving objects independent of the background.
