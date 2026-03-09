# Task Packet — M36 Basic Sprite Rendering

## Goal

Introduce the first sprite rendering demonstration in the Showcase ROM.

This milestone validates:

* sprite tile upload
* sprite attribute table usage
* sprite positioning
* sprite motion independent of background layers

The demo must run on top of the existing parallax scrolling environment.

---

# Milestone

**M36 – Basic Sprite Rendering**

---

# Scope

Extend the Showcase ROM to render and animate at least one sprite over the parallax background created in M35.

The sprite must:

* be visible above the scrolling layers
* move independently of background scroll
* demonstrate correct sprite attribute configuration

This milestone validates the **sprite pipeline**.

---

# Non-Goals

Do not implement:

* collision detection
* sprite animation systems
* controller input
* gameplay logic
* scene manager
* menus
* audio
* sprite batching engines
* SDK changes unless a real defect is discovered

---

# Files Expected to Change

Likely files:

```
rom/showcase/src/init.asm
rom/showcase/src/main_loop.asm
rom/showcase/src/vdp_init.asm
rom/showcase/src/text.asm
rom/showcase/inc/constants.inc
rom/showcase/inc/memory_map.inc
rom/showcase/inc/vdp_helpers.inc
rom/showcase/assets/
docs/showcase_state_snapshot.md
docs/state_snapshot.md
artifacts/reports/m36_basic_sprite_rendering.md
```

Modify other files only if necessary.

---

# Implementation Requirements

## 1. Sprite Asset

Add a simple sprite graphic asset.

Example:

```
rom/showcase/assets/demo_sprite.asm
```

The sprite may be:

* 8×8 or 16×16
* simple geometric shape or icon

It must be visually distinguishable during motion.

---

## 2. Sprite Tile Upload

Upload sprite tile graphics to VRAM during initialization.

This should occur after background and foreground tile uploads.

---

## 3. Sprite State

Add sprite state in RAM:

```
sprite_x
sprite_y
```

Initialize these values deterministically during boot.

---

## 4. Sprite Attribute Table

Populate a sprite entry in the sprite attribute table:

* sprite tile index
* X coordinate
* Y coordinate
* palette/flags as required

This should occur during render updates.

---

## 5. Motion

Update sprite position each frame.

Example:

```
sprite_x += 1
```

or

```
sprite_x += 1
sprite_y += 1
```

The sprite must visibly move across frames.

---

## 6. Independence From Background

The sprite must remain stable relative to the screen while:

* the background scrolls
* the foreground layer scrolls

This confirms that sprite coordinates are independent of tilemap scrolling.

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

Run deterministic headless verification twice:

```
./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 12
./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 12
```

---

# Acceptance Criteria

The milestone is complete only if:

* sprite tiles upload correctly
* sprite appears on screen
* sprite moves across frames
* parallax scrolling continues functioning
* repository builds successfully
* all tests pass
* repeated headless runs produce identical results
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
artifacts/reports/m36_basic_sprite_rendering.md
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

After M36 completes:

**M37 – Sprite Motion and Multi-Sprite Demo**

This milestone will expand the sprite system to:

* support multiple sprites
* demonstrate coordinated sprite motion
* validate sprite attribute table updates across frames.
