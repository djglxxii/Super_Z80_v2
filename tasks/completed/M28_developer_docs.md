# Task Packet

## Title

M28 — Developer Documentation and SDK Guidance

## Goal

Produce complete developer-facing documentation describing how to build software for the Super_Z80 platform using the now-validated emulator architecture.

This milestone documents the platform’s hardware model, programming interfaces, and development workflow without modifying emulator behavior.

---

# Scope

Included:

* CPU programming model documentation
* memory map documentation
* I/O register documentation
* graphics programming guide
* audio programming guide
* controller input guide
* deterministic timing model description
* development workflow documentation
* milestone report
* repository state snapshot update

Excluded:

* new emulator functionality
* changes to the hardware specification
* SDK tooling implementation
* ROM showcase development
* performance optimizations

---

# Files Expected to Change

```
docs/state_snapshot.md
```

---

# Files to Create

```
docs/developer_guide.md
docs/hardware_reference.md
docs/programming_graphics.md
docs/programming_audio.md
docs/programming_input.md
docs/platform_timing.md
artifacts/reports/M28_developer_docs_report.md
```

---

# Constraints

* Documentation must match the **actual implemented emulator behavior**.
* `docs/audio_spec.md`, `docs/vdp_baseline_v1.md`, and the repository’s I/O register map remain authoritative.
* Do not introduce speculative features not implemented in the emulator.
* Documentation must clearly distinguish between:

  * CPU-visible behavior
  * internal emulator implementation details
* No emulator code may be modified during this milestone.

---

# Implementation Requirements

## 1. Hardware Reference

Create a concise hardware reference for developers.

Content must include:

* CPU model and execution characteristics
* memory layout
* VRAM behavior
* palette format
* controller input ports
* audio subsystem overview
* I/O port map summary

This document becomes the canonical **platform overview**.

---

## 2. Graphics Programming Guide

Document how to use the VDP and tile system.

Include:

* tile format
* tilemap layout
* VRAM usage
* palette usage
* Plane A / Plane B behavior
* sprite engine rules
* layering and priority behavior
* transparency behavior

---

## 3. Audio Programming Guide

Document how software controls the audio subsystem.

Include:

* PSG channel control
* YM2151 register interface
* tone/noise behavior
* volume control
* recommended programming patterns

---

## 4. Input Programming Guide

Document how games read controller state.

Include:

* PAD1 port behavior
* PAD1_SYS behavior
* button bit mapping
* recommended polling patterns

---

## 5. Timing Model

Document the deterministic timing contract.

Include:

* scanline-driven execution
* VBlank timing
* IRQ behavior
* DMA restrictions
* audio progression timing

This section is important for deterministic gameplay behavior.

---

## 6. Development Workflow

Document how developers should build software for the platform.

Include:

* recommended toolchains
* ROM creation workflow
* emulator usage
* deterministic debugging approach

---

# Verification Commands

```
test -f docs/developer_guide.md
test -f docs/hardware_reference.md
test -f docs/programming_graphics.md
test -f docs/programming_audio.md
test -f docs/programming_input.md
test -f docs/platform_timing.md
```

---

# Deliverables

* full developer documentation
* updated repository state snapshot
* milestone completion report

```
artifacts/reports/M28_developer_docs_report.md
```

---

# Completion Criteria

The milestone is complete when:

* documentation accurately reflects implemented platform behavior
* documentation is organized and developer-readable
* repository state snapshot updated
* milestone report written
* no emulator code changed

---

# Recommended Next Step

After M28 the project can proceed to optional improvements:

```
M29 — Emulator Debug Tooling
M30 — Performance Optimization
M31 — SDK Utilities
```

At this point the Super_Z80 emulator becomes **a usable development platform rather than just a hardware recreation**.
