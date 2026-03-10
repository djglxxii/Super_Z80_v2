# Task Packet — M38 Metasprite Example

## Goal

Demonstrate how multiple hardware sprites can be combined into a **single logical object (metasprite)** in the Showcase ROM.

This milestone validates:

* multiple SAT entries
* coordinated sprite positioning
* multi-sprite composition that moves as a single object

The result should function as a reference example for building larger game objects.

---

# Milestone

**M38 – Metasprite Example**

---

# Scope

Extend the current single-sprite demo so the sprite becomes a **metasprite composed of multiple hardware sprites**.

Example configuration:

* 2×2 sprite layout
* each hardware sprite contributes one tile
* all sprites move together as a single unit

The metasprite should remain animated using the existing frame system.

---

# Non-Goals

Do not implement:

* a generalized metasprite engine
* sprite animation frameworks
* collision detection
* input handling
* gameplay logic
* scene management
* audio integration
* SDK changes unless a real defect is discovered

---

# Files Expected to Change

Likely files:

```
rom/showcase/assets/demo_sprite.asm
rom/showcase/src/init.asm
rom/showcase/src/main_loop.asm
rom/showcase/src/vdp_init.asm
rom/showcase/inc/constants.inc
rom/showcase/inc/memory_map.inc
rom/showcase/inc/vdp_helpers.inc
docs/showcase_state_snapshot.md
docs/state_snapshot.md
artifacts/reports/m38_metasprite_example.md
```

Modify other files only if necessary.

---

# Implementation Requirements

## 1. Metasprite Layout

Create a simple metasprite composed of multiple hardware sprites.

Example:

```
sprite0 (top-left)
sprite1 (top-right)
sprite2 (bottom-left)
sprite3 (bottom-right)
```

Each sprite references its own tile.

Offsets should be constant.

Example offsets:

```
(0,0)
(8,0)
(0,8)
(8,8)
```

---

## 2. Shared Position State

Maintain a single logical position:

```
meta_x
meta_y
```

Each hardware sprite derives its position from this base coordinate plus offsets.

---

## 3. Animation Support

Reuse the existing animation frame system from M37.

Frame switching should update all metasprite tiles consistently.

---

## 4. SAT Updates

Update multiple SAT entries each frame.

Example:

```
SAT[0]
SAT[1]
SAT[2]
SAT[3]
```

Each entry should reference the appropriate tile and position.

---

## 5. Preserve Existing Behavior

The metasprite must operate within the existing demo:

* parallax scrolling background remains active
* deterministic motion continues
* metasprite moves across the screen

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
./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 20
./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 20
```

---

# Acceptance Criteria

The milestone is complete only if:

* the metasprite appears composed of multiple hardware sprites
* all component sprites move together as one object
* animation frames still function
* parallax scrolling remains operational
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
artifacts/reports/m38_metasprite_example.md
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

After M38 completes:

**M39 – Controller Input Demo**

This milestone will validate:

* controller polling
* input-driven sprite movement
* integration with the main loop without breaking deterministic behavior.
