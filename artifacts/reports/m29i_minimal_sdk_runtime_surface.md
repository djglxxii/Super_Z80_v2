# M29i Minimal SDK Runtime Surface Report

Status: Completed

## Summary
Replaced the placeholder SDK directories with a real minimal ROM-development surface. The repository now contains public SDK include files, a reset/init runtime path, deterministic font and splash assets, and a starter ROM source that assembles against the checked-in SDK and boots through the M29h external ROM execution path.

## Files Changed
- `sdk/inc/super_z80.inc`
- `sdk/inc/vdp.inc`
- `sdk/inc/input.inc`
- `sdk/runtime/inc/runtime.inc`
- `sdk/runtime/src/reset.asm`
- `sdk/runtime/src/runtime_init.asm`
- `sdk/assets/font_8x8.asm`
- `sdk/assets/splash_tiles.asm`
- `sdk/assets/splash_tilemap.asm`
- `rom/starter/src/main.asm`
- `docs/showcase_state_snapshot.md`
- `docs/developer_guide.md`
- `docs/hardware_reference.md`
- `docs/programming_audio.md`
- `docs/state_snapshot.md`
- `artifacts/reports/m29i_minimal_sdk_runtime_surface.md`

## Commands Run
- `git status --short`
- `rg --files tasks/active docs | sort`
- `sed -n '1,220p' tasks/active/m29i_minimal_sdk_runtime_surface.md`
- `sed -n '1,220p' docs/state_snapshot.md`
- `sed -n '1,260p' docs/plan.md`
- `rg --files sdk rom emulator CMakeLists.txt cmake tests artifacts | sort`
- `rg -n "M29h|rom|assemble|assembler|sjasm|wla|z80asm|pasmo|sdasz80|rgbasm|sdk/runtime|super_z80.inc|runtime.inc" -S .`
- `sed -n '220,360p' tasks/active/m29i_minimal_sdk_runtime_surface.md`
- `sed -n '1,240p' artifacts/reports/m29h_rom_execution_contract.md`
- `find sdk -maxdepth 4 -type f | sort`
- `command -v z80asm || command -v sjasmplus || command -v pasmo || command -v sdasz80 || command -v rgbasm || true`
- `find rom -maxdepth 4 -type f | sort`
- `sed -n '1,220p' docs/showcase_state_snapshot.md`
- `sed -n '1,260p' docs/hardware_reference.md`
- `sed -n '1,220p' docs/programming_input.md`
- `sed -n '1,220p' emulator/src/main.cpp`
- `rg -n "sdk/|super_z80\\.inc|runtime\\.inc|Showcase ROM|SDK" docs emulator tasks/completed artifacts/reports -S`
- `ls -la sdk sdk/inc sdk/runtime sdk/runtime/inc sdk/runtime/src rom rom/starter`
- `sjasmplus --help | sed -n '1,220p'`
- `sed -n '1,220p' tasks/completed/M28_developer_docs.md`
- `sed -n '1,180p' docs/developer_guide.md`
- `tail -n 80 docs/state_snapshot.md`
- `sed -n '1,220p' emulator/include/vdp.h`
- `sed -n '1,220p' emulator/include/io.h`
- `rg -n "YM2151|FM|no canonical SDK|No SDK|future milestones|no implemented YM|Planned expansion" docs/*.md`
- `sed -n '1,220p' docs/programming_audio.md`
- `sjasmplus --nologo -I . --raw=build/test_sdk_rom.bin rom/starter/src/main.asm`
- `cmake -S . -B build`
- `cmake --build build`
- `ctest --test-dir build --output-on-failure`
- `./build/super_z80 --rom build/test_sdk_rom.bin --headless --frames 2`
- `./build/super_z80 --rom build/test_sdk_rom.bin --headless --frames 2`

## Verification Results
- `sjasmplus --nologo -I . --raw=build/test_sdk_rom.bin rom/starter/src/main.asm`: passed
- `cmake -S . -B build`: passed
- `cmake --build build`: passed
- `ctest --test-dir build --output-on-failure`: passed (`38/38`)
- `./build/super_z80 --rom build/test_sdk_rom.bin --headless --frames 2`: passed
- repeated headless ROM execution produced identical output:
  `HEADLESS_ROM_RESULT rom_crc32=0x8DDA2854 ram_crc32=0x9A494230 audio_crc32=0xCB7B98A6 frames=2 frame_counter=2 scanline_counter=0 audio_samples=1600`

## Issues Discovered
- `docs/plan.md` was already locally modified before task execution. The milestone work was kept aligned with the active task packet and the existing `M29i` plan entry without overwriting that user change.
- Several developer-facing docs still contained pre-M29 YM2151 wording. The task update corrected the directly impacted developer guides while keeping the milestone focused on SDK/runtime surface work.

## Next-Step Recommendation
Proceed to `M29j - SDK Integration Validation` to boot a minimal SDK-backed ROM through the emulator and validate the first Showcase-facing integration path.
