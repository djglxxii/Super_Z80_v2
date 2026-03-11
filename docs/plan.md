# Super_Z80_v2 Emulator Frontend Plan

This document defines the milestone plan for implementing the **interactive emulator frontend** for the Super_Z80_v2 platform.

The emulator platform is currently **hardware-complete** and provides:

* deterministic Z80 CPU execution
* memory bus and subsystem routing
* DMA controller
* IRQ controller
* deterministic scheduler
* VDP graphics pipeline
* sprite system
* controller input
* PSG audio
* YM2151 FM audio
* SDL audio playback
* deterministic headless validation harness
* Showcase demonstration ROM

The emulator currently operates primarily in **headless mode**.

While SDL support exists for audio and input, there is **no developer-facing graphical interface** for interacting with the emulator.

The purpose of this phase is to implement a **developer debugging environment** while preserving deterministic execution.

This work **must not modify hardware behavior**.

---

# Frontend Design Goals

The emulator frontend must provide:

### Developer Visibility

The interface must expose key hardware state including:

* CPU registers
* memory regions
* VRAM contents
* sprite tables
* palette data
* DMA queues
* audio registers
* scheduler timing

---

### ROM Development Workflow

Developers must be able to:

* load ROM files
* reset emulator
* pause and resume execution
* step execution frames
* inspect emulator state

---

### Determinism Preservation

The frontend must **not introduce non-deterministic behavior**.

The following must remain unchanged:

* headless execution
* deterministic validation harness
* regression tests

---

# Architectural Principles

The frontend must preserve strict subsystem separation.

---

## Core Emulator

```
emulator/core/
```

Contains deterministic hardware logic:

* CPU
* bus
* DMA
* IRQ
* scheduler
* VDP
* audio

Properties:

* deterministic
* UI-independent
* testable in headless mode

---

## Platform Shell

```
emulator/shell/
```

Responsible for:

* SDL window
* input devices
* audio device
* event loop

The shell provides host integration but does not implement debugging tools.

---

## Frontend UI

```
emulator/frontend/
```

Responsible for:

* debugging panels
* ROM loading
* developer tools
* visualization

The frontend reads emulator state but must **not modify core subsystem logic**.

---

# Phase 6 – Emulator Frontend Development

This phase introduces a graphical developer environment.

---

# M44 – Frontend Architecture Foundation

Define the structural foundation of the frontend system.

Objectives:

* create `emulator/frontend/`
* define frontend initialization interface
* integrate frontend lifecycle with SDL shell
* prepare debugging panel system

Expected structure:

```
emulator/frontend/
    frontend.cpp
    frontend.h
    debug_panels/
```

Verification:

* emulator builds successfully
* frontend initializes without rendering UI
* headless mode unaffected

Purpose:

Create the structural foundation for the frontend architecture.

---

# M45 – ImGui Integration

Integrate Dear ImGui into the emulator runtime.

Objectives:

* initialize ImGui with SDL backend
* create UI render loop
* render a minimal diagnostic window

Verification:

* emulator window displays ImGui panel
* emulator execution continues normally

Purpose:

Establish graphical UI capability.

---

# M46 – Emulator Control Panel

Implement runtime execution controls.

Features:

* run / pause toggle
* reset emulator
* frame stepping
* ROM reload

Verification:

* controls manipulate emulator execution correctly
* frame stepping advances exactly one frame

Purpose:

Provide the minimal runtime interaction interface.

---

# M47 – ROM Loader Interface

Implement ROM loading through the UI.

Features:

* File → Load ROM menu
* ROM reload option
* invalid ROM error handling

Verification:

* ROM loads successfully
* emulator resets correctly
* execution begins from reset vector

Purpose:

Allow developers to test ROMs without CLI commands.

---

# M48 – CPU Debug Panel

Add a CPU inspection panel.

Displayed information:

* PC
* SP
* AF / BC / DE / HL
* interrupt flags
* CPU state

Verification:

* register values match emulator state
* values update each frame

Purpose:

Provide visibility into CPU execution.

---

# M49 – Memory Viewer

Add memory inspection tools.

Features:

* RAM viewer
* ROM viewer
* hex display
* scrolling memory view

Verification:

* memory contents match emulator state
* address navigation works

Purpose:

Allow developers to inspect runtime memory.

---

# M50 – VRAM Viewer

Implement VRAM inspection tools.

Features:

* tile pattern viewer
* tilemap viewer
* palette visualization

Verification:

* tile graphics display correctly
* palette mapping matches hardware behavior

Purpose:

Provide visibility into graphics resources.

---

# M51 – Sprite Debug Panel

Add sprite inspection interface.

Displayed information:

* sprite index
* position
* pattern index
* attributes
* priority flags

Visualization:

* highlight sprite bounding boxes

Verification:

* displayed sprite data matches SAT contents

Purpose:

Debug sprite behavior and rendering order.

---

# M52 – DMA Debug Panel

Expose DMA queue state.

Displayed information:

* active DMA commands
* command parameters
* remaining budget
* execution progress

Verification:

* displayed queue state matches DMA controller state

Purpose:

Allow debugging of VRAM transfers.

---

# M53 – Audio Debug Panel

Provide visibility into audio subsystem.

Displayed information:

* PSG registers
* YM2151 registers
* channel activity
* waveform output (optional)

Verification:

* values match internal audio state

Purpose:

Allow debugging of audio generation.

---

# M54 – Input Visualization Panel

Display controller state.

Features:

* button indicators
* active-low status
* input history (optional)

Verification:

* panel updates correctly when buttons pressed

Purpose:

Assist debugging input handling.

---

# M55 – Frame Timing & Scheduler Panel

Expose timing and scheduler information.

Displayed information:

* frame count
* CPU cycles per frame
* VBlank state
* scheduler queue

Verification:

* values match emulator timing state

Purpose:

Debug timing-related issues.

---

# M56 – Emulator Snapshot Tools

Introduce snapshot functionality.

Features:

* save emulator state
* load emulator state
* restore deterministic execution

Verification:

* snapshot restores identical execution

Purpose:

Enable debugging and reproducibility.

---

# M57 – Integrated Debug Overlay

Combine debugging panels into a cohesive interface.

Features:

* dockable panels
* window layout persistence
* debug menu organization

Verification:

* UI layout stable across runs

Purpose:

Provide a professional development environment.

---

# M58 – Frontend Stabilization

Finalize frontend implementation.

Tasks:

* code cleanup
* documentation updates
* usability improvements
* stability testing

Verification:

* emulator runs reliably with frontend enabled
* headless mode unaffected

Purpose:

Deliver a stable developer-facing emulator UI.

---

# Verification Strategy

Frontend milestones must verify:

```
cmake -S . -B build
cmake --build build
ctest --test-dir build
```

Runtime validation:

```
emulator
```

Headless mode must remain functional:

```
emulator --headless rom/showcase/showcase.rom
```

---

# Rollback Strategy

All milestones remain narrow and reversible.

Rollback procedure:

* revert milestone commit
* verify deterministic tests
* confirm headless runner integrity

---

# Outcome

Completion of this phase produces a **full-featured developer emulator frontend** capable of:

* interactive ROM loading
* subsystem inspection
* debugging CPU, graphics, audio, and DMA
* deterministic execution control

The frontend becomes the **primary development environment for Super_Z80 ROM development**.

---
