# M42 – Integrated Showcase Scene

Agent Task Packet

## Goal

Create a unified demonstration scene in the Showcase ROM that simultaneously uses:

* scrolling background
* animated metasprite
* YM2151 background music
* PSG sound effects
* text overlay

The purpose is to produce a **single coherent demo scene** showing the full platform capability.

---

# Required Scene Behavior

The integrated scene must include the following elements running simultaneously.

## Background

Existing parallax scrolling from **M35** must remain active.

```
BG plane scrolls slowly
FG plane scrolls at a different speed
```

---

## Sprite / Metasprite

The animated metasprite from **M37–M38** must remain active.

Behavior:

```
sprite animation continues
sprite position controllable by PAD1
```

Suggested mapping:

```
LEFT  -> move left
RIGHT -> move right
UP    -> move up
DOWN  -> move down
```

---

## Music

The YM2151 music loop from **M40** must continue running.

Requirements:

```
music runs continuously
no reset when sound effects trigger
```

---

## Sound Effects

PSG sound effects from **M39** must remain available.

Trigger:

```
PAD1 A -> trigger PSG tone burst
```

Music must continue during sound effects.

---

## Text Overlay

Add an on-screen text label using the text renderer from **M31**.

Example:

```
SUPER Z80 PLATFORM SHOWCASE
D-PAD MOVE  A: SOUND
```

Text may be static.

---

# Files Expected to Change

ROM implementation:

```
rom/showcase/src/main_loop.asm
rom/showcase/src/showcase_update.asm (if present)
rom/showcase/src/render.asm (optional)
```

Possible constants:

```
rom/showcase/inc/constants.inc
```

Documentation:

```
docs/showcase_state_snapshot.md
docs/state_snapshot.md
artifacts/reports/m42_integrated_showcase_scene.md
```

---

# Implementation Guidance

This milestone should **not introduce new hardware features**.

It should only integrate the already implemented systems.

Frame loop should roughly resemble:

```
frame_update:

    read_controller
    update_sprite_position
    update_sprite_animation

    update_music
    update_sound_effects

    update_scroll

    render_text_overlay
```

Keep the scene deterministic and simple.

---

# Verification Commands

Standard repository verification:

```
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

ROM build:

```
make -C rom/showcase clean
make -C rom/showcase
```

Deterministic execution:

```
./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 48
./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 48
```

CRC results must match.

---

# Deliverables

The agent must produce:

```
artifacts/reports/m42_integrated_showcase_scene.md
```

The report must include:

```
Milestone preflight
Summary
Files changed
Commands run
Verification results
CRC determinism output
```

---

# Expected Result

The Showcase ROM now runs a **complete interactive platform demo** featuring:

```
parallax scrolling
animated metasprite
player input
text overlay
YM2151 music
PSG sound effects
simultaneous audio mixing
```

This milestone effectively produces the **final demonstration ROM behavior**.

---

# Next Milestone

After completion:

```
M43 – Developer Reference ROM Finalization
```

This final milestone prepares the ROM as the official **developer reference implementation** with documentation and source cleanup.
