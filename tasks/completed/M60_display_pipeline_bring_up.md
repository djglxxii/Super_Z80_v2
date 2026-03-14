# Task Packet

## Title

M60 — Display Pipeline Bring-Up

## Goal

Fix the missing runtime display pipeline so the Super_Z80 SDL frontend shows actual emulator video output rather than only a dark background with ImGui overlays.

This milestone must implement the missing connection between:

- VDP frame rasterization
- frontend SDL framebuffer presentation

The emulator already loads ROMs, executes CPU code, updates VRAM/palette/sprites, and produces correct headless outputs. The bug is that the runtime path never renders a completed VDP frame and never presents that framebuffer through SDL.

This milestone must restore visible game rendering in SDL mode while preserving deterministic execution and headless behavior.

## Repository Context

The current investigation identified two concrete missing pieces in the display pipeline:

1. `VDP::render_frame()` is never invoked in the normal emulation runtime path.
2. The frontend does not create/update/draw an SDL texture from the VDP framebuffer.

The VDP already has a working frame render path and framebuffer access API, and those paths are exercised in tests. The runtime/frontend path is what is missing.

This milestone is a bugfix milestone and should be implemented only after the canonical plan has been updated to include it.

## Required Read Order

Before making changes, read in this order:

1. `docs/plan.md`
2. `docs/state_snapshot.md`
3. `docs/architecture.md`
4. `docs/decisions.md`
5. `AGENTS.md`
6. the display pipeline investigation summary from the task context
7. current runtime/display files:
   - `emulator/include/vdp.h`
   - `emulator/src/vdp.cpp`
   - `emulator/include/emulator_core.h`
   - `emulator/src/emulator_core.cpp`
   - `emulator/include/scheduler.h`
   - `emulator/src/scheduler.cpp`
   - `emulator/frontend/frontend.h`
   - `emulator/frontend/frontend.cpp`
   - `emulator/src/main.cpp`

## Scope

Included in scope:

- invoke VDP frame rendering in the runtime emulation path at the frame boundary
- add a frontend/runtime SDL texture presentation path for the emulator framebuffer
- render emulator video behind ImGui overlays
- preserve existing frontend scale presets
- preserve headless mode and deterministic timing
- add/update narrow read-only framebuffer access plumbing if needed
- update state snapshot documentation
- create milestone completion report

Excluded from scope:

- ROM browser integration
- VDP architecture redesign
- scanline-accurate incremental renderer refactor
- new debug panels
- shader/post-processing work
- fullscreen redesign
- arbitrary resize policy changes beyond using existing scale behavior
- speculative graphics abstraction work

## Milestone Definition

This milestone exists to repair the missing runtime display path.

Features:

- invoke VDP frame rendering in the runtime emulation path at the frame boundary
- create frontend framebuffer presentation path using SDL texture upload/rendering
- display rendered emulator framebuffer behind ImGui debug overlays
- preserve existing frontend scale presets when presenting the framebuffer

Verification:

- ROM loads and produces visible framebuffer output in SDL runtime
- ImGui overlays still render on top of emulator output
- headless execution remains unaffected
- display path does not alter emulator timing ownership

Purpose:

- restore visible game rendering in the frontend
- complete the missing runtime display pipeline between VDP framebuffer generation and SDL presentation

## Architectural Constraints

Maintain strict separation between:

- emulator core
- platform shell
- frontend UI

The scheduler owns emulation timing.
The VDP owns framebuffer rasterization.
The frontend/runtime shell owns SDL texture creation and presentation.

Do not move display ownership into the emulator core.
Do not move timing ownership into the frontend.

Keep the fix narrow and architectural boundaries clean.

## Implementation Requirements

### 1. Call `VDP::render_frame()` at frame completion

Add the runtime invocation of frame rendering in the emulation path.

Preferred approach:

- at the frame boundary in `Scheduler::step_scanline()`, invoke `vdp_.render_frame()` before wrapping/resetting frame/scanline state

Do not redesign the VDP into scanline rendering in this milestone.

### 2. Add framebuffer presentation path

In the SDL frontend/runtime path:

- create an `SDL_Texture` sized to the emulator framebuffer dimensions
- texture format should be appropriate for RGBA presentation
- update the texture each frame from the VDP framebuffer
- render the texture before ImGui
- preserve aspect ratio and current integer display scale behavior

### 3. Framebuffer access

Use a narrow, read-only access path from the runtime to the framebuffer data.

Acceptable approaches:

- `EmulatorCore` snapshot/copy API
- framebuffer copy helper
- existing VDP color accessors used through `EmulatorCore`

Do not let frontend code depend directly on mutable VDP internals.

### 4. Render order

Runtime render order must be:

1. clear renderer
2. draw emulator framebuffer texture
3. render ImGui debug overlay
4. present SDL frame

### 5. Scaling compatibility

Preserve the existing M59 integer display scale behavior:

- 2x
- 4x
- 6x

The framebuffer output must use the selected scale without distortion.

### 6. Timing safety

The display pipeline must not alter:

- headless determinism
- scheduler timing ownership
- audio generation semantics
- CPU execution semantics

The frontend is only presenting the most recently rendered frame.

### 7. Color correctness

Be careful about color packing between VDP RGBA values and SDL texture pixel format.

If direct 32-bit writes are used, verify channel ordering is correct.
If needed, explicitly pack bytes into the SDL texture buffer in the correct order.

## Files Expected to Change

Likely:

- `emulator/src/scheduler.cpp`
- `emulator/include/emulator_core.h`
- `emulator/src/emulator_core.cpp`
- `emulator/frontend/frontend.h`
- `emulator/frontend/frontend.cpp`
- `emulator/src/main.cpp`

Possibly:

- small read-only framebuffer helper additions in VDP-facing APIs if needed

Avoid broader subsystem changes.

## Files to Create

Create milestone report:

- `artifacts/reports/M60_display_pipeline_bring_up_report.md`

If a tiny frontend helper/source file is truly warranted for SDL texture presentation, it may be created, but keep this minimal.

## Constraints

Do not:

- refactor the VDP into a new rendering model
- redesign the renderer
- add third-party dependencies
- change emulator hardware behavior
- change ROM browser functionality
- add fullscreen or post-processing features
- modify deterministic scheduler behavior

Keep this as a narrow bugfix slice.

## Verification Commands

Run:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
````

Headless verification:

```bash
./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 1
```

SDL runtime verification:

```bash
env SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software timeout 2s ./build/super_z80 --sdl-input --rom rom/showcase/build/showcase.bin
```

```bash
env SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software SDL_AUDIODRIVER=dummy timeout 2s ./build/super_z80 --sdl-audio --rom rom/showcase/build/showcase.bin
```

If practical, add or update a narrow automated test for framebuffer snapshot/copy behavior or scheduler frame-render invocation.

Confirm:

* build succeeds
* tests pass
* headless behavior unchanged
* SDL frontend launches without crash
* display path is integrated

Automated visual confirmation of actual on-screen rendered game output is not expected in this environment; document that clearly in the report.

## Acceptance Criteria

* emulator builds successfully
* tests pass
* `VDP::render_frame()` is invoked in runtime emulation flow
* SDL frontend draws emulator framebuffer output
* ImGui overlays still render on top
* existing display scale presets still work
* headless mode unchanged
* `docs/state_snapshot.md` updated
* `artifacts/reports/M60_display_pipeline_bring_up_report.md` created

## Suggested Branch

`m60_display_pipeline_bring_up`

## Suggested Commit Message

`M60: add runtime display pipeline bring-up`

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

M61 — ROM Browser Integration

```
