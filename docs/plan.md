# Super_Z80_v2 Execution Plan

## Strategy

The project follows deterministic, milestone-driven development with explicit verification at each step.

Each milestone must:

* remain narrow in scope
* produce a report in `artifacts/reports/`
* update `docs/state_snapshot.md`
* include deterministic build/test commands

Work proceeds through the following phases:

1. **Infrastructure & VDP Completion (M0–M23)** – Completed
2. **Platform Completion (M24–M28)** – Completed
3. **Audio Expansion (M29)** – Completed
4. **Platform Stabilization (implicit)** – Completed during M29 work
5. **Showcase ROM & Developer Reference (M29h–M43)** – Current focus

The goal of Phase 5 is to produce a **reference ROM for developers and a capability demonstration for the Super_Z80 platform**.

---

# Phase 1 – Infrastructure & VDP Completion (Completed)

All core emulator subsystems and graphics features were implemented.

| Milestone | Description                                                                                                                                                                                                                                                 |
| --------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| M0–M17    | Repository bootstrap; emulator skeleton; CRC utilities; memory, bus, CPU; I/O routing; scheduler/timing; IRQ controller; VBlank; DMA; video pipeline skeleton; emulator coordinator; headless runner; regression harness; SDL shell; replay/snapshot parity |
| M16       | Foreground tile plane (second layer) with independent scrolling                                                                                                                                                                                             |
| M17       | Sprite priority (front/behind FG) and deterministic ordering                                                                                                                                                                                                |
| M18       | Sprite scanline evaluation and 16-sprite limit per scanline; overflow flag                                                                                                                                                                                  |
| M19       | Sprite attributes: horizontal and vertical flipping                                                                                                                                                                                                         |
| M20       | Sprite pattern banks                                                                                                                                                                                                                                        |
| M21       | Tile pattern banks for BG and FG planes                                                                                                                                                                                                                     |
| M22       | Sprite collision detection status bit                                                                                                                                                                                                                       |
| M23       | VDP baseline freeze                                                                                                                                                                                                                                         |

Result:

The emulator now contains a **stable and complete graphics system**, including:

* background plane
* foreground plane
* sprites
* palette system
* scrolling
* pattern banks
* scanline evaluation

---

# Phase 2 – Platform Completion (Completed)

This phase completed the remaining platform subsystems beyond the graphics pipeline.

## M24 – Controller Input Hardware

Implemented deterministic controller input.

Features:

* PAD1 and PAD1_SYS ports wired through bus
* deterministic button state reads
* SDL input integration
* unit tests for input states

---

## M25 – Audio System Design

Produced `docs/audio_spec.md` defining the baseline audio architecture.

Design includes:

* tone channels
* noise channel
* register map
* scheduler integration
* deterministic timing rules

---

## M26 – Audio Implementation

### M26a – Audio Registers

CPU-visible register interface.

### M26b – Tone and Noise Generator State

Deterministic internal audio generator progression.

### M26c – Audio Mixer

Channel mixing and amplitude evaluation.

### M26d – SDL Audio Integration

Host audio playback integration.

### M26e – Audio Validation

Deterministic validation tests.

---

## M27 – Platform Validation

End-to-end hardware validation.

Tests include:

* CPU
* DMA
* IRQ
* VBlank
* input
* audio

---

## M28 – Documentation & SDK Alignment

Updated documentation and SDK interfaces to match the finalized hardware model.

Deliverables included:

* updated developer documentation
* SDK runtime guidance
* hardware contract alignment

---

# Phase 3 – YM2151 FM Audio Expansion (Completed)

This phase introduced the YM2151 FM synthesis expansion chip.

## M29 – YM2151 Implementation

### FM Hardware Milestones

* M29a – YM2151 register interface
* M29b – operator and channel state model
* M29c – timers and IRQ behavior
* M29d – FM sample generation
* M29e – PSG + YM mixer integration
* M29f – deterministic audio validation

### Host Platform Integration

* M29g – SDL audio output integration

PCM audio remains intentionally excluded.

---

# Phase 4 – Platform Stabilization (Implicit Completion)

During the YM2151 milestone work, the emulator platform was stabilized through deterministic validation and regression testing.

Subsystems now considered stable:

* CPU execution
* memory bus
* DMA
* VDP
* sprite pipeline
* input
* PSG audio
* YM2151 FM audio
* scheduler
* deterministic testing harness

At this point the **Super_Z80 platform is considered feature complete**.

---

# Phase 5 – Showcase ROM and Developer Reference (Current Focus)



This phase produces a **reference ROM demonstrating platform capabilities** and serves as a **developer template ROM**.

The Showcase ROM must emphasize:

* clarity of implementation
* reusable coding patterns
* deterministic execution
* clear subsystem demonstrations

The ROM will reside in:

```
rom/showcase/
```

---

# Showcase ROM Milestones

## M29h – ROM Execution Contract

Implement the minimal emulator support required to load and execute an external ROM artifact through the current emulator CLI and headless flows.

Objectives:

* add ROM-loading support for external ROM files
* accept ROM path in emulator execution flow
* support deterministic headless execution for ROM milestones
* document the verification command for ROM work

Verification:

* emulator runs a minimal ROM from disk
* headless execution succeeds for that ROM
* repeated runs are deterministic

