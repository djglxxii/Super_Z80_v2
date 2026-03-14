# Task Packet

## Title

M57 — Integrated Debug Overlay

## Goal

Implement an **Integrated Debug Overlay** for the Super_Z80 frontend that organizes the growing set of debug panels into a coherent, developer-friendly UI surface.

This milestone is about **UI integration and usability**, not new emulator hardware features.

The overlay should make the existing debug tools easier to use by providing:

- a top-level debug menu organization
- panel visibility toggles
- a cleaner integrated layout
- basic persistence of layout/visibility if that can be done narrowly and safely

This milestone must preserve deterministic execution and must not alter emulator hardware behavior.

Do not implement new hardware inspection panels in this milestone.

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
- M55 — Frame Timing & Scheduler Panel
- M56 — Emulator Snapshot Tools

The frontend now includes multiple always-on debug panels. The next planned milestone is **M57 — Integrated Debug Overlay**, whose purpose is to combine those panels into a cohesive interface and improve usability.

## Required Read Order

Before making changes read:

1. `docs/plan.md`
2. `docs/state_snapshot.md`
3. `docs/architecture.md`
4. `docs/decisions.md`
5. `AGENTS.md`
6. `artifacts/reports/M56_emulator_snapshot_tools_report.md`

Then inspect:

Frontend/UI:

- `emulator/frontend/frontend.h`
- `emulator/frontend/frontend.cpp`
- `emulator/frontend/debug_panels/debug_panel_host.h`
- `emulator/frontend/debug_panels/debug_panel_host.cpp`

Runtime integration:

- `emulator/src/main.cpp`

Also inspect how ImGui is currently initialized and where existing panels are rendered.

## Scope

Included:

- integrated debug overlay/menu organization
- visibility toggles for existing panels
- improved grouping of current debug tools
- optional minimal layout persistence if supported narrowly through existing ImGui configuration behavior
- frame-synchronized UI integration
- documentation update
- milestone report

Excluded:

- new hardware inspection panels
- docking-heavy redesign if it significantly expands scope
- custom layout system beyond narrow persistence
- theming/styling overhaul
- profiler tools
- debug scripting systems
- major frontend architecture rewrite

This milestone is about **organizing existing tools**, not inventing a new UI framework.

## Milestone Definition

The plan defines M57 as:

Features:

- dockable panels
- window layout persistence
- debug menu organization

Verification:

- UI layout stable across runs

Purpose:

- provide a professional development environment

For this milestone, prioritize:

1. debug menu organization
2. visibility toggles
3. stable integrated layout

Docking and persistence are desirable, but should only be implemented if they fit the current codebase cleanly and narrowly.

## Architectural Constraints

Maintain strict separation between:

- emulator core
- platform shell
- frontend debug UI

This milestone must stay entirely in the frontend/shell/UI layer.

Do not modify emulator hardware behavior.

Do not move UI concerns into core subsystems.

Do not introduce dependencies beyond what is already in the repo unless absolutely unavoidable, and avoid that if possible.

## Overlay Requirements

At minimum, the integrated overlay must provide a coherent way to control visibility of the existing panels, including at least:

- Emulator Control
- CPU Debug
- Memory Viewer
- VRAM Viewer
- Sprite Debug
- DMA Debug
- Audio Debug
- Input Visualization
- Frame Timing & Scheduler

Use the actual panel names currently present in the codebase.

## Implementation Requirements

### 1. Debug menu / overlay organization

Add a top-level debug-oriented UI organization surface.

Acceptable approaches include:

- main menu bar with a `Debug` menu
- `View` or `Panels` menu
- equivalent minimal structure consistent with the current frontend

The user must be able to toggle individual panels on/off.

### 2. Panel visibility state

Introduce a narrow panel visibility state model in the frontend/debug host.

Requirements:

- each existing panel has a visibility flag
- flags are frontend-owned
- toggling visibility does not affect emulator execution
- default visibility can be sensible and minimal

Keep this state small and explicit.

### 3. Layout integration

Improve panel arrangement so the debug UI feels like one integrated environment rather than a set of unrelated always-on windows.

A minimal acceptable implementation may use:

- consistent initial window placement/sizing
- controlled visibility defaults
- menu-driven panel access

If ImGui docking is already easy to enable cleanly with the vendored version and current setup, it may be used. If it causes scope expansion, skip docking and keep the milestone narrow.

### 4. Persistence

If layout/visibility persistence can be achieved safely through existing ImGui ini/config behavior or a similarly narrow mechanism, include it.

If not, implement stable defaults and note the limitation in the report.

Do not build a custom persistence framework for this milestone.

### 5. Existing panel integration

All existing panels must continue to work as before, just under better organization.

Do not rewrite each panel unnecessarily.

### 6. Refresh behavior

The overlay must update every rendered frame.

The overlay must not alter emulator execution, timing, or scheduler behavior.

## Files Expected to Change

Likely:

- `emulator/frontend/debug_panels/debug_panel_host.h`
- `emulator/frontend/debug_panels/debug_panel_host.cpp`
- `emulator/frontend/frontend.h`
- `emulator/frontend/frontend.cpp`
- `emulator/src/main.cpp`

Possibly small ImGui initialization changes if menu bar/docking flags/layout persistence need them.

## Files to Create

Create milestone report:

- `artifacts/reports/M57_integrated_debug_overlay_report.md`

Only create additional frontend helper files if truly necessary. Keep this minimal.

## Constraints

Do not:

- refactor the emulator core
- implement new subsystem panels
- add profiling tools
- add dependencies
- change deterministic scheduling behavior
- change ROM loading behavior
- introduce a custom large UI framework

Keep this milestone focused on integration and usability.

## Verification Commands

Run:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
````

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

Confirm:

* build succeeds
* tests pass
* frontend launches without crash
* panel visibility integration path is operational as far as practical in this environment

Automated visual confirmation of menu behavior and panel arrangement is not expected; document that clearly in the report.

## Acceptance Criteria

* emulator builds successfully
* tests pass
* existing debug panels remain functional
* panel visibility can be toggled from an integrated overlay/menu
* layout is more coherent than the current always-on arrangement
* any implemented persistence behaves stably across runs
* headless mode unchanged
* `docs/state_snapshot.md` updated
* `artifacts/reports/M57_integrated_debug_overlay_report.md` created

## Suggested Branch

`m57_integrated_debug_overlay`

## Suggested Commit Message

`M57: add integrated debug overlay`

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

M58 — Frontend Stabilization
