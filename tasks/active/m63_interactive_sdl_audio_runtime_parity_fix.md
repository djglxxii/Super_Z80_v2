# Task Packet

## Title

M63 — Interactive SDL Audio Runtime Parity Fix

## Goal

Restore audible audio playback in the visible SDL interactive runtime so that ROMs generating PSG and YM2151 output produce sound when running with the interactive window.

The current emulator exposes audio playback only through the `--sdl-audio` shell while the visible interactive runtime (`--sdl-input`) does not open an SDL audio device or pump audio samples. Because `main()` dispatches the input shell before the audio shell, launching with both flags still results in a silent runtime.

This milestone must ensure the visible SDL runtime supports window, input, and audio simultaneously without altering deterministic emulator behavior.

## Repository Context

Repository investigation confirmed:

- The showcase ROM actively generates PSG and YM2151 audio.
- The emulator core produces valid mixed audio samples.
- Headless execution confirms non-zero audio output and deterministic CRCs.
- The visible SDL runtime (`run_sdl_input_shell`) never creates an `SdlAudioOutput`.
- The audio runtime (`run_sdl_audio_shell`) contains the audio device and pumping logic but does not provide the interactive window/input shell.

The platform shell is responsible for SDL windowing, input devices, audio device management, and the event loop. Therefore the correct fix belongs in the SDL runtime layer rather than the emulator core.

## Required Read Order

Before making changes, read in this order:

1. `docs/plan.md`
2. `docs/state_snapshot.md`
3. `docs/architecture.md`
4. `docs/decisions.md`
5. `AGENTS.md`
6. `artifacts/reports/M60_display_pipeline_bring_up_report.md`
7. `artifacts/reports/M59_display_scaling_window_sizing_report.md`

Then inspect runtime code:

- `emulator/src/main.cpp`
- `emulator/src/sdl_audio_output.cpp`
- `emulator/src/sdl_audio_output.h`
- `emulator/src/emulator_core.cpp`
- `emulator/src/scheduler.cpp`
- `emulator/frontend/frontend.cpp`

Also review the audio runtime investigation summary supplied with this task.

## Scope

Included:

- integrate SDL audio playback into the visible interactive runtime
- ensure visible SDL runtime supports simultaneous window, input, and audio
- resolve CLI ambiguity when both SDL flags are provided
- preserve deterministic emulator execution
- preserve headless runtime behavior
- update state snapshot documentation
- create milestone completion report

Excluded:

- ROM browser integration
- VDP renderer changes
- audio mixer or YM2151 implementation changes
- scheduler timing modifications
- new audio features (volume, mixing UI, etc.)
- fullscreen redesign
- SDL renderer architecture changes

This milestone strictly addresses SDL runtime integration.

## Milestone Definition

M63 — Interactive SDL Audio Runtime Parity Fix

Features:

- integrate SDL audio playback into the visible interactive runtime
- support simultaneous SDL window, input, and audio playback
- ensure CLI combinations do not silently disable audio
- maintain deterministic audio generation in the emulator core

Verification:

- visible SDL runtime produces audible ROM output
- interactive input and audio playback coexist correctly
- launching with `--sdl-input` produces sound
- launching with `--sdl-input --sdl-audio` still produces sound
- headless execution remains unchanged

Purpose:

- restore audible behavior in the visible SDL runtime
- remove misleading separation between interactive and audio-enabled shells

## Architectural Constraints

Maintain clear separation between:

- emulator core
- platform shell
- frontend UI

The emulator core generates audio samples.
The platform shell is responsible for opening the SDL audio device and pumping samples.

Do not move audio generation logic into the frontend.

Do not modify audio mixer behavior.

## Implementation Requirements

### 1. Unify SDL interactive runtime and audio playback

The visible SDL runtime must open and manage the SDL audio device.

Two acceptable approaches:

Option A (preferred):

- integrate `SdlAudioOutput` initialization into the interactive runtime
- reuse the existing audio pump logic used in `run_sdl_audio_shell`

Option B:

- unify the SDL input shell and SDL audio shell into a single runtime path supporting both input and audio.

Avoid large architectural refactors; prefer the minimal safe integration.

### 2. Pump audio samples during runtime

During the SDL runtime loop:

- retrieve audio samples from `EmulatorCore`
- send them to `SdlAudioOutput`
- ensure the SDL audio device queue remains fed.

The implementation should mirror the existing audio shell pumping behavior.

### 3. Resolve CLI ambiguity

Current CLI behavior selects the input shell before the audio shell.

Correct behavior should ensure:

- interactive runtime always includes audio capability
- passing both `--sdl-input` and `--sdl-audio` does not disable audio

Options include:

- treating `--sdl-input` as the unified interactive runtime
- deprecating separate runtime shells internally
- allowing `--sdl-audio` to imply interactive audio support.

Do not break existing CLI usage.

### 4. Maintain deterministic audio generation

The SDL audio device must consume samples produced by the emulator core without altering:

- scheduler timing
- audio mixing logic
- headless deterministic results.

SDL runtime playback must not affect headless CRC verification.

### 5. Runtime safety

The SDL runtime must behave safely when:

- no ROM is loaded
- audio device initialization fails
- dummy SDL drivers are used in automated verification.

If the SDL audio device cannot be opened, the emulator must continue running silently rather than crashing.

## Files Expected to Change

Likely:

- `emulator/src/main.cpp`
- `emulator/src/sdl_audio_output.cpp`
- `emulator/src/sdl_audio_output.h`

Possibly:

- small runtime plumbing changes in `emulator_core.cpp` if needed.

Avoid changes to audio mixer or scheduler code.

## Files to Create

Create milestone report:

- `artifacts/reports/M63_interactive_sdl_audio_runtime_parity_fix_report.md`

## Constraints

Do not:

- modify emulator hardware behavior
- redesign the audio mixer
- introduce new dependencies
- alter deterministic headless execution
- change ROM browser functionality
- add new audio UI controls.

This milestone is strictly an SDL runtime integration fix.

## Verification Commands

Run:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
````

Headless verification:

```bash
./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 48
```

Interactive runtime verification:

```bash
./build/super_z80 --sdl-input --rom rom/showcase/build/showcase.bin
```

Combined flags verification:

```bash
./build/super_z80 --sdl-input --sdl-audio --rom rom/showcase/build/showcase.bin
```

Dummy SDL driver verification:

```bash
env SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software SDL_AUDIODRIVER=dummy timeout 2s ./build/super_z80 --sdl-input --rom rom/showcase/build/showcase.bin
```

Confirm:

* build succeeds
* tests pass
* headless audio output unchanged
* interactive SDL runtime produces audible output
* dummy driver runtime does not crash.

Automated verification may not confirm audible output; document this clearly in the report.

## Acceptance Criteria

* emulator builds successfully
* tests pass
* visible SDL runtime plays ROM audio
* combined SDL flags still produce audio
* headless execution unchanged
* `docs/state_snapshot.md` updated
* milestone report created.

## Suggested Branch

`m63_interactive_sdl_audio_runtime_parity_fix`

## Suggested Commit Message

`M63: unify SDL interactive runtime with audio playback`

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

Resume frontend feature work with:

M64 — ROM Browser Integration