Purpose:

Establish the execution foundation required for all Showcase ROM work.

---

## M29i – Minimal SDK Runtime Surface

Create the minimal real SDK runtime/include/resource surface required for Showcase ROM development.

Objectives:

* replace placeholder-only SDK paths with real include/runtime files
* provide minimal startup/init path
* provide default font resource path
* provide minimal splash/startup resource contract

Verification:

* minimal ROM can assemble against SDK includes
* SDK startup path is callable
* font/splash resources exist in repository truth

Purpose:

Establish the minimum real SDK foundation required before SDK-backed Showcase integration.

---

## M29j – SDK Integration Validation

Validate integration of the SDK runtime modules with a minimal Showcase ROM.

Objectives:

* create minimal `rom/showcase/`
* boot through SDK initialization path
* verify font and splash resources can be referenced
* boot successfully in emulator

Verification:

* ROM builds successfully
* ROM boots in emulator
* ROM displays SDK splash/startup deterministically

Purpose:

Ensure the Showcase ROM properly consumes the SDK runtime infrastructure before additional features are implemented.

---

## M30 – Showcase ROM Project Scaffold

Create the base ROM project.

Objectives:

* Create `rom/showcase/` directory
* Add ROM Makefile
* Add minimal boot code
* Initialize VRAM
* Clear tilemaps
* Display blank screen

Verification:

ROM boots successfully in emulator.

Purpose:

Provide a **minimal runnable ROM foundation**.

---

## M31 – Font and Text Rendering

Add a basic character set and text rendering.

Features:

* load font tiles into VRAM
* implement tile-based text output routine
* render text to screen

Verification:

ROM displays:

```
SUPER Z80 SHOWCASE
```

Purpose:

Provide a **reference implementation for text output**.

---

## M32 – System Splash Screen

Add a platform splash screen.

Features:

* load splash tilemap
* display title screen
* show system name and version

Verification:

ROM boots to splash screen.

Purpose:

Demonstrate **ROM initialization flow and static graphics rendering**.

---

## M33 – Scrolling Tilemap Demo

Introduce background scrolling.

Features:

* large tilemap
* horizontal scroll
* continuous scrolling movement

Verification:

background scrolls smoothly.

Purpose:

Demonstrate **tilemap scrolling registers**.

---

## M34 – Vertical Scrolling Demo

Add vertical scrolling capability.

Features:

* vertical scrolling
* large vertical map

Verification:

vertical scroll movement visible.

Purpose:

Show developers how to use **vertical scroll registers**.

---

## M35 – Parallax Scrolling Demo

Demonstrate layered scrolling.

Features:

* background plane scrolling
* foreground plane scrolling
* different scroll speeds

Verification:

two layers move at different speeds.

Purpose:

Demonstrate **parallax scrolling techniques**.

---

## M36 – Basic Sprite Rendering

Introduce sprite system usage.

Features:

* load sprite tiles
* render sprite
* move sprite across screen

Verification:

sprite moves across screen.

Purpose:

Demonstrate **basic sprite usage**.

---

## M37 – Sprite Animation

Add sprite animation.

Features:

* animation frames
* timed frame switching

Verification:

sprite visibly animates.

Purpose:

Demonstrate **sprite animation techniques**.

---

## M38 – Metasprite Example

Introduce multi-sprite objects.

Features:

* group multiple sprites
* construct a single object from multiple sprite entries

Verification:

multi-sprite object moves correctly.

Purpose:

Demonstrate **metasprite construction**.

---

## M39 – PSG Sound Effects

Introduce sound effects.

Features:

* PSG tone generation
* triggered sound effects

Verification:

button press triggers sound.

Purpose:

Demonstrate **PSG audio usage**.

---

## M40 – YM2151 Music Playback

Introduce FM music playback.

Features:

* load simple music pattern
* continuous playback

Verification:

background music plays.

Purpose:

Demonstrate **YM2151 FM audio usage**.

---

## M41 – Audio Mixing Demonstration

Combine music and sound effects.

Features:

* background music
* triggered sound effects

Verification:

sound effects play over music.

Purpose:

Demonstrate **simultaneous audio playback and mixing**.

---

## M42 – Integrated Showcase Scene

Combine multiple systems into one demonstration.

Scene includes:

* scrolling background
* animated metasprite
* music playback
* triggered sound effects
* text overlay

Verification:

interactive demonstration runs correctly.

Purpose:

Provide a **complete platform capability demonstration**.

---

## M43 – Developer Reference ROM Finalization

Finalize the Showcase ROM as the official reference implementation.

Deliverables:

* code cleanup and final structure
* explanatory comments in source files
* developer documentation

Documentation file:

```
docs/showcase_programmers_guide.md
```

Purpose:

Provide the **canonical example ROM for developers building games on the Super_Z80 platform**.

---

# Verification & Rollback Strategy

Each milestone must include deterministic verification.

Verification commands must include:

```
cmake -S . -B build
cmake --build build
ctest --test-dir build
```

Showcase ROM milestones may also include deterministic emulator execution:

```
emulator --headless rom/showcase/showcase.rom
```

Rollback strategy:

* each milestone remains narrow in scope
* all work is reversible through git history
* plan modifications must update `docs/plan.md` explicitly
