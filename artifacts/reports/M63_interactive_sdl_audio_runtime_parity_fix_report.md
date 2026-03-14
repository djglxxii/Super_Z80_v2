# M63 Interactive SDL Audio Runtime Parity Fix Report

TASK_PACKET_MILESTONE: M63
NEXT_EXPECTED_MILESTONE_FROM_PLAN: M63
ALIGNMENT: PASS

Status: Completed

Files Changed:
- `artifacts/reports/M63_interactive_sdl_audio_runtime_parity_fix_report.md`
- `docs/state_snapshot.md`
- `emulator/src/main.cpp`

Commands Run:
- `cmake -S . -B build`
- `cmake --build build`
- `ctest --test-dir build --output-on-failure`
- `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 48`
- `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 1`
- `env SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software SDL_AUDIODRIVER=dummy timeout 2s ./build/super_z80 --sdl-input --rom rom/showcase/build/showcase.bin`
- `env SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software SDL_AUDIODRIVER=dummy timeout 2s ./build/super_z80 --sdl-input --sdl-audio --rom rom/showcase/build/showcase.bin`
- `env SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software SDL_AUDIODRIVER=dummy timeout 2s ./build/super_z80 --sdl-audio --rom rom/showcase/build/showcase.bin`

Verification Results:
- Build succeeded.
- All `ctest` tests passed: `100% tests passed, 0 tests failed out of 39`.
- Headless ROM execution remained deterministic for `--frames 1`, producing `HEADLESS_ROM_RESULT rom_crc32=0x3C5D42C4 ram_crc32=0xB957D880 audio_crc32=0x8F537EF9 frames=1 frame_counter=1 scanline_counter=0 audio_samples=800`.
- Headless ROM execution also succeeded for `--frames 48`, producing `HEADLESS_ROM_RESULT rom_crc32=0x3C5D42C4 ram_crc32=0x1834E477 audio_crc32=0x15D6E68A frames=48 frame_counter=48 scanline_counter=0 audio_samples=4096`.
- Dummy-driver SDL startup succeeded for `--sdl-input --rom rom/showcase/build/showcase.bin`, `--sdl-input --sdl-audio --rom rom/showcase/build/showcase.bin`, and `--sdl-audio --rom rom/showcase/build/showcase.bin`; each launched the unified interactive runtime, loaded the showcase ROM, and terminated by expected `timeout` with exit code `124` rather than crashing.
- Automated verification confirms the interactive runtime now uses the same audio-capable shell path for all SDL flag combinations and safely tolerates dummy audio/video drivers.
- Direct audible confirmation in a real desktop SDL session is still required because automated dummy-driver checks cannot verify host speakers actually played sound.

Next-Step Recommendation:
- Manually run `./build/super_z80 --sdl-input --rom rom/showcase/build/showcase.bin` on a real desktop session to confirm audible PSG/YM2151 playback through host audio hardware.
