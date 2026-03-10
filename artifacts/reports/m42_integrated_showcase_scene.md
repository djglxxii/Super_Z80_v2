# M42 - Integrated Showcase Scene

## Task Status
Completed

## Milestone Preflight
TASK_PACKET_MILESTONE: M42
NEXT_EXPECTED_MILESTONE_FROM_PLAN: M42
ALIGNMENT: PASS

## Summary
- Updated the Showcase ROM foreground overlay so the running scene explicitly presents itself as the integrated platform demo.
- Preserved the existing deterministic frame flow and previously implemented M35-M41 behavior: parallax scrolling, controller-driven metasprite movement, sprite animation, YM2151 music playback, and PSG sound effects.
- Updated repository truth to mark M42 complete and advance the next expected milestone to M43.

## Files Changed
- `rom/showcase/src/text.asm`
- `docs/showcase_state_snapshot.md`
- `docs/state_snapshot.md`

## Commands Run
- `cmake -S . -B build`
- `cmake --build build`
- `ctest --test-dir build --output-on-failure`
- `make -C rom/showcase clean`
- `make -C rom/showcase`
- `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 48`
- `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 48`

## Verification Results
- `cmake -S . -B build`: passed
- `cmake --build build`: passed
- `ctest --test-dir build --output-on-failure`: passed (`38/38` tests)
- `make -C rom/showcase clean`: passed
- `make -C rom/showcase`: passed

## CRC Determinism Output
- Run 1: `HEADLESS_ROM_RESULT rom_crc32=0xD7F53636 ram_crc32=0x9A494230 audio_crc32=0xD8F49994 frames=48 frame_counter=48 scanline_counter=0 audio_samples=4096`
- Run 2: `HEADLESS_ROM_RESULT rom_crc32=0xD7F53636 ram_crc32=0x9A494230 audio_crc32=0xD8F49994 frames=48 frame_counter=48 scanline_counter=0 audio_samples=4096`
- Determinism: PASS

## Next-Step Recommendation
Proceed to `M43 - Developer Reference ROM Finalization`.
