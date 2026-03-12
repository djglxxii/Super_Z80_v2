# Task Packet

## Title

M49 — Memory Viewer

## Goal

Implement the first general-purpose memory inspection tool in the Super_Z80 frontend: a read-only **Memory Viewer** panel that allows developers to inspect ROM and RAM contents during emulator execution.

This milestone establishes the baseline UI pattern for memory-oriented debug tooling and gives developers direct visibility into the system address space without modifying emulator state.

The panel must be **read-only** in this milestone.

Do not implement memory editing, breakpoints, search, disassembly, or write-back behavior.

## Repository Context

Recent frontend milestones completed:

- M45 — ImGui Integration
- M46 — Emulator Control Panel
- M47 — ROM Loader Interface
- M48 — CPU Debug Panel

The emulator frontend now supports:

- SDL + ImGui runtime
- emulator lifecycle controls
- interactive ROM loading/reloading
- CPU register/state visibility

The next step is exposing the memory map to developers through a simple viewer.

The deterministic emulator core and headless execution paths must remain unchanged.

## Required Read Order

Before making changes, read in this order:

1. `docs/plan.md`
2. `docs/state_snapshot.md`
3. `docs/architecture.md`
4. `docs/decisions.md`
5. `AGENTS.md`
6. `artifacts/reports/M48_cpu_debug_panel_report.md`

Then inspect the current frontend implementation:

- `emulator/frontend/frontend.h`
- `emulator/frontend/frontend.cpp`
- `emulator/frontend/debug_panels/debug_panel_host.h`
- `emulator/frontend/debug_panels/debug_panel_host.cpp`

And the current memory/bus-facing implementation:

- `emulator/include/emulator_core.h`
- `emulator/src/emulator_core.cpp`
- `emulator/include/bus.h`
- `emulator/src/bus.cpp`
- `emulator/include/memory.h`
- `emulator/src/memory.cpp`

If needed, inspect any existing ROM/RAM read accessors already used by tests or runtime code.

## Scope

Included:

- a read-only `Memory Viewer` ImGui panel
- address range selection / viewing support
- visible ROM and RAM inspection
- hex-oriented display
- basic address navigation
- refresh every frame

Excluded:

- memory editing
- search
- symbolic labels
- disassembly
- breakpoints
- watchpoints
- per-byte annotations
- export/save tools
- advanced scrolling/virtualization work beyond what is needed for a practical first viewer

This milestone is inspection-only.

## Milestone Definition

The Memory Viewer must provide visibility into emulator memory.

Features:

- ROM viewer
- RAM viewer
- hex display
- scrolling memory view
- address navigation

Verification:

- memory contents match emulator state
- address navigation works

Purpose:

Allow developers to inspect runtime memory.

## Architectural Constraints

Maintain strict separation between:

- emulator core
- platform shell
- frontend UI/debug layer

The emulator core must remain deterministic and UI-independent.

The frontend must observe memory state through narrow, read-only accessors or snapshots.

Do not expose mutable memory references to the frontend.

Do not add UI dependencies to core hardware modules.

## Memory Coverage Requirements

The viewer must support at minimum:

### ROM window

Display bytes from the ROM-mapped address space.

Expected relevant range:

- `0x0000` – `0x7FFF`

### RAM window

Display bytes from the RAM-mapped address space.

Expected relevant range:

- `0xC000` – `0xFFFF`

### Unmapped area

If the current platform exposes unmapped/open-bus ranges in the address space, they may be displayed using existing bus read semantics or omitted if that would create misleading behavior.

For this milestone, prioritize **clear ROM and RAM visibility** over exhaustive full-bus modeling in the panel.

## Panel Layout

Create a window titled:

```text
Memory Viewer
````

Suggested minimal layout:

* region selector:

  * `ROM`
  * `RAM`
* starting address input
* memory table / rows of hex bytes
* optional ASCII column if easy to add cleanly

A simple acceptable format is:

```text
Memory Viewer

Region: ROM
Start: 0000

0000: 3E 01 D3 E0 21 00 C0 36 00 ...
0010: ...
0020: ...
```

Exact formatting may vary, but it must remain readable and compact.

## Implementation Requirements

### 1. Create Memory Viewer Panel

Extend the existing debug panel host system.

Do not bypass the current debug panel architecture.

The Memory Viewer must render through the same frontend/debug-panel system used for the CPU panel.

### 2. Read-only memory access

Retrieve ROM/RAM bytes from the emulator runtime using existing public accessors if possible.

If narrow new accessors are needed, they must be:

* read-only
* minimal in scope
* appropriate for frontend inspection only

Avoid exposing raw mutable containers to the UI.

### 3. Address navigation

Provide a practical way to change the visible memory start address.

Acceptable options include:

* hex input field for start address
* step buttons
* page up/page down style buttons
* region-relative clamped address movement

This milestone does not require polished UX, only practical navigation.

### 4. Display format

Render bytes in a hex viewer style.

Reasonable default:

* 16 bytes per row

Display enough rows to make the panel useful.

If ASCII rendering is easy and clean, include it. If not, omit it and keep the milestone narrow.

### 5. Region handling

At minimum, support viewing:

* ROM region
* RAM region

The currently selected region should clamp navigation to valid bounds.

### 6. Refresh behavior

The viewer must update every frame and reflect current emulator state.

The panel must not trigger emulator execution or otherwise alter runtime timing.

### 7. UI integration

The panel should appear automatically alongside the current frontend panels.

No panel visibility toggles are required in this milestone unless the existing debug panel structure already naturally supports them.

## Files Expected to Change

Likely:

* `emulator/frontend/debug_panels/debug_panel_host.h`
* `emulator/frontend/debug_panels/debug_panel_host.cpp`
* `emulator/frontend/frontend.h`
* `emulator/frontend/frontend.cpp`

Possibly:

* `emulator/include/emulator_core.h`
* `emulator/src/emulator_core.cpp`
* `emulator/include/bus.h`
* `emulator/src/bus.cpp`
* `emulator/include/memory.h`
* `emulator/src/memory.cpp`

Only if small read-only accessors/snapshots are required.

## Files to Create

Create milestone report:

```text
artifacts/reports/M49_memory_viewer_report.md
```

## Constraints

Do not:

* refactor the emulator core
* implement memory editing
* implement search
* implement disassembly
* implement watchpoints/breakpoints
* add third-party dependencies
* modify deterministic scheduling
* modify ROM loading behavior
* add speculative debugger abstractions

Keep this milestone small and focused.

## Verification Commands

Run:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Then run frontend/runtime smoke checks:

```bash
./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 1
```

```bash
env SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software timeout 2s ./build/super_z80 --sdl-input --rom rom/showcase/build/showcase.bin
```

```bash
env SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software SDL_AUDIODRIVER=dummy timeout 2s ./build/super_z80 --sdl-audio --rom rom/showcase/build/showcase.bin
```

If a small targeted unit test is practical for any new read-only memory snapshot/accessor behavior, add it.

Automated verification cannot fully validate the ImGui panel visually; document that clearly in the report.

## Acceptance Criteria

* emulator builds successfully
* tests pass
* `Memory Viewer` window appears
* ROM bytes are visible
* RAM bytes are visible
* address navigation works
* panel updates during execution
* panel is read-only
* headless mode unchanged
* `docs/state_snapshot.md` updated
* `artifacts/reports/M49_memory_viewer_report.md` created

## Suggested Branch

```text
m49_memory_viewer
```

## Suggested Commit Message

```text
M49: add memory viewer panel
```

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

M50 — VRAM Viewer

