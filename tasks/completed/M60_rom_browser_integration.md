# Task Packet

## Title

M60 — ROM Browser Integration

## Goal

Implement a graphical **ROM Browser Integration** for the Super_Z80 frontend so developers can select ROM files from an in-application file browser instead of manually typing full paths.

This milestone must add a minimal, usable ROM browsing workflow to the SDL frontend while preserving the existing ROM loading path, deterministic execution, and headless behavior.

This is a frontend/runtime-shell milestone only.

Do not add native OS dialog dependencies unless already present in the repository. Prefer an in-app ImGui-based browser.

## Repository Context

The canonical frontend plan now defines:

- M59 — Display Scaling & Window Sizing
- M60 — ROM Browser Integration

M60 specifically requires:

- graphical ROM selection dialog
- in-application file browser for navigating directories
- ability to select ROM files without manually entering paths
- graceful handling of invalid or unsupported files

Verification in the plan requires:

- ROM browser opens from frontend UI
- directory navigation functions correctly
- selected ROM loads successfully
- invalid selections produce safe error handling
- headless execution remains unaffected

The previous milestone M59 is complete and added persisted display scaling and window resizing behavior in the frontend/runtime shell.

## Required Read Order

Before making changes, read in this order:

1. `docs/plan.md`
2. `docs/state_snapshot.md`
3. `docs/architecture.md`
4. `docs/decisions.md`
5. `AGENTS.md`
6. `artifacts/reports/M59_display_scaling_window_sizing_report.md`

Then inspect the current frontend/runtime files:

- `emulator/frontend/frontend.h`
- `emulator/frontend/frontend.cpp`
- `emulator/frontend/debug_panels/debug_panel_host.h`
- `emulator/frontend/debug_panels/debug_panel_host.cpp`
- `emulator/src/main.cpp`

Also inspect the current ROM loading/reload path added earlier, including any existing File menu path-entry modal or CLI/SDL ROM-load plumbing.

## Scope

Included in scope:

- in-application ROM browser UI
- directory navigation
- file selection for ROM loading
- integration with existing File menu / ROM load path
- graceful invalid-file handling
- optional filtering to likely ROM file types if this can be done narrowly
- documentation update
- milestone completion report

Excluded from scope:

- native platform file picker dependencies
- recent files list
- drag-and-drop loading
- thumbnail/preview system
- cartridge metadata browser
- recursive indexing
- favorites/bookmarks
- filesystem abstraction framework
- headless path changes

This milestone is specifically about replacing manual path entry with a usable in-app browser.

## Milestone Definition

M60 — ROM Browser Integration

Features:

- graphical ROM selection dialog
- in-application file browser for navigating directories
- ability to select ROM files without manually entering paths
- graceful handling of invalid or unsupported files

Verification:

- ROM browser opens from frontend UI
- directory navigation functions correctly
- selected ROM loads successfully
- invalid selections produce safe error handling
- headless execution remains unaffected

Purpose:

- improve ROM loading usability
- remove reliance on manual path entry

## Architectural Constraints

Maintain strict separation between:

- emulator core
- platform shell
- frontend UI

This work must remain in the frontend/runtime shell layer.

Do not move filesystem/UI concerns into deterministic emulator-core subsystems.

Do not modify hardware behavior, scheduler semantics, or headless execution.

Do not introduce large new abstractions or dependencies for file dialogs if an ImGui-based browser can satisfy the milestone.

## Implementation Requirements

### 1. Replace or extend current ROM load UI

Integrate the ROM browser into the existing `File` menu ROM-load flow.

Acceptable end state:

- `File -> Load ROM...` opens a browser modal/window
- user can navigate directories
- user can select a file
- selecting a valid ROM loads it through the existing runtime ROM-load path

If a manual path field still exists, it may remain as fallback, but the browser must become the primary UI path.

### 2. In-app file browser

Implement a narrow ImGui-based file browser.

Minimum required behavior:

- show current directory
- list directory entries
- allow entering subdirectories
- allow navigating to parent directory
- allow selecting a file
- show current selected file/path before confirm if helpful

Keep this lightweight. Do not build a general-purpose file manager.

### 3. File filtering

If easy to add cleanly, filter displayed files to likely ROM file extensions and/or allow all files with directories always visible.

This is optional. Do not overengineer extension handling.

### 4. Load integration

Selecting a file must reuse the existing shell-owned ROM load/reset path.

Do not duplicate ROM initialization logic.

Requirements:

- valid selection loads ROM
- current ROM path updates correctly
- reload continues to work after browser-based load
- invalid file selection fails clearly and safely

### 5. Persistence / start directory

If there is an obvious narrow place to remember the last browsed directory using the same persistence mechanism already used for frontend settings, do so.

If this would cause scope growth, default to:

- current ROM directory if a ROM is already loaded
- otherwise current working directory or another narrow existing default

Document the choice in the report.

### 6. Runtime safety

The browser must behave safely when:

- no ROM is loaded
- invalid directory is encountered
- permissions or unreadable entries occur
- the user cancels the dialog

Do not crash or leave stale state in a way that breaks later loads.

### 7. No timing impact

The browser and directory scanning must not affect emulator determinism or scheduler behavior beyond the ordinary paused/running UI flow already present.

Keep file browsing entirely frontend/runtime local.

## Files Expected to Change

Likely:

- `emulator/frontend/frontend.h`
- `emulator/frontend/frontend.cpp`
- `emulator/frontend/debug_panels/debug_panel_host.h`
- `emulator/frontend/debug_panels/debug_panel_host.cpp`
- `emulator/src/main.cpp`

Possibly small additions to existing frontend persistence/config handling if a last-directory value is persisted.

Avoid touching emulator core files unless absolutely necessary, which is not expected.

## Files to Create

Create milestone report:

- `artifacts/reports/M60_rom_browser_integration_report.md`

If a small dedicated frontend helper for the browser is clearly warranted, it may be created, but keep the implementation minimal.

## Constraints

Do not:

- add new third-party dependencies for native dialogs
- modify emulator core architecture
- change ROM load semantics beyond UI/browser integration
- add recent-files systems
- add drag-and-drop
- add speculative settings frameworks
- modify deterministic scheduler behavior
- modify headless output behavior

Keep the implementation lightweight and frontend-local.

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

If practical, also add one narrow frontend/runtime-safe verification of browser state logic or directory enumeration helper behavior.

Confirm:

* build succeeds
* tests pass
* headless execution remains unchanged
* SDL frontend launches without crash
* ROM browser path is integrated

Automated visual confirmation of the actual browser interaction flow is not expected in this environment; document that clearly in the report.

## Acceptance Criteria

* emulator builds successfully
* tests pass
* `Load ROM...` opens an in-app browser UI
* browser supports directory navigation
* valid ROM selection loads successfully
* invalid selections fail clearly and safely
* existing reload path still works
* headless mode unchanged
* `docs/state_snapshot.md` updated
* `artifacts/reports/M60_rom_browser_integration_report.md` created

## Suggested Branch

`m60_rom_browser_integration`

## Suggested Commit Message

`M60: add rom browser integration`

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

Plan review for the next frontend phase after M60.
