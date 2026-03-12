# Task Packet

## Title

M50 — VRAM Viewer

## Goal

Implement a read-only **VRAM Viewer** panel in the Super_Z80 frontend that allows developers to inspect the current contents of Video RAM (VRAM) during emulator execution.

This panel extends the existing debug tooling (CPU Debug + Memory Viewer) to include visibility into the graphics subsystem.

The panel must remain **read-only** and purely observational.

Do not implement graphics decoding, tile rendering, palette visualization, or editing features in this milestone.

## Repository Context

Recent completed milestones:

M45 — ImGui Integration  
M46 — Emulator Control Panel  
M47 — ROM Loader Interface  
M48 — CPU Debug Panel  
M49 — Memory Viewer  

The emulator frontend now provides:

- SDL + Dear ImGui runtime
- emulator run/pause/reset/step controls
- ROM loading/reloading
- CPU state inspection
- system memory inspection

The next step is exposing **video subsystem memory (VRAM)** for debugging graphics state.

This milestone only exposes the raw VRAM data.

## Required Read Order

Before making changes read:

1. `docs/plan.md`
2. `docs/state_snapshot.md`
3. `docs/architecture.md`
4. `docs/decisions.md`
5. `AGENTS.md`
6. `artifacts/reports/M49_memory_viewer_report.md`

Then inspect the current frontend:

- `emulator/frontend/frontend.h`
- `emulator/frontend/frontend.cpp`
- `emulator/frontend/debug_panels/debug_panel_host.h`
- `emulator/frontend/debug_panels/debug_panel_host.cpp`

And inspect video subsystem implementation:

- `emulator/include/vdp.h`
- `emulator/src/vdp.cpp`
- `emulator/include/emulator_core.h`
- `emulator/src/emulator_core.cpp`

If necessary inspect:

- video memory layout
- VDP VRAM storage container
- existing VDP read functions

## Scope

Included:

- read-only VRAM viewer panel
- hex display of VRAM
- address navigation
- frame-synchronized refresh

Excluded:

- tile decoding
- sprite decoding
- tilemap rendering
- palette viewer
- editing VRAM
- search
- symbol labeling
- graphics inspector tools

This milestone is **raw VRAM inspection only**.

## Milestone Definition

The VRAM Viewer must display:

- raw VRAM bytes
- hex formatted memory
- navigation within VRAM bounds

Verification:

- VRAM contents update during execution
- addresses remain clamped to VRAM limits

Purpose:

Allow developers to debug graphics state and validate rendering pipeline behavior.

## Architectural Constraints

Maintain strict separation between:

- emulator core
- platform shell
- frontend debug UI

The frontend must not manipulate VDP state.

VRAM must be exposed through **read-only snapshot accessors** from the emulator core.

Do not expose mutable references to VRAM buffers.

Do not introduce UI dependencies into the VDP implementation.

## VRAM Coverage

Expose the entire VRAM region managed by the VDP.

Typical expected size (depending on platform configuration):

Example:

```

VRAM size: 64 KB
Range: 0x0000 – 0xFFFF

```

Use the actual VRAM size defined in the emulator implementation.

Navigation must clamp within this range.

## Panel Layout

Create a window titled:

```

VRAM Viewer

```

Example layout:

```

VRAM Viewer

Start: 0000

0000: 3F 00 21 04 0A 02 11 ...
0010: 00 00 00 00 00 00 00 ...
0020: ...

```

Use the same visual conventions established by the Memory Viewer where practical.

Recommended:

- 16 bytes per row
- hex display
- optional ASCII column if easily reusable

## Implementation Requirements

### 1. Create VRAM Viewer Panel

Extend the existing debug panel host system.

Do not create a separate debug UI system.

Render the VRAM Viewer alongside:

- CPU Debug
- Memory Viewer

### 2. Access VRAM

Retrieve VRAM contents via a narrow read-only accessor.

Possible approaches:

- VRAM snapshot copy
- const view of VRAM data
- byte-read accessor

Avoid exposing raw mutable arrays to the frontend.

### 3. Address Navigation

Allow developers to navigate VRAM.

Acceptable navigation methods:

- hex start address input
- page up/down
- region clamped scrolling

The viewer must not allow addresses outside VRAM bounds.

### 4. Display Format

Use hex viewer format.

Recommended:

- 16 bytes per row
- multiple rows per panel

Reuse code patterns from the Memory Viewer where practical.

### 5. Refresh Behavior

The viewer must refresh every rendered frame.

It must reflect current VRAM state.

The panel must not alter emulator execution.

### 6. UI Integration

The panel should appear automatically with other debug panels.

Do not add menu toggles in this milestone unless the debug panel system already supports them cleanly.

## Files Expected to Change

Likely:

- `emulator/frontend/debug_panels/debug_panel_host.h`
- `emulator/frontend/debug_panels/debug_panel_host.cpp`
- `emulator/frontend/frontend.h`
- `emulator/frontend/frontend.cpp`

Possibly:

- `emulator/include/emulator_core.h`
- `emulator/src/emulator_core.cpp`
- `emulator/include/vdp.h`
- `emulator/src/vdp.cpp`

Only if narrow read-only VRAM accessors are required.

## Files to Create

Create milestone report:

```

artifacts/reports/M50_vram_viewer_report.md

```

## Constraints

Do not:

- refactor the video subsystem
- implement tile decoding
- implement graphics viewers
- allow editing VRAM
- add dependencies
- modify deterministic scheduling
- modify ROM loading
- introduce debugger abstractions

Keep this milestone minimal.

## Verification Commands

Run:

```

cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure

```

Run runtime checks:

```

./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 1

```
```

env SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software timeout 2s ./build/super_z80 --sdl-input --rom rom/showcase/build/showcase.bin

```
```

env SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software SDL_AUDIODRIVER=dummy timeout 2s ./build/super_z80 --sdl-audio --rom rom/showcase/build/showcase.bin

```

Confirm:

- build succeeds
- tests pass
- frontend launches without crash

Document that visual verification of the ImGui panel cannot be automated.

## Acceptance Criteria

- emulator builds successfully
- tests pass
- `VRAM Viewer` window appears
- VRAM bytes visible
- navigation works
- viewer refreshes during execution
- viewer is read-only
- headless mode unchanged
- `docs/state_snapshot.md` updated
- `artifacts/reports/M50_vram_viewer_report.md` created

## Suggested Branch

```

m50_vram_viewer

```

## Suggested Commit Message

```

M50: add VRAM viewer panel

```

## Completion Report Format

Use:

Summary  
Files Changed  
Files Created  
Commands Run  
Verification Results  
Issues Encountered  
Recommended Next Step

## Recommended Next Step

M51 — Tile Viewer
```
