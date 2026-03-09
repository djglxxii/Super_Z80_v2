# M35 Parallax Scrolling Demo Report

## Status
Completed.

## Summary
Extended the Showcase ROM so it now composes and uploads both background and foreground tilemaps, keeps explicit per-layer scroll state in RAM, and applies separate BG/FG scroll register writes during render. The background layer carries the splash/logo composition and pattern bands, while the foreground layer carries transparent text banners that move faster horizontally, making the parallax effect clear without changing the asset pipeline.

## Files Changed
- `rom/showcase/inc/constants.inc`
- `rom/showcase/inc/memory_map.inc`
- `rom/showcase/src/init.asm`
- `rom/showcase/src/main_loop.asm`
- `rom/showcase/src/text.asm`
- `rom/showcase/src/vdp_init.asm`
- `docs/showcase_state_snapshot.md`
- `docs/state_snapshot.md`
- `artifacts/reports/m35_parallax_scrolling_demo.md`

## Commands Run
- `cmake -S . -B build`
- `cmake --build build`
- `ctest --test-dir build --output-on-failure`
- `make -C rom/showcase clean`
- `make -C rom/showcase`
- `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 10`
- `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 10`

## Verification Results
- Repository configure/build succeeded.
- `ctest` passed: `38/38` tests.
- Showcase ROM assembled successfully with `sjasmplus`.
- Repeated headless runs were deterministic and produced identical output:
  - `HEADLESS_ROM_RESULT rom_crc32=0xF1052A9D ram_crc32=0x9A494230 audio_crc32=0xD8F49994 frames=10 frame_counter=10 scanline_counter=0 audio_samples=4096`

## Issues Discovered
- A first draft of the background composition wrote decorative rows before copying the full splash tilemap, which would have cleared those rows back to zero. The final implementation copies the splash tilemap first and then overlays the decorative background text bands.
- A first draft of the foreground banner text exceeded the `32`-tile row width, which assembly does not reject. The final banner strings were shortened to preserve deterministic row-local writes.

## Next-Step Recommendation
Proceed to `M36 - Basic Sprite Rendering` and keep the current two-plane Showcase composition intact while adding a small deterministic sprite path on top.
