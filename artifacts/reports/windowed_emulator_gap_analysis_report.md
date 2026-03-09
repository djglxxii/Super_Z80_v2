# Windowed Emulator Gap Analysis Report

## Status
Partially Completed

This report captures the current repository state for running the emulator in a visible SDL window and identifies the missing work needed for windowed external-ROM execution.

## Summary

The repository already contains SDL2 integration for:

- build-time detection and linkage
- an SDL input shell
- an SDL audio shell

The repository does **not** yet contain a windowed video shell that renders the emulated VDP framebuffer to an SDL window during ROM execution.

External ROM execution is currently restricted to headless mode only.

## Current Implemented Pieces

### SDL build integration exists

SDL2 is discovered and linked conditionally in:

- `/home/djglxxii/src/Super_Z80_v2/emulator/CMakeLists.txt`

Relevant behavior:

- `find_package(SDL2)` is present
- `super_z80` receives `SUPER_Z80_HAS_SDL` when SDL2 is found
- SDL libraries and include directories are linked into the main executable

### SDL shell windows already exist

The main executable already creates SDL windows for non-video shell modes in:

- `/home/djglxxii/src/Super_Z80_v2/emulator/src/main.cpp`

Implemented today:

- `run_sdl_input_shell(EmulatorCore&)`
- `run_sdl_audio_shell(EmulatorCore&)`

This confirms the project is not blocked on basic SDL initialization or SDL window creation.

### VDP framebuffer data already exists

The VDP already owns:

- a `256 x 192` framebuffer
- a `256` entry RGBA palette
- per-pixel palette resolution helpers

Relevant files:

- `/home/djglxxii/src/Super_Z80_v2/emulator/include/vdp.h`
- `/home/djglxxii/src/Super_Z80_v2/emulator/src/vdp.cpp`

Relevant APIs already available:

- `framebuffer_pixel(x, y)`
- `framebuffer_color(pixel_index)`
- `render_frame()`
- `frame_ready()`

This means the emulated video state needed for a host window is already present in the core.

## Current Blocking Gaps

### 1. No SDL video presentation path

There is currently no implementation that:

- creates an SDL renderer or texture for emulator video output
- converts the VDP framebuffer into host-visible pixels each frame
- uploads those pixels into an SDL texture
- presents that texture to the window

There is no dedicated SDL video module comparable to the existing SDL audio output path.

### 2. External ROMs are rejected for SDL execution

`emulator/src/main.cpp` currently rejects any `--rom` usage combined with `--sdl-input` or `--sdl-audio` with this behavior:

- external ROM execution currently supports headless mode only

This is the direct reason a ROM cannot currently run in a visible SDL shell.

### 3. No windowed ROM loop

There is no runtime path that:

- loads an external ROM
- enters an SDL event loop
- advances emulation continuously
- renders completed frames to the window
- optionally maps SDL input into the controller ports during ROM execution

Today, the ROM path is:

- load ROM
- step a fixed frame count in headless mode
- emit deterministic summary text

That is sufficient for validation, but not for visible execution.

### 4. Frame pacing policy is not yet defined for video shell mode

The core is deterministic and scanline-driven. A windowed mode still needs an explicit host policy for:

- how long to run per host iteration
- whether to present on `frame_ready()` or after a full frame boundary
- whether to throttle to approximately `60 Hz`
- how to preserve deterministic core stepping while allowing manual inspection

The existing documentation already treats SDL modes as non-authoritative for timing, so this can remain a shell concern rather than a hardware concern.

## Code-Level Observations

### CLI and mode handling

In `/home/djglxxii/src/Super_Z80_v2/emulator/src/main.cpp`:

- command-line parsing supports `--sdl-input`, `--sdl-audio`, `--rom`, `--headless`, and `--frames`
- ROM execution defaults to headless when no SDL mode is selected
- ROM + SDL mode is explicitly rejected

Implication:

- a new explicit mode is needed for windowed ROM execution, or
- the current SDL shell flags need to be extended to allow ROM-backed execution

### Core video access

In `/home/djglxxii/src/Super_Z80_v2/emulator/include/vdp.h` and `/home/djglxxii/src/Super_Z80_v2/emulator/src/vdp.cpp`:

- `render_frame()` already produces the indexed framebuffer
- `framebuffer_color(pixel_index)` resolves palette-indexed pixels to RGBA

Implication:

- no immediate VDP redesign appears necessary just to draw the current framebuffer in an SDL window

### Scheduler behavior

In `/home/djglxxii/src/Super_Z80_v2/emulator/src/scheduler.cpp`:

- scanline stepping advances CPU, DMA, YM2151 timing, mixer state, VBlank, and VDP scanline state

