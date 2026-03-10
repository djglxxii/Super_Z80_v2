# M39 PSG Sound Effects Report

Status: Completed

## Milestone Preflight

```text
TASK_PACKET_MILESTONE: M39 – PSG Sound Effects
NEXT_EXPECTED_MILESTONE_FROM_PLAN: M39 – PSG Sound Effects
ALIGNMENT: PASS
```

## Summary

Added a ROM-local PSG sound effect to the existing Showcase scene. The ROM now initializes PSG state explicitly at startup, detects a press edge on `PAD1` button `A`, starts a short tone burst on PSG tone channel A, and silences the channel again after a fixed `10`-frame duration without changing the existing parallax, controller movement, or metasprite animation flow.

## Files Changed

- `rom/showcase/inc/constants.inc`
- `rom/showcase/inc/memory_map.inc`
- `rom/showcase/src/init.asm`
- `rom/showcase/src/main_loop.asm`
- `rom/showcase/src/vdp_init.asm`
- `docs/showcase_state_snapshot.md`
- `docs/state_snapshot.md`
- `artifacts/reports/m39_psg_sound_effects.md`

## Commands Run

- `cmake -S . -B build`
- `cmake --build build`
- `ctest --test-dir build --output-on-failure`
- `make -C rom/showcase clean`
- `make -C rom/showcase`
- `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 24`
- `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 24`

## Verification Results

- Repository configuration completed successfully.
- Repository build completed successfully.
- `ctest` passed with `38/38` tests passing.
- The Showcase ROM assembled successfully with `sjasmplus`.
- Both required headless runs produced the same result:
  - `HEADLESS_ROM_RESULT rom_crc32=0xCDD4CAD9 ram_crc32=0x9A494230 audio_crc32=0xD8F49994 frames=24 frame_counter=24 scanline_counter=0 audio_samples=4096`

## Trigger Behavior

- Trigger button: `PAD1` button `A`
- Trigger mode: press edge only
- PSG channel used: tone channel A
- Tone period: `$0060`
- Tone volume: `$02`
- Duration: `10` frames

## Issues Discovered

- External ROM execution currently supports headless mode only in the CLI, so there is no repository-supported automated way to interactively press `A` and listen to the effect through SDL during `--rom` execution.

## Next-Step Recommendation

Proceed to `M40 - YM2151 Music Playback`.
