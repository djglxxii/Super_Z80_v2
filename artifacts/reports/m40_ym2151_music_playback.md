# M40 YM2151 Music Playback Report

Status: Completed

## Milestone Preflight

```text
TASK_PACKET_MILESTONE: M40 – YM2151 Music Playback
NEXT_EXPECTED_MILESTONE_FROM_PLAN: M40 – YM2151 Music Playback
ALIGNMENT: PASS
```

## Summary

Extended the Showcase ROM with a ROM-local YM2151 music example. Startup now places the YM2151 into a known deterministic state, programs a minimal voice on channel `0`, and starts a simple four-note loop that continues during the existing parallax, controller, metasprite, animation, and PSG sound-effect scene.

## Files Changed

- `rom/showcase/inc/constants.inc`
- `rom/showcase/inc/memory_map.inc`
- `rom/showcase/src/main_loop.asm`
- `rom/showcase/src/vdp_init.asm`
- `docs/showcase_state_snapshot.md`
- `docs/state_snapshot.md`
- `artifacts/reports/m40_ym2151_music_playback.md`

## Commands Run

- `cmake -S . -B build`
- `cmake --build build`
- `ctest --test-dir build --output-on-failure`
- `make -C rom/showcase clean`
- `make -C rom/showcase`
- `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 32`
- `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 32`

## Verification Results

- Repository configuration completed successfully.
- Repository build completed successfully.
- `ctest` passed.
- The Showcase ROM assembled successfully with `sjasmplus`.
- Both required headless runs produced the same result:
  - `HEADLESS_ROM_RESULT rom_crc32=0xFCEDF42B ram_crc32=0x9A494230 audio_crc32=0xD8F49994 frames=32 frame_counter=32 scanline_counter=0 audio_samples=4096`

## Determinism Notes

- FM playback is driven only from ROM state updated once per frame.
- The note loop uses fixed note data, fixed `16`-frame note length, and explicit YM register writes with no host-timing dependence.

## Issues Discovered

- External ROM execution still does not have a repository-supported interactive SDL verification path for automated listening tests, so verification remains headless plus repository build/test coverage.

## Next-Step Recommendation

Proceed to `M41 - Audio Mixing Demonstration`.
