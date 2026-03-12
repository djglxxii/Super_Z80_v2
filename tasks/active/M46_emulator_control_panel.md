# Task Packet

## Title

M46 — Emulator Control Panel

## Goal

Implement the first interactive runtime control interface for the Super_Z80 emulator using the ImGui integration introduced in M45.

This milestone introduces a minimal **Emulator Control Panel** that allows developers to control emulator execution during runtime. The panel will provide basic lifecycle control without altering deterministic emulator behavior.

The goal is to allow developers to:

- pause execution
- resume execution
- reset the emulator
- step execution one frame at a time

This is the first developer-facing control interface for the emulator.

Do not implement ROM loading or subsystem inspection yet.

## Repository Context

M44 established the frontend module scaffold.

M45 integrated Dear ImGui into the SDL runtime and created a placeholder diagnostic window.

The repository now supports:

- frontend lifecycle
- ImGui initialization
- ImGui rendering
- a minimal diagnostic window

This milestone extends the frontend UI to provide **runtime execution control**.

The deterministic emulator core and headless execution paths must remain unchanged.

## Required Read Order

Before editing, read in this order:

1. `docs/plan.md`
2. `docs/state_snapshot.md`
3. `docs/architecture.md`
4. `docs/decisions.md`
5. `AGENTS.md`
6. `artifacts/reports/M45_imgui_integration_report.md`
7. frontend files:
   - `emulator/frontend/frontend.h`
   - `emulator/frontend/frontend.cpp`
   - `emulator/frontend/debug_panels/debug_panel_host.*`
8. shell/runtime entry path:
   - `emulator/src/main.cpp`

## Scope

Included in scope:

- implement a simple ImGui **Emulator Control Panel**
- add runtime control buttons
- integrate control logic with the existing emulator execution loop
- preserve deterministic stepping semantics
- update state snapshot documentation
- create milestone completion report

Excluded from scope:

- ROM loader UI
- debug subsystem panels
- memory/VRAM inspection
- breakpoint systems
- instruction stepping
- docking/layout persistence
- major renderer or architecture changes

## Milestone Definition

M46 adds the first functional runtime UI tools.

Features:

- run / pause toggle
- reset emulator
- step one frame

Verification:

- controls manipulate emulator execution correctly
- frame stepping advances exactly one frame
- emulator state remains deterministic

Purpose:

Provide minimal runtime interaction capability for developers.

## Architectural Constraints

Maintain strict subsystem separation:

core emulator
platform shell
frontend UI/debug layer

The emulator core must remain deterministic and UI-independent.

The frontend must interact with the emulator only through existing runtime interfaces.

Do not introduce UI dependencies into core hardware modules.

Execution control should be handled through the existing runtime execution loop rather than altering CPU scheduling logic.

## Implementation Requirements

### 1. Control Panel UI

Add an ImGui window titled:

```

Emulator Control

```

This window should contain:

- Run/Pause toggle button
- Reset button
- Step Frame button

Display optional status text such as:

```

Execution State: Running / Paused
Frame Counter: <value>

```

Keep layout minimal.

### 2. Execution State Handling

Introduce a simple execution state in the frontend layer, such as:

```

running
paused
step_frame_requested

```

This state should influence the emulator execution loop but must not alter core timing behavior.

### 3. Frame Stepping

Frame stepping must advance the emulator exactly one frame.

After stepping, execution returns to paused state.

Ensure frame stepping is deterministic and consistent with scheduler behavior.

### 4. Reset Handling

The Reset button should trigger the same reset logic used by normal emulator startup.

Avoid duplicating reset logic.

### 5. Frontend Integration

Extend the frontend module rather than bypassing it.

Likely locations:

```

emulator/frontend/frontend.cpp

```

or

```

emulator/frontend/debug_panels/

```

depending on current panel hosting structure.

### 6. Documentation and Report

Update:

```

docs/state_snapshot.md

```

Create:

```

artifacts/reports/M46_emulator_control_panel_report.md

```

## Files Expected to Change

Likely:

- `emulator/frontend/frontend.h`
- `emulator/frontend/frontend.cpp`
- `emulator/frontend/debug_panels/debug_panel_host.cpp`
- `emulator/frontend/debug_panels/debug_panel_host.h`
- `emulator/src/main.cpp`
- `docs/state_snapshot.md`

## Files to Create

- `artifacts/reports/M46_emulator_control_panel_report.md`

## Constraints

Do not:

- modify emulator core subsystem code
- introduce new third-party libraries
- change CPU scheduling semantics
- implement ROM loader UI
- implement debug panels
- add complex UI abstractions

Keep this milestone intentionally small.

## Verification Commands

Run:

```

cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure

```

Runtime smoke verification:

```

./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 1

```

SDL runtime verification:

```

./build/super_z80 --sdl-input

```

or CI-safe equivalent using dummy SDL drivers.

## Acceptance Criteria

- Emulator builds successfully
- tests pass
- control panel window renders
- run/pause works
- reset works
- frame stepping works
- headless mode unaffected
- state snapshot updated
- completion report created

## Deliverables

1. Working Emulator Control Panel
2. Run/Pause/Reset/Step functionality
3. Updated state snapshot
4. Completion report

## Suggested Branch Name

```

m46_emulator_control_panel

```

## Suggested Commit Message

```

M46: add emulator control panel

```

## Completion Report Format

Use:

- Summary
- Files Changed
- Files Created
- Commands Run
- Verification Results
- Issues Encountered
- Recommended Next Step

## Recommended Next Step

M47 — ROM Loader Interface
