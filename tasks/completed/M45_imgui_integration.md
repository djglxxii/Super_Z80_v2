# Task Packet

## Title

M45 — ImGui Integration

## Goal

Integrate Dear ImGui into the Super_Z80 frontend/runtime shell so the emulator can render a minimal developer UI window on top of the existing SDL-based shell. This milestone is limited to UI framework bring-up only. Do not implement ROM loading, control panels, or subsystem-specific debug views yet. The goal is to establish a clean, working ImGui render path that later milestones can build on.

## Repository Context

M44 is complete. The repository now has a frontend scaffold under `emulator/frontend/` with a small lifecycle interface and a stub debug panel host, integrated into SDL shell entry paths while preserving deterministic core behavior and headless execution. This task extends that foundation and should not redesign it.

This project follows AWOS. Work must remain narrowly scoped, deterministic, explicitly verified, and documented through a completion report.

## Required Read Order

Before editing, read in this order:

1. `docs/plan.md`
2. `docs/state_snapshot.md`
3. `docs/architecture.md`
4. `AGENTS.md`
5. `artifacts/reports/M44_frontend_architecture_foundation_report.md`
6. current frontend/shell/runtime files under:
   - `emulator/frontend/`
   - `emulator/src/main.cpp`
   - `emulator/CMakeLists.txt`

## Scope

Included in scope:

- integrate Dear ImGui into the SDL-capable emulator shell
- update build system to compile/link ImGui support
- initialize and shut down ImGui cleanly
- add a minimal frontend UI render path
- render at least one minimal diagnostic/test window
- preserve existing emulator execution behavior
- preserve headless behavior
- update repository truth docs as needed
- create milestone completion report

Excluded from scope:

- ROM loading UI
- emulator control panel
- CPU/memory/VRAM/audio inspection panels
- docking/layout persistence
- renderer redesign beyond what is necessary for ImGui bring-up
- changes to emulator hardware behavior
- speculative abstractions beyond what M45 requires

## Milestone Definition

The plan defines M45 as:

- initialize ImGui with SDL backend
- create UI render loop
- render a minimal diagnostic window

Verification:

- emulator window displays ImGui panel
- emulator execution continues normally

Purpose:

- establish graphical UI capability

## Architectural Constraints

Maintain strict separation between:

- emulator core
- platform shell
- frontend UI/debug layer

The emulator core must remain deterministic, UI-independent, and usable in headless mode.

The shell remains responsible for SDL/window/event-loop responsibilities.

The frontend owns UI/debug integration and must not push UI concerns into emulator core classes.

Do not add SDL or ImGui includes to core hardware subsystem headers/classes unless absolutely unavoidable, and avoid that if at all possible.

## Dependency Constraint

Do not introduce arbitrary new tooling or large frameworks.

For this milestone, Dear ImGui is the intended dependency. Keep integration minimal and local to the frontend/shell layer.

Prefer vendoring or integrating ImGui in the repository in the simplest maintainable way consistent with current project conventions. Do not add extra GUI frameworks.

If repository conventions clearly indicate a preferred vendoring location, follow that.

## Implementation Requirements

1. ImGui integration

Add Dear ImGui support to the SDL runtime path.

This includes:

- creating ImGui context
- wiring SDL event forwarding into ImGui
- beginning a frame
- rendering a frame
- shutting down cleanly

2. Minimal rendering path

Render a minimal diagnostic/test window. It should be simple, such as:

- window title like `Super_Z80 Frontend`
- small text showing frontend initialized
- optional placeholder text for future debug panels

Do not build real debug panels yet.

3. Shell integration

Integrate ImGui only where SDL/windowed runtime exists.

Requirements:

- existing SDL startup still works
- existing headless usage remains unaffected
- no change to deterministic core stepping semantics
- no wall-clock ownership moved into ImGui

4. Frontend structure

Extend the frontend module created in M44 rather than bypassing it.

If appropriate, add only the minimal methods needed, such as:

- `initialize_ui(...)`
- `begin_ui_frame(...)`
- `render_ui(...)`
- `shutdown_ui(...)`

Exact naming is up to you, but keep it narrow.

5. Build system

Update CMake only as needed.

Ensure build remains clean and that ImGui code is isolated to the frontend/shell side.

6. Documentation and report

Update:

- `docs/state_snapshot.md`

Create:

- `artifacts/reports/M45_imgui_integration_report.md`

## Files Expected to Change

Expected, not mandatory exact list:

- `emulator/CMakeLists.txt`
- `emulator/src/main.cpp`
- `emulator/frontend/frontend.h`
- `emulator/frontend/frontend.cpp`
- `docs/state_snapshot.md`

Possible additional files depending on integration strategy:

- vendored ImGui source files under an appropriate third-party/vendor location
- frontend-specific renderer/backend glue files if needed

## Files to Create

At minimum:

- `artifacts/reports/M45_imgui_integration_report.md`

Possibly additional frontend UI glue files if needed, but keep this minimal.

## Constraints

- Do not implement ROM loader UI yet.
- Do not implement control panels yet.
- Do not add subsystem inspection panels yet.
- Do not refactor unrelated emulator code.
- Do not modify hardware behavior.
- Do not break headless execution.
- Do not add non-ImGui GUI dependencies.
- Keep the diff as small as practical.
- If the repository already contains partial ImGui-related code or vendor content, extend it instead of replacing it wholesale.
- If integration requires a non-trivial design choice not covered by repository truth, choose the narrowest option and document it in the report.

## Verification Commands

Run exactly these unless a repository-specific equivalent is clearly preferable and already established:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
````

Also run runtime smoke verification appropriate to this milestone. At minimum, verify:

1. headless path still works
2. SDL/windowed path starts successfully
3. ImGui initialization/render path executes successfully

If using CI-safe dummy SDL drivers is appropriate for smoke validation, use them and report exact commands.

## Acceptance Criteria

* [ ] Dear ImGui is integrated into the SDL-capable frontend path.
* [ ] Emulator builds successfully.
* [ ] Tests still pass.
* [ ] A minimal ImGui window renders.
* [ ] Existing emulator execution continues normally.
* [ ] Headless mode remains unaffected.
* [ ] `docs/state_snapshot.md` is updated.
* [ ] `artifacts/reports/M45_imgui_integration_report.md` is created.

## Deliverables

1. Working ImGui integration in the frontend/runtime shell
2. Minimal test/diagnostic ImGui window
3. Updated build-system integration
4. Updated state snapshot
5. Completion report

## Suggested Branch Name

`m45_imgui_integration`

## Suggested Commit Message

`M45: integrate imgui into frontend shell`

## Completion Report Format

Use this structure:

* Summary
* Files Changed
* Files Created
* Commands Run
* Verification Results
* Issues Encountered
* Recommended Next Step

## Recommended Next Step

M46 — Emulator Control Panel
