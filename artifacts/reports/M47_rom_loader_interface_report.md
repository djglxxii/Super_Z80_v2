# M47 ROM Loader Interface Report

## Status
Completed

TASK_PACKET_MILESTONE: M47
NEXT_EXPECTED_MILESTONE_FROM_PLAN: M47
ALIGNMENT: PASS

## Summary

Implemented a minimal frontend `File` menu with `Load ROM...` and `Reload ROM`, backed by shell-owned ROM load/reload handling that reuses the existing `initialize()` plus `load_rom()` runtime path. SDL input/audio shells now track the active ROM path, surface load status in the frontend, support optional `--rom <path>` startup loading, and preserve headless behavior unchanged.

## Files Changed

- `docs/state_snapshot.md`
- `emulator/frontend/debug_panels/debug_panel_host.cpp`
- `emulator/frontend/debug_panels/debug_panel_host.h`
- `emulator/src/main.cpp`
- `emulator/src/sdl_audio_output.cpp`
- `emulator/src/sdl_audio_output.h`

## Files Created

- `artifacts/reports/M47_rom_loader_interface_report.md`

## Commands Run

- `rg --files tasks/active docs`
- `sed -n '1,220p' docs/plan.md`
- `sed -n '1,260p' docs/state_snapshot.md`
- `sed -n '1,260p' tasks/active/M47_rom_loader_interface.md`
- `sed -n '120,240p' docs/plan.md`
- `rg -n "Frontend|ROM Loader|rom reload|ImGui|single-frame|Scheduler::kScanlinesPerFrame|sdl-input|sdl-audio" emulator tests CMakeLists.txt`
- `sed -n '1,220p' docs/architecture.md`
- `sed -n '1,220p' docs/decisions.md`
- `sed -n '1,240p' artifacts/reports/M46_emulator_control_panel_report.md`
- `sed -n '1,240p' emulator/frontend/frontend.h`
- `sed -n '1,280p' emulator/frontend/frontend.cpp`
- `sed -n '1,220p' emulator/frontend/debug_panels/debug_panel_host.h`
- `sed -n '1,260p' emulator/frontend/debug_panels/debug_panel_host.cpp`
- `sed -n '1,760p' emulator/src/main.cpp`
- `git status --short`
- `rg -n "load_rom\\(|initialize\\(|reset vector|ROM" emulator/include emulator/src tests`
- `sed -n '1,260p' emulator/include/emulator_core.h`
- `sed -n '1,320p' emulator/src/emulator_core.cpp`
- `sed -n '1,180p' emulator/src/bus.cpp`
- `sed -n '1,220p' emulator/include/bus.h`
- `sed -n '1,240p' emulator/src/sdl_audio_output.h`
- `sed -n '1,320p' emulator/src/sdl_audio_output.cpp`
- `cmake -S . -B build`
- `cmake --build build`
- `ctest --test-dir build --output-on-failure`
- `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 1`
- `env SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software timeout 2s ./build/super_z80 --sdl-input --rom rom/showcase/build/showcase.bin`
- `env SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software SDL_AUDIODRIVER=dummy timeout 2s ./build/super_z80 --sdl-audio --rom rom/showcase/build/showcase.bin`
- `env SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software ./build/super_z80 --sdl-input --rom /tmp/does_not_exist.bin`

## Verification Results

- `cmake -S . -B build`: passed
- `cmake --build build`: passed
- `ctest --test-dir build --output-on-failure`: passed
- `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 1`: passed, `HEADLESS_ROM_RESULT rom_crc32=0xD7F53636 ram_crc32=0x9A494230 audio_crc32=0xC5117D35 frames=1 frame_counter=1 scanline_counter=0 audio_samples=800`
- `env SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software timeout 2s ./build/super_z80 --sdl-input --rom rom/showcase/build/showcase.bin`: startup observed, ROM loaded through shell-owned runtime path, terminated by `timeout`
- `env SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software SDL_AUDIODRIVER=dummy timeout 2s ./build/super_z80 --sdl-audio --rom rom/showcase/build/showcase.bin`: startup observed, ROM loaded through shell-owned runtime path, terminated by `timeout`
- `env SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software ./build/super_z80 --sdl-input --rom /tmp/does_not_exist.bin`: passed, invalid ROM path failed clearly with `Unable to open ROM file: /tmp/does_not_exist.bin`

## Issues Encountered

- The shell-owned ROM load/reset path is exercised by automated SDL startup smoke checks, but the specific ImGui menu and modal click path is not covered by an automated UI harness in this milestone.

## Recommended Next Step

M48 - CPU Debug Panel
