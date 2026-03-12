# M53 Audio Debug Panel Report

## Status
Completed

TASK_PACKET_MILESTONE: M53
NEXT_EXPECTED_MILESTONE_FROM_PLAN: M53
ALIGNMENT: PASS

## Summary

Implemented a read-only `Audio Debug` frontend panel that renders alongside the existing debug panels and refreshes every frame with live PSG and YM2151 state. Added narrow read-only snapshot accessors on `APU` and `YM2151`, exposed a combined audio snapshot through `EmulatorCore`, and routed it through the existing shell-owned frontend runtime-state handoff so the UI can inspect raw registers, decoded channel activity, timer/IRQ status, and current sample values without modifying deterministic execution.

## Files Changed

- `docs/state_snapshot.md`
- `emulator/audio/ym2151/ym2151.cpp`
- `emulator/audio/ym2151/ym2151.h`
- `emulator/frontend/debug_panels/debug_panel_host.cpp`
- `emulator/frontend/debug_panels/debug_panel_host.h`
- `emulator/include/apu.h`
- `emulator/include/emulator_core.h`
- `emulator/src/apu.cpp`
- `emulator/src/emulator_core.cpp`
- `emulator/src/main.cpp`
- `emulator/tests/unit/test_emulator_core.cpp`

## Files Created

- `artifacts/reports/M53_audio_debug_panel_report.md`

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
- Visual confirmation that the `Audio Debug` ImGui window renders cannot be automated in this environment

## Next-Step Recommendation

M54 - Input Visualization Panel
