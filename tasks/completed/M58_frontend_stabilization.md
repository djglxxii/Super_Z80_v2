# Task Packet

## Title

M58 — Frontend Stabilization

## Goal

Perform a stabilization pass on the Super_Z80 frontend to improve robustness, correctness, and maintainability after the large series of debug UI milestones.

This milestone focuses on:

- improving frontend reliability
- addressing minor UI issues
- ensuring runtime safety and graceful behavior
- validating integration with emulator core systems
- cleaning up small technical debt introduced during the panel expansion milestones

No new emulator hardware features or major UI systems should be introduced in this milestone.

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
- M57 — Integrated Debug Overlay

The frontend now contains a large number of debug panels and UI systems.

The purpose of **M58 — Frontend Stabilization** is to ensure the UI system is solid before moving forward with further development milestones.

## Required Read Order

Before making changes read:

1. `docs/plan.md`
2. `docs/state_snapshot.md`
3. `docs/architecture.md`
4. `docs/decisions.md`
5. `AGENTS.md`
6. `artifacts/reports/M57_integrated_debug_overlay_report.md`

Then inspect:

Frontend code:

- `emulator/frontend/frontend.h`
- `emulator/frontend/frontend.cpp`
- `emulator/frontend/debug_panels/debug_panel_host.h`
- `emulator/frontend/debug_panels/debug_panel_host.cpp`

Runtime integration:

- `emulator/src/main.cpp`

Also inspect ImGui initialization and configuration used by the frontend.

## Scope

Included:

- frontend robustness improvements
- small UI bug fixes
- safe handling of null or unavailable emulator state
- validation of panel visibility state logic
- validation of snapshot tool integration
- verification that panel rendering does not crash during ROM reloads or emulator resets
- small code cleanup where appropriate
- documentation update
- milestone report

Excluded:

- new debug panels
- emulator core refactors
- hardware emulation changes
- major UI redesign
- new dependencies
- new UI frameworks
- docking system integration

This milestone should remain focused on stabilization and cleanup.

## Stabilization Areas

Review and address potential issues in the following areas:

### 1. Panel lifecycle safety

Ensure that:

- panels behave safely when emulator state is unavailable
- panels behave correctly before a ROM is loaded
- panels do not dereference null runtime pointers
- panels do not assume subsystem presence without checks

### 2. Snapshot tool integration

Verify that:

- snapshot controls behave correctly
- restore without snapshot produces safe user feedback
- snapshot restore does not break panel state
- snapshot restore does not corrupt frontend UI state

### 3. Panel visibility system

Verify that:

- panel visibility toggles behave correctly
- toggling panels repeatedly does not create duplicates
- menu state reflects actual panel visibility
- panels reopen correctly after being closed

### 4. Layout persistence

Verify that:

- ImGui ini persistence path works correctly
- layout persistence does not crash when the file is missing
- layout persistence does not break when new panels are added
- the frontend behaves safely if the ini file is corrupted

Do not build a custom persistence system.

### 5. Window placement sanity

Ensure first-use window placement is sensible and does not cause panels to render off-screen.

### 6. Error handling

Improve safe handling for:

- invalid ROM load attempts
- missing snapshot restore
- unexpected subsystem unavailability

Only minimal user feedback is required.

### 7. Minor code cleanup

Clean up:

- redundant panel initialization logic
- minor inconsistencies
- obvious code duplication
- small readability improvements

Avoid broad refactoring.

## Implementation Requirements

### 1. Maintain frontend/core separation

All stabilization work must remain in the frontend or shell.

Do not introduce UI dependencies into emulator core subsystems.

### 2. Preserve deterministic execution

Frontend changes must not alter emulator timing or behavior.

### 3. Avoid architecture changes

Do not introduce new frameworks or large abstractions.

### 4. Maintain headless compatibility

The headless execution path must remain unchanged.

## Files Expected to Change

Likely:

- `emulator/frontend/frontend.h`
- `emulator/frontend/frontend.cpp`
- `emulator/frontend/debug_panels/debug_panel_host.h`
- `emulator/frontend/debug_panels/debug_panel_host.cpp`
- `emulator/src/main.cpp`

Possibly small adjustments in panel files if required.

Avoid touching emulator core unless absolutely necessary.

## Files to Create

Create milestone report:

- `artifacts/reports/M58_frontend_stabilization_report.md`

## Constraints

Do not:

- add new emulator features
- add new debug panels
- change emulator core architecture
- introduce dependencies
- modify deterministic scheduler behavior
- change ROM loading behavior beyond minor safety checks

Keep this milestone focused and safe.

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
* panel visibility works correctly
* snapshot tools remain functional
* layout persistence behaves safely

Automated visual confirmation of UI stability is not expected; document this clearly in the report.

## Acceptance Criteria

* emulator builds successfully
* tests pass
* frontend launches without crash
* panel toggles remain functional
* snapshot tools remain functional
* UI behaves safely when no ROM is loaded
* layout persistence behaves safely
* headless mode unchanged
* `docs/state_snapshot.md` updated
* `artifacts/reports/M58_frontend_stabilization_report.md` created

## Suggested Branch

`m58_frontend_stabilization`

## Suggested Commit Message

`M58: frontend stabilization pass`

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

M59 — Frontend Performance Diagnostics
