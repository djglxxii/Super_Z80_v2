# Task Packet

## Title

M55 — Frame Timing & Scheduler Panel

## Goal

Implement a read-only **Frame Timing & Scheduler Panel** in the Super_Z80 frontend that exposes the current timing state of emulator execution.

This milestone gives developers visibility into frame progression, scanline progression, scheduler-owned timing, and related counters so they can validate runtime behavior without relying on logs or headless-only output.

The panel must remain **read-only** in this milestone.

Do not implement timing controls, scheduler editing, profiling systems, historical graphs, or performance optimization work.

## Repository Context

Recent completed milestones:

- M45 — ImGui Integration
- M46 — Emulator Control Panel
- M47 — ROM Loader Interface
- M48 — CPU Debug Panel
- M49 — Memory Viewer
- M50 — VRAM Viewer
- M51 — Sprite Debug Panel
- M52 — DMA Debug Panel
- M53 — Audio Debug Panel
- M54 — Input Visualization Panel

The frontend now exposes CPU, memory, VRAM, sprites, DMA, audio, and input. The next milestone in the plan is **M55 — Frame Timing & Scheduler Panel**.

The deterministic emulator core and headless execution paths must remain unchanged.

## Required Read Order

Before making changes read:

1. `docs/plan.md`
2. `docs/state_snapshot.md`
3. `docs/architecture.md`
4. `docs/decisions.md`
5. `AGENTS.md`
6. `artifacts/reports/M54_input_visualization_panel_report.md`

Then inspect:

Frontend:

- `emulator/frontend/frontend.h`
- `emulator/frontend/frontend.cpp`
- `emulator/frontend/debug_panels/debug_panel_host.h`
- `emulator/frontend/debug_panels/debug_panel_host.cpp`

Timing/runtime path:

- `emulator/include/scheduler.h`
- `emulator/src/scheduler.cpp`
- `emulator/include/emulator_core.h`
- `emulator/src/emulator_core.cpp`
- `emulator/src/main.cpp`

Also inspect any timing-related state already surfaced in headless output, tests, or runtime control logic.

## Scope

Included:

- read-only `Frame Timing & Scheduler` ImGui panel
- current frame count display
- current scanline / scanline position display if available
- scheduler/timing counters already present in the implementation
- frame-synchronized refresh

Excluded:

- timing control UI
- frame history graphs
- performance profiler
- wall-clock FPS instrumentation unless already trivially available
- scheduler mutation/editing
- event trace history
- execution replay tooling

This milestone is **state inspection only**.

## Milestone Definition

The Frame Timing & Scheduler Panel must expose current timing state.

At minimum, display:

- frame count
- scanline state
- scheduler-related counters already present in the implementation

If available through narrow existing state or easy read-only snapshots, also display:

- frame-ready / VBlank-related timing state
- audio sample counters
- scanline counter
- cycle/tick counters
- current execution mode state if already naturally available in frontend/runtime

Only show fields that actually exist in the repository.

Verification:

- displayed timing values match emulator internal state
- panel updates during execution
- state remains deterministic

Purpose:

- assist debugging timing-related issues

## Architectural Constraints

Maintain strict separation between:

- emulator core
- platform shell
- frontend debug UI

The frontend must not modify scheduler state.

Timing state must be obtained through narrow read-only snapshot accessors or existing runtime-visible state handoff.

Do not expose mutable references.

Do not add UI dependencies to the scheduler or emulator core.

Do not move wall-clock ownership into the scheduler.

## Timing Coverage Requirements

Display the current timing state already maintained by the emulator.

Candidate fields include, if present:

- frame counter
- scanline counter
- current scanline within frame
- VBlank active / frame-ready
- scheduler scanlines per frame constant if useful for context
- audio sample count already produced
- any scheduler pending/phase counters that are naturally exposed

Prefer correctness and narrow scope over breadth.

## Panel Layout

