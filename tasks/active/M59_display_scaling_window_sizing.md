# Task Packet

## Title

M59 — Display Scaling & Window Sizing

## Goal

Implement integer display scaling controls for the Super_Z80 SDL frontend so the emulator window is usable on high-resolution displays, including 4K monitors.

This milestone must add selectable scale presets for the emulator display and ensure the window resizes consistently to the chosen scale without affecting emulator timing or headless execution.

This is a frontend/runtime-shell milestone only.

Do not implement ROM browser functionality in this task. That belongs to M60.

## Repository Context

The canonical frontend plan now defines:

- M59 — Display Scaling & Window Sizing
- M60 — ROM Browser Integration

M59 specifically requires:

- integer display scaling presets
- selectable scaling values (recommended: 2x, 4x, 6x)
- scaling suitable for high-resolution displays
- consistent window resizing behavior
- persistence of selected scale between runs

Verification in the plan requires:

- emulator window resizes correctly
- framebuffer scales without distortion
- scaling changes do not affect emulator timing
- headless execution remains unaffected

This milestone is therefore plan-authorized and aligned with repository truth. :contentReference[oaicite:1]{index=1}

## Required Read Order

Before making changes, read in this order:

1. `docs/plan.md`
2. `docs/state_snapshot.md`
3. `docs/architecture.md`
4. `docs/decisions.md`
5. `AGENTS.md`
6. `artifacts/reports/M58_frontend_stabilization_report.md`

Then inspect the current frontend/runtime files:

- `emulator/frontend/frontend.h`
- `emulator/frontend/frontend.cpp`
- `emulator/frontend/debug_panels/debug_panel_host.h`
- `emulator/frontend/debug_panels/debug_panel_host.cpp`
- `emulator/src/main.cpp`

Also inspect current SDL window/renderer initialization and any existing persisted frontend settings/layout code.

## Scope

Included in scope:

- integer display scale presets for SDL frontend runtime
- selectable scale values: 2x, 4x, 6x
- frontend UI control for changing scale during interactive use
- runtime shell behavior that resizes the window consistently
- persistence of selected scale between runs if a narrow existing persistence mechanism can be reused
- documentation update
- milestone completion report

Excluded from scope:

- arbitrary freeform scaling values
- non-integer scaling
- fullscreen overhaul
- DPI-awareness overhaul
- ROM browser integration
- renderer redesign
- new emulator hardware/debug panels
- headless path changes
- speculative settings framework

This milestone is specifically about window/display scale usability.

## Milestone Definition

M59 — Display Scaling & Window Sizing

Features:

- integer display scaling presets for emulator window
- selectable scaling values (recommended: 2x, 4x, 6x)
- scaling appropriate for high-resolution displays (4K monitors)
- window resizing behavior consistent with selected scale
- frontend persistence of selected scale between runs

Verification:

- emulator window correctly resizes to selected scale
- framebuffer content scales correctly without distortion
- scaling changes do not affect emulator timing
- headless execution remains unaffected

Purpose:

- improve usability on high-resolution displays
- allow developers to comfortably view emulator output :contentReference[oaicite:2]{index=2}

## Architectural Constraints

Maintain strict separation between:

- emulator core
- platform shell
- frontend UI

This work must remain in the frontend/runtime shell layer. The emulator core is still intended to remain bus-centric, modular, and independent of platform/UI concerns, while SDL-specific behavior stays in the shell/frontend boundary. :contentReference[oaicite:3]{index=3}

Do not move UI or SDL sizing logic into deterministic emulator-core subsystems.

Do not modify hardware behavior, scheduler semantics, or headless execution.

AWOS requires small, bounded, deterministic tasks with explicit verification and no silent expansion of scope. This task must stay narrow and mergeable. 

## Implementation Requirements

### 1. Scale presets

Implement integer display scale presets:

- 2x
- 4x
- 6x

Do not add 8x in this milestone.

These presets should scale the emulator display window based on the native framebuffer dimensions already used by the SDL frontend.

### 2. Window sizing behavior

Changing scale must resize the SDL window consistently.

Requirements:

- selected scale changes window size immediately during SDL runtime if practical
- initial window size on startup uses the persisted or default scale
- scaling must not distort the framebuffer
- scaling must preserve aspect ratio

Use the existing SDL renderer/window flow. Do not redesign the rendering pipeline.

### 3. Frontend UI integration

Add a minimal UI control for scale selection.

Acceptable placements include:

- a `View` menu
- a `Display` menu
- a `Debug`/frontend settings area if that is the narrowest fit

Use a compact control such as:

- menu items
- radio items
- combo/select box

The currently selected scale should be visible to the user.

### 4. Persistence

Persist the selected scale between runs if this can be done by reusing the current frontend persistence approach or another narrow existing mechanism.

If the existing persistence path is ImGui ini or SDL preference-backed config and it can cleanly store one integer scale value, use that.

Do not build a large custom settings system for this milestone.

If persistence cannot be added narrowly without scope growth, implement stable defaults and clearly document the limitation in the report.

### 5. Runtime safety

Scale changes must behave safely when:

- no ROM is loaded
- panels are open
- layout persistence exists

Do not let scale changes break panel rendering or crash SDL.

### 6. No timing impact

Changing display scale must not affect:

- emulator frame stepping semantics
- headless determinism
- scheduler timing
- audio generation behavior

This is display-shell behavior only.

## Files Expected to Change

Likely:

- `emulator/frontend/frontend.h`
- `emulator/frontend/frontend.cpp`
- `emulator/frontend/debug_panels/debug_panel_host.h`
- `emulator/frontend/debug_panels/debug_panel_host.cpp`
- `emulator/src/main.cpp`

Possibly small changes to existing frontend persistence/config handling if present.

Avoid touching emulator core files unless absolutely necessary, which is not expected for this milestone.

## Files to Create

Create milestone report:

- `artifacts/reports/M59_display_scaling_window_sizing_report.md`

## Constraints

Do not:

- add new third-party dependencies
- modify emulator core architecture
- change ROM loader behavior beyond incidental menu placement integration
- introduce non-integer scaling
- add a large settings framework
- modify deterministic scheduler behavior
- modify headless output behavior

Keep the implementation lightweight and SDL/frontend-local.

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

If practical, also run at least one additional SDL startup check that exercises a non-default persisted or configured scale value.

Confirm:

* build succeeds
* tests pass
* headless execution remains unchanged
* SDL frontend launches without crash
* scale selection path is integrated

Automated visual confirmation of resized window dimensions and menu-driven scaling behavior is not expected in this environment; document that clearly in the report.

## Acceptance Criteria

* emulator builds successfully
* tests pass
* display scale presets 2x, 4x, 6x exist
* selected scale changes SDL window sizing correctly
* framebuffer scaling remains undistorted
* selected scale is persisted between runs, or limitation is explicitly documented if narrow persistence is not feasible
* headless mode unchanged
* `docs/state_snapshot.md` updated
* `artifacts/reports/M59_display_scaling_window_sizing_report.md` created

## Suggested Branch

`m59_display_scaling_window_sizing`

## Suggested Commit Message

`M59: add display scaling and window sizing`

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

M60 — ROM Browser Integration
