# M60 ROM Browser Integration Report

## Status
Completed

TASK_PACKET_MILESTONE: M60
NEXT_EXPECTED_MILESTONE_FROM_PLAN: M60
ALIGNMENT: PASS

## Summary

Replaced the frontend's manual-path-only `File -> Load ROM...` modal with a narrow ImGui ROM browser that shows the current directory, supports parent/up navigation, lists directories before files, and loads a selected ROM through the existing runtime-owned ROM load path. The browser remains frontend-local, preserves the manual path field as fallback, and persists the last browsed directory in the existing frontend settings file without changing emulator-core timing or headless behavior.

## Files Changed

- `docs/state_snapshot.md`
- `emulator/frontend/debug_panels/debug_panel_host.cpp`
- `emulator/frontend/debug_panels/debug_panel_host.h`
- `emulator/frontend/frontend.cpp`
- `emulator/frontend/frontend.h`

## Files Created

- `artifacts/reports/M60_rom_browser_integration_report.md`

## Commands Run

- `rg --files docs tasks/active`
- `sed -n '1,220p' docs/plan.md`
- `sed -n '1,260p' docs/state_snapshot.md`
- `sed -n '1,220p' docs/architecture.md`
- `sed -n '1,220p' docs/decisions.md`
- `sed -n '1,240p' AGENTS.md`
- `sed -n '1,240p' artifacts/reports/M59_display_scaling_window_sizing_report.md`
- `sed -n '1,260p' emulator/frontend/frontend.h`
- `sed -n '1,360p' emulator/frontend/frontend.cpp`
- `sed -n '1,260p' emulator/frontend/debug_panels/debug_panel_host.h`
- `sed -n '1,760p' emulator/frontend/debug_panels/debug_panel_host.cpp`
- `sed -n '1,1080p' emulator/src/main.cpp`
- `cmake -S . -B build`
- `cmake --build build`
- `ctest --test-dir build --output-on-failure`
- `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 1`
- `env SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software timeout 2s ./build/super_z80 --sdl-input --rom rom/showcase/build/showcase.bin`
- `env SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software SDL_AUDIODRIVER=dummy timeout 2s ./build/super_z80 --sdl-audio --rom rom/showcase/build/showcase.bin`
- `tmp=$(mktemp -d) ... env HOME=\"$tmp\" SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software timeout 2s ./build/super_z80 --sdl-input --rom rom/showcase/build/showcase.bin`

## Verification Results

- `cmake -S . -B build`: passed
- `cmake --build build`: passed
- `ctest --test-dir build --output-on-failure`: passed, `100% tests passed, 0 tests failed out of 39`
- `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 1`: passed, `HEADLESS_ROM_RESULT rom_crc32=0xD7F53636 ram_crc32=0x9A494230 audio_crc32=0xC5117D35 frames=1 frame_counter=1 scanline_counter=0 audio_samples=800`
- `env SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software timeout 2s ./build/super_z80 --sdl-input --rom rom/showcase/build/showcase.bin`: frontend startup observed under dummy SDL drivers, ROM loaded, terminated by `timeout`
- `env SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software SDL_AUDIODRIVER=dummy timeout 2s ./build/super_z80 --sdl-audio --rom rom/showcase/build/showcase.bin`: frontend startup observed under dummy SDL and dummy audio drivers, ROM loaded, terminated by `timeout`
- Temporary-home startup check with seeded `display_scale=4` and `rom_browser_directory=rom/showcase/build`: frontend startup observed under dummy SDL drivers, ROM loaded, terminated by `timeout`
- Automated verification confirms build, tests, headless execution, and browser-capable frontend startup remain stable; interactive confirmation of actual menu opening, directory navigation, and click-driven file selection still requires a real SDL desktop session

## Notes

- The browser persists the last browsed directory in the existing `super_z80_frontend_settings.cfg` file to stay within the prior narrow persistence mechanism.
- When no persisted browser directory exists, the browser defaults to the current ROM directory if a ROM is active, otherwise the process working directory.
- Invalid directories, unreadable entries, and unsupported/manual file selections fail safely through frontend status messaging and the existing runtime ROM loader error path.

## Recommended Next Step

Frontend milestone plan complete in `docs/plan.md`; define the next project milestone packet before further implementation.
