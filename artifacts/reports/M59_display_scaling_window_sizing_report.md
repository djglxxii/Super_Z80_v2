# M59 Display Scaling & Window Sizing Report

## Status
Completed

TASK_PACKET_MILESTONE: M59
NEXT_EXPECTED_MILESTONE_FROM_PLAN: M59
ALIGNMENT: PASS

## Summary

Added frontend-local integer display scaling for the SDL shells with `2x`, `4x`, and `6x` presets, a new `View` menu selector, immediate SDL window resizing, and narrow persistence of the selected scale through an SDL preference-directory-backed settings file. The implementation stays in the frontend/runtime shell layer and does not alter emulator-core timing, scheduler behavior, audio generation, or headless execution.

## Files Changed

- `docs/state_snapshot.md`
- `emulator/frontend/debug_panels/debug_panel_host.cpp`
- `emulator/frontend/debug_panels/debug_panel_host.h`
- `emulator/frontend/frontend.cpp`
- `emulator/frontend/frontend.h`
- `emulator/src/main.cpp`

## Files Created

- `artifacts/reports/M59_display_scaling_window_sizing_report.md`

## Commands Run

- `git status --short`
- `cmake -S . -B build`
- `cmake --build build`
- `ctest --test-dir build --output-on-failure`
- `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 1`
- `env SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software timeout 2s ./build/super_z80 --sdl-input --rom rom/showcase/build/showcase.bin`
- `env SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software SDL_AUDIODRIVER=dummy timeout 2s ./build/super_z80 --sdl-audio --rom rom/showcase/build/showcase.bin`
- `tmp=$(mktemp -d) ... env HOME="$tmp" SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software timeout 2s ./build/super_z80 --sdl-input --rom rom/showcase/build/showcase.bin`

## Verification Results

- `cmake -S . -B build`: passed
- `cmake --build build`: passed
- `ctest --test-dir build --output-on-failure`: passed, `100% tests passed, 0 tests failed out of 39`
- `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 1`: passed, `HEADLESS_ROM_RESULT rom_crc32=0xD7F53636 ram_crc32=0x9A494230 audio_crc32=0xC5117D35 frames=1 frame_counter=1 scanline_counter=0 audio_samples=800`
- `env SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software timeout 2s ./build/super_z80 --sdl-input --rom rom/showcase/build/showcase.bin`: frontend startup observed under dummy SDL drivers, ROM loaded, terminated by `timeout`
- `env SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software SDL_AUDIODRIVER=dummy timeout 2s ./build/super_z80 --sdl-audio --rom rom/showcase/build/showcase.bin`: frontend startup observed under dummy SDL and audio drivers, ROM loaded, terminated by `timeout`
- Seeded persisted-scale startup check under a temporary `HOME` with `display_scale=4`: passed, frontend startup observed under dummy SDL drivers and ROM loaded without crash
- Automated verification confirms the scale-selection path is integrated and that persisted startup sizing is exercised; direct visual confirmation of actual on-screen resized window dimensions and menu-driven scaling behavior still requires a real SDL desktop session

## Issues Encountered

- The first build failed because `frontend.cpp` tried to include `vdp.h` from a non-exported include path for the frontend target. This was resolved by keeping the native `256x192` framebuffer dimensions local to the frontend sizing helper instead of adding a new cross-target include dependency.

## Recommended Next Step

M60 — ROM Browser Integration
