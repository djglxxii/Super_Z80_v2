# Task Packet

## Title

M56 — Emulator Snapshot Tools

## Goal

Implement the first **Emulator Snapshot Tools** for the Super_Z80 frontend/runtime so developers can capture and restore a deterministic emulator state during interactive use.

This milestone should introduce a minimal, reliable snapshot facility that supports:

- saving the current emulator state into an in-memory snapshot
- restoring that snapshot later
- preserving deterministic execution when restored

This is a tooling milestone for debugging and repeatability.

Do not implement file export/import, snapshot history browsers, rewind systems, or multiple save slots beyond what is strictly needed for this milestone.

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

The frontend now exposes broad visibility into emulator state. The next milestone in the plan is **M56 — Emulator Snapshot Tools**.

The deterministic emulator core and headless execution paths must remain unchanged.

## Required Read Order

Before making changes read:

1. `docs/plan.md`
2. `docs/state_snapshot.md`
3. `docs/architecture.md`
4. `docs/decisions.md`
5. `AGENTS.md`
6. `artifacts/reports/M55_frame_timing_scheduler_panel_report.md`

Then inspect:

Frontend/runtime:

- `emulator/frontend/frontend.h`
- `emulator/frontend/frontend.cpp`
- `emulator/frontend/debug_panels/debug_panel_host.h`
- `emulator/frontend/debug_panels/debug_panel_host.cpp`
- `emulator/src/main.cpp`

Core/state-bearing subsystems:

- `emulator/include/emulator_core.h`
- `emulator/src/emulator_core.cpp`
- `emulator/include/bus.h`
- `emulator/src/bus.cpp`
- `emulator/include/cpu.h`
- `emulator/src/cpu.cpp`
- `emulator/include/memory.h`
- `emulator/src/memory.cpp`
- `emulator/include/vdp.h`
- `emulator/src/vdp.cpp`
- `emulator/include/dma.h`
- `emulator/src/dma.cpp`
- `emulator/include/io.h`
- `emulator/src/io.cpp`
- `emulator/include/apu.h`
- `emulator/src/apu.cpp`
- `emulator/audio/ym2151/ym2151.h`
- `emulator/audio/ym2151/ym2151.cpp`
- `emulator/include/scheduler.h`
- `emulator/src/scheduler.cpp`

Also inspect any existing state snapshot or serialization helpers already present in tests or internal APIs.

## Scope

Included:

- minimal save/restore snapshot capability
- in-memory snapshot representation
- frontend/runtime controls to save and restore snapshot
- deterministic restore behavior
- narrow unit/integration test coverage where practical
- frame-synchronized UI integration
- documentation update
- milestone report

Excluded:

- file-based snapshot persistence
- multiple named snapshot slots unless trivial
- rewind/rollback timeline
- diff viewers
- snapshot browser/history UI
- portability/versioned snapshot format
- speculative serialization framework

This milestone should remain narrow and practical.

## Milestone Definition

The plan defines M56 as:

Features:

- save emulator state
- load emulator state
- restore deterministic execution

Verification:

- snapshot restores identical execution

Purpose:

- enable debugging and reproducibility

## Architectural Constraints

Maintain strict separation between:

- emulator core
- platform shell
- frontend debug UI

The frontend may issue snapshot commands, but actual snapshot capture/restore must live in runtime/core-appropriate code.

Do not add UI dependencies to emulator subsystems.

Do not change emulator hardware behavior.

Any snapshot representation must be deterministic and fully owned by the emulator/runtime, not by SDL or ImGui.

## Snapshot Requirements

The snapshot must be sufficient to restore deterministic execution from the point captured.

At minimum, include all state necessary so that after restore, continuing execution yields the same emulator-visible behavior as if execution had never diverged.

That likely includes, as applicable:

- CPU state
- memory / RAM state
- ROM reference or currently loaded ROM identity handling
- bus-owned subsystem state
- VDP / VRAM / palette / scroll / sprite state
- DMA state
- IRQ/VBlank state
- input subsystem state
- scheduler timing state
- audio subsystem state
- YM2151 state
- internal emulator counters / audio buffer state if needed for deterministic continuation

Use the current implementation as the source of truth. Do not invent snapshot content that is not required.

## Implementation Requirements

### 1. Snapshot model

Introduce a minimal snapshot structure or set of structures sufficient to capture the current emulator state.

Acceptable approaches:

- a single `EmulatorCore::Snapshot` struct
- composed subsystem snapshot structs aggregated by `EmulatorCore`
- another narrow equivalent consistent with the repository style

Keep the API small and explicit.

### 2. Save and restore APIs

Add narrow APIs such as:

- capture snapshot
- restore snapshot

The exact names are up to the implementer, but they must be clear and read naturally from the runtime.

Do not add unnecessary abstraction layers.

### 3. Frontend/runtime controls

Add minimal UI/runtime controls to trigger:

- Save Snapshot
- Restore Snapshot

Acceptable locations:

- control panel
- debug tools panel
- menu bar

Keep the UI minimal.

If no snapshot has been saved yet, Restore must fail gracefully and visibly.

### 4. Determinism verification

Restore must resume execution in a way that preserves determinism.

At minimum, add practical verification showing that:

- take snapshot
- advance some amount
- restore snapshot
- advance the same amount again

produces identical observable results

Use the narrowest practical automated test, preferably at unit/integration level.

### 5. Snapshot storage policy

For this milestone, one in-memory snapshot slot is sufficient.

Do not build a general snapshot manager unless absolutely necessary.

### 6. Refresh behavior

The UI should reflect snapshot availability if useful, but keep this simple.

Snapshot operations must not introduce scheduler ownership changes or wall-clock timing dependencies.

## Files Expected to Change

Likely:

- `emulator/include/emulator_core.h`
- `emulator/src/emulator_core.cpp`
- `emulator/src/main.cpp`
- `emulator/frontend/debug_panels/debug_panel_host.h`
- `emulator/frontend/debug_panels/debug_panel_host.cpp`
- `emulator/frontend/frontend.h`
- `emulator/frontend/frontend.cpp`

Possibly several subsystem headers/sources if explicit snapshot structs/accessors are needed:

- CPU
- Bus
- VDP
- DMA
- IO
- APU
- YM2151
- Scheduler

Only add what is necessary for a deterministic restore.

Also likely:

- one or more test files under `emulator/tests/unit/`

## Files to Create

Create milestone report:

- `artifacts/reports/M56_emulator_snapshot_tools_report.md`

Create or update tests as needed for snapshot determinism verification.

## Constraints

Do not:

- refactor the entire emulator architecture
- add file-based snapshot persistence
- add rewind/history systems
- add new dependencies
- modify deterministic scheduling semantics
- modify ROM loading behavior beyond what is necessary for correct snapshot restore
- introduce speculative abstractions

Keep this milestone minimal and mergeable.

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

Also add and run a practical automated verification for snapshot restore determinism if possible.

Confirm:

* build succeeds
* tests pass
* frontend launches without crash
* snapshot restore path is verified as deterministically as practical

Automated visual confirmation of the ImGui controls is not expected; document that clearly in the report.

## Acceptance Criteria

* emulator builds successfully
* tests pass
* snapshot save operation exists
* snapshot restore operation exists
* restore without existing snapshot fails gracefully
* snapshot restore resumes deterministically
* headless mode unchanged
* `docs/state_snapshot.md` updated
* `artifacts/reports/M56_emulator_snapshot_tools_report.md` created

## Suggested Branch

`m56_emulator_snapshot_tools`

## Suggested Commit Message

`M56: add emulator snapshot tools`

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

M57 — Integrated Debug Overlay
