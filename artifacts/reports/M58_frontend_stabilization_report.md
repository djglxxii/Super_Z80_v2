# M58 Frontend Stabilization Report

## Status
Completed

TASK_PACKET_MILESTONE: M58
NEXT_EXPECTED_MILESTONE_FROM_PLAN: M58
ALIGNMENT: PASS

## Summary

Stabilized the frontend by fixing first-use panel placement so default debug windows stay inside the active SDL viewport, and by switching ImGui layout persistence to an SDL preference-directory-backed ini path with explicit load/save handling. These changes stay in the frontend layer and do not alter deterministic emulator execution or headless behavior.

## Files Changed

- `docs/state_snapshot.md`
- `emulator/frontend/debug_panels/debug_panel_host.cpp`
- `emulator/frontend/frontend.cpp`

## Files Created

- `artifacts/reports/M58_frontend_stabilization_report.md`

## Commands Run

- `git status --short`
- `cmake -S . -B build`
- `cmake --build build`
- `ctest --test-dir build --output-on-failure`
- `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 1`
- `env SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software timeout 2s ./build/super_z80 --sdl-input --rom rom/showcase/build/showcase.bin`
- `env SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software SDL_AUDIODRIVER=dummy timeout 2s ./build/super_z80 --sdl-audio --rom rom/showcase/build/showcase.bin`

## Verification Results

- `cmake -S . -B build`: passed
- `cmake --build build`: passed
- `ctest --test-dir build --output-on-failure`: passed, `100% tests passed, 0 tests failed out of 39`
- `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 1`: passed, `HEADLESS_ROM_RESULT rom_crc32=0xD7F53636 ram_crc32=0x9A494230 audio_crc32=0xC5117D35 frames=1 frame_counter=1 scanline_counter=0 audio_samples=800`
- `env SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software timeout 2s ./build/super_z80 --sdl-input --rom rom/showcase/build/showcase.bin`: frontend startup observed under dummy SDL drivers, ROM loaded, terminated by `timeout`
- `env SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software SDL_AUDIODRIVER=dummy timeout 2s ./build/super_z80 --sdl-audio --rom rom/showcase/build/showcase.bin`: frontend startup observed under dummy SDL and audio drivers, ROM loaded, terminated by `timeout`
- Automated visual confirmation of panel placement, visibility toggles, and restored layout contents is not available in this environment and still requires a real SDL session

## Next-Step Recommendation

Run a manual SDL session to confirm the stabilized default panel layout and persisted window arrangement feel reasonable on a real desktop environment.
