# Task Packet

## Title

M54 — Input Visualization Panel

## Goal

Implement a read-only **Input Visualization Panel** in the Super_Z80 frontend that shows the current controller/input state during emulator execution.

This milestone provides developer visibility into input handling so ROM authors can confirm button state, active-low behavior, and current runtime input values without relying on logs or headless inspection.

The panel must remain **read-only** in this milestone.

Do not implement input injection, rebinding, scripting controls, recording UI, or multi-controller expansion beyond what already exists.

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

The frontend now exposes visibility into CPU, memory, VRAM, sprites, DMA, and audio. The next milestone in the plan is **M54 — Input Visualization Panel**.

The deterministic emulator core and headless execution paths must remain unchanged.

## Required Read Order

Before making changes read:

1. `docs/plan.md`
2. `docs/state_snapshot.md`
3. `docs/architecture.md`
4. `docs/decisions.md`
5. `AGENTS.md`
6. `artifacts/reports/M53_audio_debug_panel_report.md`

Then inspect:

Frontend:

- `emulator/frontend/frontend.h`
- `emulator/frontend/frontend.cpp`
- `emulator/frontend/debug_panels/debug_panel_host.h`
- `emulator/frontend/debug_panels/debug_panel_host.cpp`

Input/runtime path:

- `emulator/include/io.h`
- `emulator/src/io.cpp`
- `emulator/include/bus.h`
- `emulator/src/bus.cpp`
- `emulator/include/emulator_core.h`
- `emulator/src/emulator_core.cpp`
- `emulator/src/main.cpp`

Also inspect any existing input-related tests and SDL input mapping code already used by the shell.

## Scope

Included:

- read-only `Input Visualization` ImGui panel
- current controller/button state display
- active-low/raw port visibility if available
- frame-synchronized refresh
- optional current source summary if already naturally available

Excluded:

- input rebinding UI
- input injection from UI
- headless input script editor
- record/replay controls
- second-controller expansion unless already present
- input history graphs unless trivial and already naturally available

This milestone is **state inspection only**.

## Milestone Definition

The Input Visualization Panel must show the current controller/input state.

At minimum, display:

- button indicators for currently supported controls
- active-low/raw input state visibility
- PAD1 / PAD1_SYS values or equivalent current port-visible state if available

Verification:

- panel updates when buttons are pressed
- displayed values match emulator input state

Purpose:

- assist debugging input handling

## Architectural Constraints

Maintain strict separation between:

- emulator core
- platform shell
- frontend debug UI

The frontend must not modify input state.

Input state must be obtained through narrow read-only snapshot accessors or existing runtime-visible state handoff.

Do not expose mutable references.

Do not add UI dependencies to the input subsystem.

## Input Coverage Requirements

Display the current state for the currently supported controller inputs.

Expected controls based on existing shell mappings likely include:

- Up
- Down
- Left
- Right
- A
- B
- Start

Use the actual supported control set from the current implementation.

If the system already distinguishes between:

- decoded logical button states
- raw active-low port values

then display both.

If only one clean read-only representation exists, display that accurately rather than inventing more layers.

## Panel Layout

Create a window titled:

```text
Input Visualization
````

Suggested layout:

```text
Input Visualization

Buttons
Up:    ON
Down:  OFF
Left:  OFF
Right: ON
A:     OFF
B:     ON
Start: OFF

Raw Ports
PAD1:     0xEF
PAD1_SYS: 0xFE
```

A compact table or grouped text layout is acceptable.

If useful and easy, button indicators may be shown as colored labels or checkboxes, but they must remain read-only.

## Implementation Requirements

### 1. Create Input Visualization Panel

Extend the existing debug panel host system.

Do not introduce a separate UI architecture.

Render alongside the existing panels:

* CPU Debug
* Memory Viewer
* VRAM Viewer
* Sprite Debug
* DMA Debug
* Audio Debug

### 2. Access input state

Retrieve input state through narrow read-only accessors or snapshots.

Possible approaches:

* IO snapshot struct
* Bus-level input snapshot
* EmulatorCore-level input snapshot

Avoid exposing mutable references.

Prefer a small snapshot API consistent with the recent CPU/memory/VRAM/sprite/DMA/audio accessors.

### 3. Display logical buttons

Show the current logical button state for each supported control.

If the current implementation stores button state in decoded form, use that.

### 4. Display raw port-visible values

If available through existing bus/IO semantics, display the current port-visible values for controller-related input ports.

This is valuable because ROM developers care about what the hardware-visible read path sees.

### 5. Refresh behavior

The panel must refresh every rendered frame.

The panel must not trigger emulator execution.

The panel must not modify input, timing, or scheduler behavior.

### 6. UI integration

The panel should appear automatically with the existing debug panels.

Use existing rendering conventions for consistency.

## Files Expected to Change

Likely:

* `emulator/frontend/debug_panels/debug_panel_host.h`
* `emulator/frontend/debug_panels/debug_panel_host.cpp`
* `emulator/frontend/frontend.h`
* `emulator/frontend/frontend.cpp`

Possibly:

* `emulator/include/io.h`
* `emulator/src/io.cpp`
* `emulator/include/bus.h`
* `emulator/src/bus.cpp`
* `emulator/include/emulator_core.h`
* `emulator/src/emulator_core.cpp`
* `emulator/src/main.cpp`

Only if narrow read-only snapshot/accessor plumbing is required.

## Files to Create

Create milestone report:

* `artifacts/reports/M54_input_visualization_panel_report.md`

## Constraints

Do not:

* refactor the input subsystem
* add input editing or rebinding
* add input recording/replay UI
* add dependencies
* modify deterministic scheduling
* modify ROM loading
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

If a narrow unit test is practical for any new input snapshot accessor behavior, add it.

Confirm:

* build succeeds
* tests pass
* frontend launches without crash

Automated verification cannot fully confirm the ImGui panel visually; document that clearly in the report.

## Acceptance Criteria

* emulator builds successfully
* tests pass
* `Input Visualization` window appears
* logical button states are visible
* raw input/port-visible state is visible if available
* panel refreshes every frame
* panel is read-only
* headless mode unchanged
* `docs/state_snapshot.md` updated
* `artifacts/reports/M54_input_visualization_panel_report.md` created

## Suggested Branch

`m54_input_visualization_panel`

## Suggested Commit Message

`M54: add input visualization panel`

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

M55 — Frame Timing & Scheduler Panel
