# M30 Showcase ROM Project Scaffold

## Status
Completed.

## Summary
Restructured `rom/showcase/` into the canonical Showcase ROM project scaffold. The ROM now has modular source files for entry, initialization, VDP upload helpers, text/tilemap preparation, and the main loop; local asset ownership under `rom/showcase/assets/`; include files for project constants and memory layout; and a repository-local `Makefile` that builds `rom/showcase/build/showcase.bin`.

## Files Changed
- `rom/showcase/Makefile`
- `rom/showcase/assets/font_8x8.asm`
- `rom/showcase/assets/splash_tilemap.asm`
- `rom/showcase/assets/splash_tiles.asm`
- `rom/showcase/build/.gitkeep`
- `rom/showcase/inc/constants.inc`
- `rom/showcase/inc/memory_map.inc`
- `rom/showcase/inc/vdp_helpers.inc`
- `rom/showcase/src/init.asm`
- `rom/showcase/src/main.asm`
- `rom/showcase/src/main_loop.asm`
- `rom/showcase/src/text.asm`
- `rom/showcase/src/vdp_init.asm`
- `docs/showcase_state_snapshot.md`
- `docs/state_snapshot.md`

## Commands Run
- `cmake -S . -B build`
- `cmake --build build`
- `ctest --test-dir build --output-on-failure`
- `make -C rom/showcase clean`
- `make -C rom/showcase`
- `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 4`
- `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 4`

## Verification Results
- `cmake --build build`: passed
- `ctest --test-dir build --output-on-failure`: passed (`38/38` tests)
- `make -C rom/showcase`: passed and produced `rom/showcase/build/showcase.bin`
- First headless ROM run: passed
  `HEADLESS_ROM_RESULT rom_crc32=0xB05BAA8F ram_crc32=0x9A494230 audio_crc32=0x1F3134B frames=4 frame_counter=4 scanline_counter=0 audio_samples=3200`
- Second headless ROM run: passed with identical output
  `HEADLESS_ROM_RESULT rom_crc32=0xB05BAA8F ram_crc32=0x9A494230 audio_crc32=0x1F3134B frames=4 frame_counter=4 scanline_counter=0 audio_samples=3200`

## Next-Step Recommendations
- Proceed with `M31 - Font and Text Rendering`.
- Build a reusable text renderer on top of the scaffolded `showcase_write_ascii_line` path and current local font asset ownership.
