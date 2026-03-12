# Task Packet

## Title

M53 — Audio Debug Panel

## Goal

Implement a read-only **Audio Debug Panel** in the Super_Z80 frontend that exposes the current state of the audio subsystem for developer inspection.

This milestone must provide visibility into the **PSG** and **YM2151** state already present in the emulator, while preserving deterministic execution and keeping the panel purely observational.

This milestone is limited to **register/state inspection** plus lightweight activity/status display if such signals are already available or can be exposed through narrow read-only accessors.

Do not implement audio editing, transport controls, oscilloscope-style waveform rendering, spectrum analysis, or any change to synthesis behavior.

## Repository Context

The frontend plan defines **M53 — Audio Debug Panel** as the next milestone after M52. It specifically calls for visibility into the audio subsystem, including PSG registers, YM2151 registers, channel activity, and optional waveform output, with verification based on matching internal audio state. :contentReference[oaicite:0]{index=0}

This project follows AWOS. Work must remain narrowly scoped, deterministic, restartable from repository state, and verified with explicit commands and observable output. The builder must not invent new project goals or architectural changes beyond the current milestone. :contentReference[oaicite:1]{index=1}

The current frontend already includes:

- ImGui integration
- emulator control panel
- ROM loader interface
- CPU Debug panel
- Memory Viewer
- VRAM Viewer
- Sprite Debug panel
- DMA Debug panel

This milestone extends that same debug-panel pattern to the audio subsystem.

## Required Read Order

Before making changes, read in this order:

1. `docs/plan.md`
2. `docs/state_snapshot.md`
3. `docs/architecture.md`
4. `docs/decisions.md`
5. `AGENTS.md`
6. `artifacts/reports/M52_dma_debug_panel_report.md`

Then inspect the current frontend/debug implementation:

- `emulator/frontend/frontend.h`
- `emulator/frontend/frontend.cpp`
- `emulator/frontend/debug_panels/debug_panel_host.h`
- `emulator/frontend/debug_panels/debug_panel_host.cpp`

Then inspect the current audio implementation and runtime handoff:

- `emulator/include/emulator_core.h`
- `emulator/src/emulator_core.cpp`
- `emulator/include/apu.h`
- `emulator/src/apu.cpp`
- `emulator/include/ym2151.h`
- `emulator/src/ym2151.cpp`
- `emulator/src/main.cpp`

Also inspect any current SDL audio output files only if needed to understand what state is already available, but do not move host-audio concerns into this milestone.

## Scope

Included in scope:

- read-only `Audio Debug` ImGui panel
- display of PSG register state
- display of YM2151 register state
- display of audio enable/mute/control state if already represented
- minimal activity/status indicators if they are already available or can be exposed through narrow read-only accessors
- frame-synchronized refresh
- state snapshot update
- milestone completion report

Excluded from scope:

- editing audio registers
- muting/unmuting channels from UI
- waveform oscilloscope
- spectrum analyzer
- audio transport controls
- recording/export
- synthesis changes
- balancing/mixing changes
- host audio pipeline redesign
- speculative debugger abstractions

This milestone is **inspection-only**.

## Milestone Definition

The frontend plan defines M53 as follows:

Displayed information:

- PSG registers
- YM2151 registers
- channel activity
- waveform output (optional)

Verification:

- values match internal audio state

Purpose:

- allow debugging of audio generation :contentReference[oaicite:2]{index=2}

For this task, **waveform output remains optional and is not required**. Prioritize reliable register/state visibility first.

## Architectural Constraints

Maintain strict separation between:

- emulator core
- platform shell
- frontend debug UI

The frontend must not modify APU or YM2151 state.

Audio state must be obtained through **narrow read-only snapshot accessors** on existing emulator/audio subsystems.

Do not expose mutable references to internal register storage.

Do not add UI dependencies to the audio subsystem.

Do not move SDL audio-output concerns into the emulator core.

Keep the task as a small mergeable slice with explicit verification, consistent with 2026 agent workflow best practices: short-horizon tasks, explicit constraints, and deterministic verification. :contentReference[oaicite:3]{index=3}

## Audio Coverage Requirements

At minimum, the panel must expose:

### PSG