Implication:

- the natural shell structure is to keep stepping scanlines and present after a completed frame boundary

## Recommended Minimal Implementation Plan

### Phase 1: Add SDL video shell support

Implement a narrow SDL-facing video presenter that:

- creates an SDL window sized for the `256 x 192` framebuffer with integer scaling
- creates an SDL renderer and streaming texture
- copies VDP RGBA output into the texture once per frame
- presents the frame

Suggested shape:

- keep this logic in a small SDL-specific file near the existing SDL shell code
- avoid moving video timing ownership out of `EmulatorCore` or the scheduler

### Phase 2: Add a windowed ROM run path

Add a ROM execution mode that:

- loads `--rom <path>`
- initializes SDL video
- optionally enables SDL input mapping already used by the input shell
- steps scanlines until one frame is ready
- calls `vdp.render_frame()`
- uploads and presents the framebuffer
- repeats until quit

This should be a separate runtime path from headless validation mode.

### Phase 3: Reuse existing controller mapping

The existing SDL input mapping logic in `main.cpp` can likely be reused directly for:

- keyboard directions
- buttons `A`, `B`, `Start`
- controller d-pad, analog threshold mapping, and standard buttons

This suggests the first windowed ROM mode can combine:

- ROM execution
- video presentation
- current input shell mapping

without introducing new hardware semantics.

### Phase 4: Keep shell timing clearly non-authoritative

The implementation should preserve the current architecture:

- emulator state advances from deterministic scanline stepping
- SDL does not own emulation timing
- host pacing only affects presentation and user interaction

If throttling is added, it should be documented as shell behavior only.

## Suggested CLI Direction

Any of these would be workable:

- `super_z80 --rom <path> --sdl-video`
- `super_z80 --rom <path> --window`
- `super_z80 --rom <path> --sdl-input --sdl-video`

Preferred practical direction:

- introduce a dedicated video/window flag instead of overloading `--sdl-input`

Reason:

- input, audio, and video become composable shell features
- the CLI remains explicit
- existing behavior stays backward compatible

## Suggested Verification

When implementation begins, likely verification should include:

- build succeeds with SDL enabled
- headless ROM execution remains unchanged
- windowed ROM mode initializes successfully under `SDL_VIDEODRIVER=dummy`
- windowed ROM mode can load `rom/showcase/build/showcase.bin`
- repeated headless runs remain deterministic
- SDL shell input still updates controller state correctly

Potential smoke commands:

- `cmake -S . -B build`
- `cmake --build build`
- `ctest --test-dir build --output-on-failure`
- `make -C rom/showcase`
- `env SDL_VIDEODRIVER=dummy ./build/super_z80 --rom rom/showcase/build/showcase.bin <future-window-flag>`

## Files Most Likely To Change

- `/home/djglxxii/src/Super_Z80_v2/emulator/src/main.cpp`
- `/home/djglxxii/src/Super_Z80_v2/emulator/CMakeLists.txt`

Possible new files:

- SDL video presenter implementation under `/home/djglxxii/src/Super_Z80_v2/emulator/src/`
- focused SDL video tests under `/home/djglxxii/src/Super_Z80_v2/emulator/tests/`

## Commands Run For This Analysis

- `find tasks/active -maxdepth 1 -type f | sort`
- `git status --short`
- `rg -n "SDL|window|renderer|headless|platform|display|framebuffer|blit|present|video" .`
- `find . -maxdepth 3 -type f | sort`
- `sed -n '1,260p' emulator/src/main.cpp`
- `sed -n '260,520p' emulator/src/main.cpp`
- `sed -n '1,220p' docs/developer_guide.md`
- `sed -n '1,260p' docs/state_snapshot.md`
- `sed -n '1,220p' docs/programming_input.md`
- `sed -n '180,260p' emulator/CMakeLists.txt`
- `sed -n '1,240p' emulator/include/emulator_core.h`
- `sed -n '1,220p' emulator/src/emulator_core.cpp`
- `sed -n '1,220p' emulator/include/vdp.h`
- `sed -n '1,240p' emulator/src/scheduler.cpp`
- `sed -n '1,220p' emulator/include/scheduler.h`
- `rg -n "render_frame\\(|frame_ready\\(" emulator/src emulator/tests`
- `sed -n '160,360p' emulator/src/vdp.cpp`
- `sed -n '130,220p' emulator/tests/integration/test_platform_determinism.cpp`

## Next-Step Recommendation

Create an explicit task packet for SDL video/windowed ROM execution before implementation starts.

The smallest coherent milestone would be:

- add a windowed SDL video shell for external ROM execution
- preserve current headless deterministic behavior
- optionally reuse current SDL input mapping in the same shell
