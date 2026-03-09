# M29j SDK Integration Validation Report

## Status
Completed

## Summary
Implemented the first Showcase-oriented ROM under `rom/showcase/` and validated that it boots through the checked-in SDK runtime. The new ROM assembles against `sdk/inc/super_z80.inc` and `sdk/runtime/inc/runtime.inc`, initializes a minimal palette, streams the repository font and splash tiles into VRAM through the current sequential VDP path, clears a background tilemap buffer, writes `SUPER Z80 SDK` and `BOOT VALIDATED`, and executes deterministically through the emulator's external ROM headless flow.

## Files Changed
- `rom/showcase/src/main.asm`
- `rom/showcase/Makefile`
- `docs/showcase_state_snapshot.md`
- `docs/state_snapshot.md`
- `artifacts/reports/m29j_sdk_integration_validation.md`

## Commands Run
- `make -C rom/showcase`
- `sjasmplus --nologo -I . --raw=build/showcase_validation.bin rom/showcase/src/main.asm`
- `cmake -S . -B build`
- `cmake --build build`
- `ctest --test-dir build --output-on-failure`
- `./build/super_z80 --rom build/showcase_validation.bin --headless --frames 4`
- `./build/super_z80 --rom build/showcase_validation.bin --headless --frames 4`

## Verification Results
- `make -C rom/showcase`: passed
- `sjasmplus --nologo -I . --raw=build/showcase_validation.bin rom/showcase/src/main.asm`: passed
- `cmake -S . -B build`: passed
- `cmake --build build`: passed
- `ctest --test-dir build --output-on-failure`: passed (`38/38` tests)
- repeated headless ROM execution: passed with identical output on both runs

Headless output:

```text
Emulator core initialized (audio output ready).
HEADLESS_ROM_RESULT rom_crc32=0xD1C100F5 ram_crc32=0x86844D41 audio_crc32=0x1F3134B frames=4 frame_counter=4 scanline_counter=0 audio_samples=3200
```

## Issues Discovered
- None.

## Next-Step Recommendation
Proceed to `M30 - Showcase ROM Project Scaffold` to expand `rom/showcase/` into the canonical runnable Showcase project baseline for later content milestones.
