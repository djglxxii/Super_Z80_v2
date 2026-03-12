# M50 VRAM Viewer Report

## Status
Completed

TASK_PACKET_MILESTONE: M50
NEXT_EXPECTED_MILESTONE_FROM_PLAN: M50
ALIGNMENT: PASS

## Summary

Implemented a read-only `VRAM Viewer` frontend panel that renders alongside the existing debug panels and refreshes every frame with live VDP VRAM contents. Added a narrow VRAM snapshot accessor on `EmulatorCore`, routed that snapshot through the existing shell-owned runtime-state handoff, and kept viewer navigation local to the frontend with clamped hex paging across the full `0x0000-0xFFFF` VRAM range.

## Files Changed

- `docs/state_snapshot.md`
- `emulator/frontend/debug_panels/debug_panel_host.cpp`
- `emulator/frontend/debug_panels/debug_panel_host.h`
- `emulator/include/emulator_core.h`
- `emulator/src/emulator_core.cpp`
- `emulator/src/main.cpp`
- `emulator/tests/unit/test_emulator_core.cpp`

## Files Created

- `artifacts/reports/M50_vram_viewer_report.md`

## Commands Run

- `rg --files docs tasks/active`
- `sed -n '1,240p' docs/plan.md`
- `sed -n '1,240p' docs/state_snapshot.md`
- `sed -n '1,240p' tasks/active/M50_vram_viewer.md`
- `sed -n '241,520p' tasks/active/M50_vram_viewer.md`
- `sed -n '1,220p' docs/architecture.md`
- `sed -n '1,220p' docs/decisions.md`
- `sed -n '1,220p' artifacts/reports/M49_memory_viewer_report.md`
- `sed -n '1,260p' emulator/frontend/frontend.h`
- `sed -n '1,360p' emulator/frontend/frontend.cpp`
- `sed -n '1,260p' emulator/frontend/debug_panels/debug_panel_host.h`
- `sed -n '1,420p' emulator/frontend/debug_panels/debug_panel_host.cpp`
- `sed -n '1,260p' emulator/include/vdp.h`
- `sed -n '1,360p' emulator/src/vdp.cpp`
- `sed -n '1,260p' emulator/include/emulator_core.h`
- `sed -n '1,360p' emulator/src/emulator_core.cpp`
- `sed -n '1,260p' emulator/include/bus.h`
- `sed -n '1,320p' emulator/src/bus.cpp`
- `rg -n "# M49|# M50|# M51|M50|M51" docs/plan.md`
- `sed -n '140,260p' docs/plan.md`
- `rg -n "memory_viewer_state|cpu_debug_state|set_runtime_control_state|rom_snapshot|ram_snapshot|RuntimeControlState" emulator/src/main.cpp emulator/tests/unit emulator/CMakeLists.txt -g'*.cpp' -g'*.h'`
- `sed -n '1,360p' emulator/src/main.cpp`
- `sed -n '1,260p' emulator/tests/unit/test_emulator_core.cpp`
- `sed -n '260,520p' emulator/tests/unit/test_emulator_core.cpp`
- `sed -n '1,240p' emulator/CMakeLists.txt`
- `git status --short`
- `cmake -S . -B build`
- `cmake --build build`
- `./build/emulator/super_z80_test_emulator_core`
- `ctest --test-dir build --output-on-failure`
- `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 1`
- `env SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software timeout 2s ./build/super_z80 --sdl-input --rom rom/showcase/build/showcase.bin`
- `env SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software SDL_AUDIODRIVER=dummy timeout 2s ./build/super_z80 --sdl-audio --rom rom/showcase/build/showcase.bin`

## Verification Results

- `cmake -S . -B build`: passed
- `cmake --build build`: passed
- `./build/emulator/super_z80_test_emulator_core`: passed
- `ctest --test-dir build --output-on-failure`: passed, `100% tests passed, 0 tests failed out of 39`
- `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 1`: passed, `HEADLESS_ROM_RESULT rom_crc32=0xD7F53636 ram_crc32=0x9A494230 audio_crc32=0xC5117D35 frames=1 frame_counter=1 scanline_counter=0 audio_samples=800`
- `env SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software timeout 2s ./build/super_z80 --sdl-input --rom rom/showcase/build/showcase.bin`: frontend startup observed under dummy SDL drivers, ROM loaded, terminated by `timeout`
- `env SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software SDL_AUDIODRIVER=dummy timeout 2s ./build/super_z80 --sdl-audio --rom rom/showcase/build/showcase.bin`: frontend startup observed under dummy SDL and audio drivers, ROM loaded, terminated by `timeout`
- Visual confirmation that the `VRAM Viewer` ImGui window renders cannot be automated in this environment

## Issues Encountered

- The active task packet existed as an untracked file in `tasks/active/`; work proceeded because the packet contents aligned with `docs/plan.md` and `docs/state_snapshot.md`

## Recommended Next Step

M51 - Sprite Debug Panel
