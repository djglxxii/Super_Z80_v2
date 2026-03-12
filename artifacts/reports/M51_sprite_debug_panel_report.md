# M51 Sprite Debug Panel Report

## Status
Completed

TASK_PACKET_MILESTONE: M51
NEXT_EXPECTED_MILESTONE_FROM_PLAN: M51
ALIGNMENT: PASS

## Summary

Implemented a read-only `Sprite Debug` frontend panel that renders alongside the existing debug panels and refreshes every frame with the full SAT contents. Added a narrow sprite table snapshot accessor on `EmulatorCore`, routed that snapshot through the existing shell-owned runtime-state handoff, and kept the UI limited to attribute inspection with `Index`, `X`, `Y`, `Tile`, and raw `Attr` values for all `64` sprite entries.

## Files Changed

- `docs/state_snapshot.md`
- `emulator/frontend/debug_panels/debug_panel_host.cpp`
- `emulator/frontend/debug_panels/debug_panel_host.h`
- `emulator/include/emulator_core.h`
- `emulator/src/emulator_core.cpp`
- `emulator/src/main.cpp`
- `emulator/tests/unit/test_emulator_core.cpp`

## Files Created

- `artifacts/reports/M51_sprite_debug_panel_report.md`

## Commands Run

- `pwd && rg --files docs tasks/active | sort`
- `sed -n '1,220p' docs/plan.md`
- `sed -n '1,260p' docs/state_snapshot.md`
- `sed -n '1,260p' tasks/active/m51_sprite_debug_panel.md`
- `rg -n "^# M5[0-9]|^## Current Milestone|next official frontend milestone|M51|M52" docs/plan.md docs/state_snapshot.md tasks/active/m51_sprite_debug_panel.md`
- `sed -n '300,345p' docs/plan.md`
- `sed -n '1,240p' docs/architecture.md`
- `sed -n '1,240p' docs/decisions.md`
- `sed -n '1,260p' artifacts/reports/M50_vram_viewer_report.md`
- `sed -n '1,260p' emulator/frontend/debug_panels/debug_panel_host.h`
- `sed -n '1,420p' emulator/frontend/debug_panels/debug_panel_host.cpp`
- `sed -n '1,260p' emulator/frontend/frontend.h`
- `sed -n '1,320p' emulator/frontend/frontend.cpp`
- `sed -n '1,280p' emulator/include/vdp.h`
- `sed -n '1,360p' emulator/src/vdp.cpp`
- `sed -n '1,280p' emulator/include/emulator_core.h`
- `sed -n '1,360p' emulator/src/emulator_core.cpp`
- `rg -n "cpu_debug_state|memory_viewer_state|vram_viewer_state|set_runtime_control_state|RuntimeControlState|Frontend" emulator/src/main.cpp emulator/tests/unit -g'*.cpp' -g'*.h'`
- `rg -n "SpriteAttributeTable|kSpriteAttributeTableBase|kMaxSprites|kSpriteSizeBytes|sprite" emulator/tests/unit emulator/src emulator/include -g'*.cpp' -g'*.h'`
- `sed -n '260,340p' emulator/src/main.cpp`
- `sed -n '1,260p' emulator/tests/unit/test_emulator_core.cpp`
- `sed -n '260,420p' tasks/active/m51_sprite_debug_panel.md`
- `sed -n '1,220p' emulator/CMakeLists.txt`
- `git status --short`
- `cmake -S . -B build`
- `git diff -- emulator/include/emulator_core.h emulator/src/emulator_core.cpp emulator/frontend/debug_panels/debug_panel_host.h emulator/frontend/debug_panels/debug_panel_host.cpp emulator/src/main.cpp emulator/tests/unit/test_emulator_core.cpp`
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
- Visual confirmation that the `Sprite Debug` ImGui window renders cannot be automated in this environment

## Issues Encountered

- The frontend panel initially assumed access to `vdp.h`; that dependency was removed in favor of local SAT constants to keep frontend wiring narrow.
- The active task packet existed as an untracked file in `tasks/active/`; work proceeded because the packet contents aligned with `docs/plan.md` and `docs/state_snapshot.md`.
- The task packet's suggested next step named `M52 — Tile Viewer`, but `docs/plan.md` defines `M52 — DMA Debug Panel`; repository truth remains the authoritative sequence.

## Recommended Next Step

M52 - DMA Debug Panel
