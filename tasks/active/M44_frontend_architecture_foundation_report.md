# Task Packet

## Title

M44 — Frontend Architecture Foundation

## Goal

Implement the structural foundation for the Super_Z80 emulator frontend in alignment with the new plan. This milestone is limited to establishing the frontend module boundary, frontend lifecycle scaffolding, and shell integration points. Do not implement ImGui, ROM loading, or full rendering/debug panels yet. The goal is to create the architectural base that later frontend milestones will build on.

## Repository Context

This project follows AWOS. Work must be executed as a bounded, deterministic task packet with explicit verification and documented results. The repository is the source of truth, task packets must be self-contained, and success must be demonstrated with observable verification output rather than subjective claims. Read the relevant repository documents before editing, especially current state snapshot, architecture docs, plan, and instruction files. The frontend plan explicitly defines M44 as: create `emulator/frontend/`, define a frontend initialization interface, integrate frontend lifecycle with the SDL shell, and prepare the debugging panel system. Verification for this milestone is that the emulator builds successfully, frontend initialization runs, and headless mode is unaffected. :contentReference[oaicite:0]{index=0} :contentReference[oaicite:1]{index=1}

## Required Read Order

Before making changes, read in this order:

1. `docs/state_snapshot.md`
2. `docs/architecture.md`
3. `docs/plan.md`
4. `AGENTS.md`
5. any current frontend/shell entry files under `emulator/`

This ordering follows AWOS cold-start expectations and is mandatory for this task. :contentReference[oaicite:2]{index=2}

## Scope

Included in scope:

- create the initial frontend module structure under `emulator/frontend/`
- add a minimal frontend interface/class that can be initialized from the runtime shell
- add lifecycle hooks for frontend startup/shutdown and a no-op per-frame/per-loop update path if needed
- prepare a place for future debug panels, such as `emulator/frontend/debug_panels/`
- integrate the frontend scaffold into the existing build system
- integrate the frontend scaffold into the current emulator shell/runtime entry path in a way that does not change current hardware behavior
- update repository truth docs as needed:
  - `docs/state_snapshot.md`
- produce a completion report in `artifacts/reports/`

Excluded from scope:

- Dear ImGui integration
- ROM loading UI
- actual debug panels
- renderer redesign
- emulator control panel
- changes to emulator core hardware behavior
- architectural rewrites of unrelated subsystems
- speculative abstractions beyond what M44 needs

## Architectural Constraints

Maintain strict separation between:

- emulator core
- platform shell
- frontend UI/debug layer

The core must remain deterministic, UI-independent, and usable in headless mode. The shell owns SDL/window/input/audio event loop responsibilities. The frontend owns developer-facing UI/debug structures and must not modify core subsystem logic. The current plan explicitly defines these boundaries and M44 exists to create the structural foundation only. :contentReference[oaicite:3]{index=3}

Also preserve the broader emulator design principle that core logic remains separate from platform-dependent code, with SDL/OS-specific code isolated from the emulator core. 

## Implementation Requirements

1. Create frontend structure

Create a minimal structure similar to:

- `emulator/frontend/frontend.h`
- `emulator/frontend/frontend.cpp`
- `emulator/frontend/debug_panels/` (directory placeholder, with minimal stub file only if needed for build system clarity)

Do not overbuild this. Keep it narrow and milestone-sized.

2. Define frontend interface

Add a small frontend interface/class with only the responsibilities needed for this milestone. Suitable examples include:

- initialize
- shutdown
- begin_frame / end_frame or equivalent no-op lifecycle hooks
- a way for the shell to create/use the frontend without coupling the core to frontend code

Keep the interface intentionally small.

3. Integrate with existing shell/runtime

Wire the frontend scaffold into the existing runtime/shell path so that:

- it can be constructed/initialized in SDL-capable mode
- it does not require ImGui yet
- it does not break existing current CLI behavior
- it does not break headless usage

4. Build system updates

Update CMake files only as needed to include the new frontend module cleanly.

5. Documentation and report

Update:

- `docs/state_snapshot.md`

Create:

- `artifacts/reports/M44_frontend_architecture_foundation_report.md`

The report must include summary, files changed, commands run, verification results, issues encountered, and recommended next step, consistent with AWOS completion-report expectations. 

## Files Expected to Change

Expected, not mandatory exact list:

- `emulator/CMakeLists.txt`
- `emulator/src/main.cpp` or current shell/runtime entry file
- `docs/state_snapshot.md`

## Files to Create

Likely:

- `emulator/frontend/frontend.h`
- `emulator/frontend/frontend.cpp`
- `artifacts/reports/M44_frontend_architecture_foundation_report.md`

Optional if useful and minimal:

- `emulator/frontend/debug_panels/.gitkeep`
- or a very small stub header/source for future panel registration

## Constraints

- Do not add new third-party dependencies.
- Do not add Dear ImGui yet.
- Do not refactor unrelated emulator subsystems.
- Do not modify hardware behavior.
- Do not move SDL timing or shell responsibilities into the emulator core.
- Do not change existing deterministic validation behavior.
- Do not change the milestone plan except to reflect progress in repository truth docs if needed.
- Prefer the smallest mergeable slice that satisfies M44.
- If you encounter a conflict between this task and repository truth, stop and report it instead of inventing behavior. This is required by AWOS. 

## Verification Commands

Run exactly these unless a repository-specific equivalent already exists and is clearly preferable:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
````

If a current runnable emulator binary exists after build, also perform a basic runtime smoke verification appropriate to the current repo state and report exactly what you ran.

The frontend plan defines these build/test commands as the milestone verification baseline, and AWOS requires explicit verification commands in every task packet.

## Acceptance Criteria

* [ ] `emulator/frontend/` exists with a minimal frontend architecture scaffold.
* [ ] A frontend initialization interface exists.
* [ ] The runtime shell is aware of the frontend scaffold.
* [ ] The project builds successfully.
* [ ] Tests still pass.
* [ ] Headless-oriented functionality is not broken.
* [ ] `docs/state_snapshot.md` is updated.
* [ ] `artifacts/reports/M44_frontend_architecture_foundation_report.md` is created.

## Deliverables

1. Minimal frontend architecture scaffold
2. Build-system integration
3. Runtime/shell integration hook
4. Updated state snapshot
5. Completion report

## Suggested Branch Name

`m44_frontend_architecture_foundation`

## Suggested Commit Message

`M44: add frontend architecture foundation`

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

M45 — ImGui Integration

The plan defines M45 as the next milestone after M44, focused on initializing Dear ImGui with the SDL backend and rendering a minimal diagnostic window. 

```