Display the current PSG register state already maintained by the emulator.

Use the actual register layout present in the implementation.

If the APU exposes decoded channel state cleanly and narrowly, it is acceptable to show small extra fields such as:

- tone period per channel
- volume per channel
- noise control
- global control / enable bits

But raw register display is sufficient if that is the narrowest correct implementation.

### YM2151

Display the current YM2151-visible register or state representation already stored by the emulator.

If the emulator stores only a subset of directly inspectable state, display that subset accurately rather than inventing a richer model.

### Status / Activity

If current implementation already has or can narrowly expose read-only values for:

- channel enabled/disabled
- current output active/silent
- IRQ pending
- buffered sample count
- mute/control bits

then display them.

Do not build a new audio instrumentation framework just for this milestone.

## Panel Layout

Create a window titled:

```text
Audio Debug
````

Suggested structure:

```text
Audio Debug

PSG
Reg 00: XX
Reg 01: XX
...
Reg 0F: XX

YM2151
Reg 00: XX
Reg 01: XX
...
Reg NN: XX

Status
Audio Enabled: Yes/No
YM IRQ: Yes/No
...
```

If space is limited, tables or collapsing sections are acceptable.

Readable compact layout matters more than visual flair.

## Implementation Requirements

### 1. Create Audio Debug Panel

Extend the existing debug panel host system.

Do not create a separate debug UI architecture.

Render alongside the existing panels:

* CPU Debug
* Memory Viewer
* VRAM Viewer
* Sprite Debug
* DMA Debug

### 2. Access audio state

Retrieve audio state through narrow read-only accessors or snapshot structs.

Possible approaches:

* APU snapshot struct
* YM2151 snapshot struct
* EmulatorCore-level combined audio snapshot

Avoid exposing mutable references.

Prefer a small snapshot API similar in style to the recently added CPU/memory/VRAM/sprite/DMA inspection accessors.

### 3. Display raw registers first

Prioritize correctness and narrow scope:

* raw PSG register/state display
* raw YM2151 register/state display

If decoded derived values are already naturally available, include them only if they do not significantly expand scope.

### 4. Refresh behavior

The panel must refresh every rendered frame.

The panel must not trigger emulator execution.

The panel must not modify timing or audio generation.

### 5. UI integration

The panel should appear automatically with the existing debug panels.

Use existing panel rendering conventions for consistency.

## Files Expected to Change

Likely:

* `emulator/frontend/debug_panels/debug_panel_host.h`
* `emulator/frontend/debug_panels/debug_panel_host.cpp`
* `emulator/frontend/frontend.h`
* `emulator/frontend/frontend.cpp`

Possibly:

* `emulator/include/emulator_core.h`
* `emulator/src/emulator_core.cpp`
* `emulator/include/apu.h`
* `emulator/src/apu.cpp`
* `emulator/include/ym2151.h`
* `emulator/src/ym2151.cpp`
* `emulator/src/main.cpp`

Only if narrow read-only snapshot/accessor plumbing is required.

## Files to Create

Create milestone report:

* `artifacts/reports/M53_audio_debug_panel_report.md`

## Constraints

Do not:

* refactor the audio subsystem
* change synthesis behavior
* change mixer behavior
* change SDL audio output behavior
* allow editing audio registers
* add dependencies
* modify deterministic scheduling
* modify ROM loading
* introduce speculative abstractions

Keep this milestone minimal and repository-truth-aligned.

## Verification Commands

Run:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

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

If a narrow unit test is practical for any new audio snapshot accessor behavior, add it.

Automated verification cannot fully confirm the ImGui panel visually; document that clearly in the report.

## Acceptance Criteria

* emulator builds successfully
* tests pass
* `Audio Debug` window appears
* PSG register/state values are visible
* YM2151 register/state values are visible
* any displayed status/activity fields match internal state
* panel refreshes every frame
* panel is read-only
* headless mode unchanged
* `docs/state_snapshot.md` updated
* `artifacts/reports/M53_audio_debug_panel_report.md` created

## Suggested Branch

`m53_audio_debug_panel`

## Suggested Commit Message

`M53: add audio debug panel`

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

M54 — Input Visualization Panel
