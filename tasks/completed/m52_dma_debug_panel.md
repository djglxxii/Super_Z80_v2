# Task Packet

## Title

M52 — DMA Debug Panel

## Goal

Implement a read-only **DMA Debug Panel** in the Super_Z80 frontend that allows developers to inspect the current DMA engine state and recent DMA activity.

This panel provides visibility into DMA transfers occurring within the system and establishes the debugging surface for validating DMA behavior used by graphics and other subsystems.

The panel must remain **read-only** in this milestone.

Do not implement DMA control, editing, tracing systems, or performance profiling.

## Repository Context

Recent completed milestones:

M45 — ImGui Integration  
M46 — Emulator Control Panel  
M47 — ROM Loader Interface  
M48 — CPU Debug Panel  
M49 — Memory Viewer  
M50 — VRAM Viewer  
M51 — Sprite Debug Panel  

The frontend now exposes visibility into:

• CPU registers  
• RAM  
• VRAM  
• Sprite attribute table  

The next debug surface is **DMA activity and register state**.

## Required Read Order

Before making changes read:

1. `docs/plan.md`
2. `docs/state_snapshot.md`
3. `docs/architecture.md`
4. `docs/decisions.md`
5. `AGENTS.md`
6. `artifacts/reports/M51_sprite_debug_panel_report.md`

Then inspect:

Frontend:

- `emulator/frontend/frontend.h`
- `emulator/frontend/frontend.cpp`
- `emulator/frontend/debug_panels/debug_panel_host.h`
- `emulator/frontend/debug_panels/debug_panel_host.cpp`

DMA subsystem:

- `emulator/include/dma.h`
- `emulator/src/dma.cpp`
- `emulator/include/emulator_core.h`
- `emulator/src/emulator_core.cpp`

If the DMA engine is embedded in another subsystem, inspect that implementation instead.

## Scope

Included:

- read-only DMA debug panel
- DMA register/state display
- transfer status visibility
- frame-synchronized refresh

Excluded:

- DMA editing
- manual DMA triggering
- transfer tracing history
- profiling statistics
- transfer visualization
- memory diff viewers

This milestone is **state inspection only**.

## Milestone Definition

The DMA Debug Panel must show the DMA engine state.

Expected information to display:

### DMA Registers

Example values (actual names must match implementation):

Source Address  
Destination Address  
Transfer Length  
Control / Mode Register  

### Transfer Status

Examples:

DMA active  
DMA idle  
DMA complete  

### Optional Status Fields (if available)

Current transfer progress  
Remaining bytes  
Transfer direction  

Only include fields that exist in the current implementation.

## Panel Layout

Create a window titled:

DMA Debug

Example layout:

DMA Debug

Source:      C000
Destination: 8000
Length:      0200
Mode:        VRAM_TRANSFER

Status:      ACTIVE
Remaining:   0012

Exact formatting may vary but must remain readable and compact.

## Implementation Requirements

### 1. Create DMA Debug Panel

Extend the existing debug panel host system.

Do not introduce a separate UI architecture.

Render alongside:

CPU Debug  
Memory Viewer  
VRAM Viewer  
Sprite Debug

### 2. Access DMA State

Retrieve DMA state through a narrow read-only accessor.

Possible approaches:

- DMA snapshot struct
- DMA register snapshot
- const accessors

Avoid exposing mutable references.

If DMA is implemented as part of another subsystem, extract only the required state fields.

### 3. Display Fields

Display:

Source address  
Destination address  
Transfer length  
Control/mode register  
Transfer status

Optional fields may be displayed if available.

### 4. Refresh Behavior

The panel must update every rendered frame.

The panel must not alter emulator execution.

The panel must not trigger DMA.

### 5. UI Integration

The panel should appear automatically with the existing debug panels.

Use existing panel rendering conventions.

## Files Expected to Change

Likely:

- emulator/frontend/debug_panels/debug_panel_host.h
- emulator/frontend/debug_panels/debug_panel_host.cpp
- emulator/frontend/frontend.h
- emulator/frontend/frontend.cpp

Possibly:

- emulator/include/dma.h
- emulator/src/dma.cpp
- emulator/include/emulator_core.h
- emulator/src/emulator_core.cpp

Only if narrow read-only snapshot accessors are required.

## Files to Create

Create milestone report:

artifacts/reports/M52_dma_debug_panel_report.md

## Constraints

Do not:

- refactor the DMA subsystem
- allow DMA editing
- implement tracing systems
- add dependencies
- modify deterministic scheduling
- modify ROM loading
- introduce debugger abstractions

Keep this milestone minimal.

## Verification Commands

Run:

cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure

Runtime checks:

./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 1

env SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software timeout 2s ./build/super_z80 --sdl-input --rom rom/showcase/build/showcase.bin

env SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software SDL_AUDIODRIVER=dummy timeout 2s ./build/super_z80 --sdl-audio --rom rom/showcase/build/showcase.bin

Confirm:

• build succeeds  
• tests pass  
• frontend launches without crash  

Visual verification of the ImGui panel cannot be automated.

## Acceptance Criteria

- emulator builds successfully
- tests pass
- DMA Debug window appears
- DMA registers displayed
- transfer status visible
- panel refreshes every frame
- panel is read-only
- headless mode unchanged
- docs/state_snapshot.md updated
- artifacts/reports/M52_dma_debug_panel_report.md created

## Suggested Branch

m52_dma_debug_panel

## Suggested Commit Message

M52: add DMA debug panel

## Completion Report Format

Summary  
Files Changed  
Files Created  
Commands Run  
Verification Results  
Issues Encountered  
Recommended Next Step

## Recommended Next Step

M53 — Tile Viewer
```
