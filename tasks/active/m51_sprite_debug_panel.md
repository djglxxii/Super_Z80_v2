Use the following coding-agent prompt for the next milestone.

```text
# Task Packet

## Title

M51 — Sprite Debug Panel

## Goal

Implement a read-only **Sprite Debug Panel** in the Super_Z80 frontend that allows developers to inspect the current sprite table state used by the VDP.

This panel provides visibility into sprite attributes stored in VRAM and establishes the first debugging tool focused on the graphics object layer.

The panel must remain **read-only** in this milestone.

Do not implement sprite rendering previews, editing tools, or collision debugging yet.

## Repository Context

Recent completed milestones:

- M45 — ImGui Integration
- M46 — Emulator Control Panel
- M47 — ROM Loader Interface
- M48 — CPU Debug Panel
- M49 — Memory Viewer
- M50 — VRAM Viewer

The emulator frontend now supports:

- runtime control (pause/reset/frame-step)
- ROM loading and reload
- CPU inspection
- system RAM inspection
- VRAM inspection

The next logical debug surface is **sprite attribute inspection**.

## Required Read Order

Before making changes read:

1. `docs/plan.md`
2. `docs/state_snapshot.md`
3. `docs/architecture.md`
4. `docs/decisions.md`
5. `AGENTS.md`
6. `artifacts/reports/M50_vram_viewer_report.md`

Then inspect:

Frontend:

- `emulator/frontend/frontend.h`
- `emulator/frontend/frontend.cpp`
- `emulator/frontend/debug_panels/debug_panel_host.h`
- `emulator/frontend/debug_panels/debug_panel_host.cpp`

Video subsystem:

- `emulator/include/vdp.h`
- `emulator/src/vdp.cpp`
- `emulator/include/emulator_core.h`
- `emulator/src/emulator_core.cpp`

If necessary inspect:

- sprite attribute table layout
- sprite count limits
- VRAM sprite region definitions

## Scope

Included:

- read-only sprite debug panel
- sprite attribute table viewer
- display of sprite properties
- frame-synchronized refresh

Excluded:

- sprite preview rendering
- tile decoding
- palette viewer
- sprite editing
- collision debugging
- sprite layering visualization
- performance analysis

This milestone focuses on **attribute inspection only**.

## Milestone Definition

The Sprite Debug Panel must show sprite entries extracted from VRAM.

Each sprite entry should include at minimum:

- sprite index
- X position
- Y position
- tile index
- attribute flags

Verification:

- sprite data updates when VRAM changes
- sprite list remains within platform limits

Purpose:

Provide a basic debugging view of sprite objects.

## Architectural Constraints

Maintain strict separation between:

- emulator core
- platform shell
- frontend debug UI

The frontend must not modify VDP state.

Sprite data must be obtained through **read-only snapshot accessors** from the emulator core or VDP subsystem.

Do not expose mutable references.

Do not add UI dependencies to the VDP.

## Sprite Data Coverage

The panel must display the entire sprite attribute table.

Expected typical structure (example):

```

Sprite Table Entry

Byte 0: Y position
Byte 1: X position
Byte 2: Tile index
Byte 3: Attribute flags

```

Use the exact format defined by the VDP implementation.

If sprite count is fixed (example: 64 sprites), display all entries.

If fewer sprites are active, unused entries may still be shown.

## Panel Layout

Create a window titled:

```

Sprite Debug

```

Suggested layout:

```

Sprite Debug

## Index | X | Y | Tile | Attr

0     | 32 | 40 | 12 | 80
1     | 16 | 72 | 05 | 00
2     | ...

```

A simple table layout using ImGui is acceptable.

## Implementation Requirements

### 1. Create Sprite Debug Panel

Extend the existing debug panel host system.

Do not create a separate debug UI architecture.

Render alongside:

- CPU Debug
- Memory Viewer
- VRAM Viewer

### 2. Access Sprite Data

Retrieve sprite attribute table data through a narrow read-only accessor.

Possible methods:

- VDP snapshot struct
- sprite table snapshot
- VRAM region decode helper

Avoid exposing mutable references.

### 3. Display Sprite Attributes

For each sprite entry display:

- index
- X coordinate
- Y coordinate
- tile index
- attribute flags

If the attribute byte contains flags (flip, palette, priority), display either raw value or decoded bits.

Raw display is acceptable for this milestone.

### 4. Refresh Behavior

The panel must refresh every rendered frame.

The panel must not trigger emulator execution.

The panel must not modify runtime timing.

### 5. UI Integration

The panel should appear automatically with other debug panels.

Use existing debug panel patterns for consistency.

## Files Expected to Change

Likely:

- `emulator/frontend/debug_panels/debug_panel_host.h`
- `emulator/frontend/debug_panels/debug_panel_host.cpp`
- `emulator/frontend/frontend.h`
- `emulator/frontend/frontend.cpp`

Possibly:

- `emulator/include/vdp.h`
- `emulator/src/vdp.cpp`
- `emulator/include/emulator_core.h`
- `emulator/src/emulator_core.cpp`

Only if narrow read-only sprite snapshot accessors are required.

## Files to Create

Create milestone report:

```

artifacts/reports/M51_sprite_debug_panel_report.md

```

## Constraints

Do not:

- refactor the video subsystem
- implement sprite rendering previews
- implement tile decoding
- allow editing sprite attributes
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

Runtime checks:

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

Visual verification of the ImGui panel cannot be automated.

## Acceptance Criteria

- emulator builds successfully
- tests pass
- `Sprite Debug` window appears
- sprite table entries visible
- sprite attributes displayed
- panel refreshes every frame
- panel is read-only
- headless mode unchanged
- `docs/state_snapshot.md` updated
- `artifacts/reports/M51_sprite_debug_panel_report.md` created

## Suggested Branch

```

m51_sprite_debug_panel

```

## Suggested Commit Message

```

M51: add sprite debug panel

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

M52 — Tile Viewer
```
