# Task Packet

## Title

M47 — ROM Loader Interface

## Goal

Implement the first ROM-loading UI flow for the Super_Z80 emulator frontend.

This milestone adds a minimal **File menu / ROM Loader Interface** that allows a developer to choose a ROM path, load that ROM into the emulator runtime, and restart execution from reset using the newly loaded ROM.

The purpose of this milestone is to remove dependence on CLI-only ROM startup during interactive use and establish the basic interactive ROM workflow for the frontend.

This milestone is limited to ROM loading and reload behavior only.

Do not implement advanced file browsing widgets, recent files, cartridge metadata, or subsystem inspection panels.

## Repository Context

M44 established the frontend architecture foundation.

M45 integrated Dear ImGui into the SDL runtime and added a minimal frontend window.

M46 added the Emulator Control Panel with run/pause, reset, and step-frame controls using frontend-owned commands consumed by shell-owned runtime state.

This milestone extends that runtime UI by allowing the user to load and reload ROMs interactively.

The deterministic emulator core and headless execution paths must remain unchanged.

## Required Read Order

Before editing, read in this order:

1. `docs/plan.md`
2. `docs/state_snapshot.md`
3. `docs/architecture.md`
4. `docs/decisions.md`
5. `AGENTS.md`
6. `artifacts/reports/M46_emulator_control_panel_report.md`
7. current frontend files:
   - `emulator/frontend/frontend.h`
   - `emulator/frontend/frontend.cpp`
   - `emulator/frontend/debug_panels/debug_panel_host.h`
   - `emulator/frontend/debug_panels/debug_panel_host.cpp`
8. current runtime/shell entry path:
   - `emulator/src/main.cpp`
9. any existing ROM-loading / ROM-path / initialization logic already present in the runtime

## Scope

Included in scope:

- add a minimal frontend **File** menu or equivalent ROM-loading UI entry point
- add a way to specify a ROM path during interactive runtime
- load the selected ROM into the emulator runtime
- reset/reinitialize emulator state after ROM load
- support ROM reload of the current interactive ROM
- handle invalid path / failed load errors in a clear UI-visible way
- preserve existing CLI ROM loading behavior
- preserve headless behavior
- update state snapshot documentation
- create milestone completion report

Excluded from scope:

- native OS file picker integration if it requires new dependencies
- recent files list
- cartridge/header viewer
- ROM metadata panel
- drag-and-drop loading
- save states
- subsystem inspection panels
- major shell/runtime redesign

## Milestone Definition

M47 implements ROM loading through the frontend.

Features:

- File → Load ROM menu
- ROM reload capability
- error handling for invalid ROMs

Verification:

- ROM loads successfully
- emulator resets correctly
- execution begins from reset vector

Purpose:

Enable interactive ROM testing without CLI usage.

## Architectural Constraints

Maintain strict separation between:

- emulator core
- platform shell
- frontend UI/debug layer

The emulator core must remain deterministic and UI-independent.

ROM loading should be orchestrated through shell/runtime-owned state and existing initialization pathways, not by introducing UI logic into core hardware modules.

Do not move cartridge/ROM-loading policy into the frontend beyond command/request generation and status display.

Reuse existing ROM initialization/loading code paths where possible.

## Implementation Requirements

### 1. File menu / ROM loading UI

Add a minimal top-level menu or equivalent UI entry for ROM actions.

Preferred structure:

- `File`
  - `Load ROM...`
  - `Reload ROM`

If a full text-entry popup is the narrowest practical solution for this milestone, that is acceptable.

Examples of acceptable narrow implementations:

- a popup/modal with a text field for ROM path and Load/Cancel buttons
- a simple input field + button in a File menu or dedicated modal
- reusing an existing current ROM path known to the runtime for Reload

Do not add a dependency-heavy native file dialog library.

### 2. ROM path handling

Support entering/selecting a ROM path during SDL/windowed runtime.

Requirements:

- loading a valid ROM path updates the active runtime ROM
- reloading uses the currently active interactive ROM path
- if no ROM is loaded yet, Reload should fail gracefully
- frontend should display success/failure status in a simple way

### 3. Runtime reload / reset behavior

Loading a ROM must:

- reinitialize emulator state cleanly
- load the selected ROM through the normal runtime path
- start execution from reset vector
- preserve shell/frontend ownership boundaries

Avoid duplicate initialization logic. Prefer reusing the same startup/reset path already used by CLI boot or M46 reset behavior.

### 4. Error handling

If ROM load fails, provide clear user-visible feedback.

Examples:

- file not found
- unreadable file
- invalid size / failed load path if such checks already exist

Do not silently fail.

### 5. Frontend integration

Extend the existing frontend panel/menu structure rather than bypassing it.

The frontend should issue commands such as:

- load requested ROM path
- reload current ROM

The shell/runtime should consume those commands and perform the actual reinitialization/load.

### 6. Documentation and report

Update:

- `docs/state_snapshot.md`

Create:

- `artifacts/reports/M47_rom_loader_interface_report.md`

## Files Expected to Change

Likely:

- `emulator/frontend/frontend.h`
- `emulator/frontend/frontend.cpp`
- `emulator/frontend/debug_panels/debug_panel_host.h`
- `emulator/frontend/debug_panels/debug_panel_host.cpp`
- `emulator/src/main.cpp`
- `docs/state_snapshot.md`

Possibly additional runtime/shell support files if the current ROM loading path is factored elsewhere.

## Files to Create

- `artifacts/reports/M47_rom_loader_interface_report.md`

Possibly small frontend helper files only if truly needed. Keep this minimal.

## Constraints

Do not:

- add new third-party file dialog dependencies
- refactor unrelated emulator subsystems
- modify emulator core hardware behavior
- alter scheduler semantics
- implement recent-files systems
- implement cartridge metadata systems
- implement drag-and-drop loading
- implement subsystem inspection panels

Keep this milestone intentionally small and mergeable.

## Verification Commands

Run:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
````

Also run runtime smoke verification.

At minimum verify:

1. existing headless ROM execution still works
2. SDL frontend starts successfully
3. ROM load/reload path works in a runtime-verifiable way

If CI-safe automation requires dummy SDL drivers, use them and report exact commands.

If the interactive load path cannot be fully exercised in an automated way, make the best practical verification attempt and clearly state what was and was not exercised.

## Acceptance Criteria

* [ ] Emulator builds successfully
* [ ] Tests pass
* [ ] Frontend exposes a `File` ROM-loading path or equivalent minimal ROM-loading UI
* [ ] A valid ROM can be loaded interactively
* [ ] Reload current ROM works
* [ ] Invalid ROM path failure is surfaced clearly
* [ ] Emulator resets/restarts correctly after ROM load
* [ ] Headless mode remains unaffected
* [ ] `docs/state_snapshot.md` is updated
* [ ] `artifacts/reports/M47_rom_loader_interface_report.md` is created

## Deliverables

1. Minimal ROM Loader Interface
2. Interactive ROM load capability
3. ROM reload capability
4. Updated state snapshot
5. Completion report

## Suggested Branch Name

`m47_rom_loader_interface`

## Suggested Commit Message

`M47: add rom loader interface`

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

M48 — CPU Debug Panel

```