Create a window titled:

```text
Frame Timing & Scheduler
````

Suggested layout:

```text
Frame Timing & Scheduler

Frame Counter:      123
Scanline Counter:   45
Scanlines/Frame:    262
VBlank:             ON
Frame Ready:        OFF
Audio Samples:      800
```

A compact grouped-text layout is acceptable.

If a small table is cleaner, that is acceptable too.

## Implementation Requirements

### 1. Create Frame Timing & Scheduler Panel

Extend the existing debug panel host system.

Do not introduce a separate UI architecture.

Render alongside the existing panels:

* CPU Debug
* Memory Viewer
* VRAM Viewer
* Sprite Debug
* DMA Debug
* Audio Debug
* Input Visualization

### 2. Access timing state

Retrieve timing/scheduler state through narrow read-only accessors or snapshots.

Possible approaches:

* Scheduler snapshot struct
* EmulatorCore-level timing snapshot
* reuse of already exposed counters/state

Avoid exposing mutable references.

Prefer a small snapshot API consistent with recent CPU/memory/VRAM/sprite/DMA/audio/input accessors.

### 3. Display timing fields

Display only fields that exist in the implementation and are meaningful for developers.

Prioritize:

* frame counter
* scanline counter / current scanline
* scheduler-owned timing state
* VBlank/frame-ready related state if available

### 4. Refresh behavior

The panel must refresh every rendered frame.

The panel must not trigger emulator execution.

The panel must not modify timing or scheduler behavior.

### 5. UI integration

The panel should appear automatically with the existing debug panels.

Use existing rendering conventions for consistency.

## Files Expected to Change

Likely:

* `emulator/frontend/debug_panels/debug_panel_host.h`
* `emulator/frontend/debug_panels/debug_panel_host.cpp`
* `emulator/frontend/frontend.h`
* `emulator/frontend/frontend.cpp`

Possibly:

* `emulator/include/scheduler.h`
* `emulator/src/scheduler.cpp`
* `emulator/include/emulator_core.h`
* `emulator/src/emulator_core.cpp`
* `emulator/src/main.cpp`

Only if narrow read-only snapshot/accessor plumbing is required.

## Files to Create

Create milestone report:

* `artifacts/reports/M55_frame_timing_scheduler_panel_report.md`

## Constraints

Do not:

* refactor the scheduler subsystem
* change timing semantics
* add timing control UI
* add performance profiling systems
* add dependencies
* modify ROM loading
* modify deterministic scheduling
* introduce speculative abstractions

Keep this milestone minimal.

## Verification Commands

Run:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Runtime checks:

```bash
./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 1
```

```bash
env SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software timeout 2s ./build/super_z80 --sdl-input --rom rom/showcase/build/showcase.bin
```

```bash
env SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software SDL_AUDIODRIVER=dummy timeout 2s ./build/super_z80 --sdl-audio --rom rom/showcase/build/showcase.bin
```

If a narrow unit test is practical for any new timing snapshot accessor behavior, add it.

Confirm:

* build succeeds
* tests pass
* frontend launches without crash

Automated verification cannot fully confirm the ImGui panel visually; document that clearly in the report.

## Acceptance Criteria

* emulator builds successfully
* tests pass
* `Frame Timing & Scheduler` window appears
* frame count is visible
* scanline/scheduler timing state is visible
* panel refreshes every frame
* panel is read-only
* headless mode unchanged
* `docs/state_snapshot.md` updated
* `artifacts/reports/M55_frame_timing_scheduler_panel_report.md` created

## Suggested Branch

`m55_frame_timing_scheduler_panel`

## Suggested Commit Message

`M55: add frame timing and scheduler panel`

## Completion Report Format

Use:

* Summary
* Files Changed
* Files Created
* Commands Run
* Verification Results
* Issues Encountered
* Recommended Next Step

## Recommended Next Step

M56 — Emulator Snapshot Tools
