# M29h ROM Execution Contract Report

Status: Completed

## Summary
Implemented the minimum external ROM execution contract in the emulator CLI. `super_z80` now loads ROM bytes from disk with `--rom <path>`, executes them through the existing bus/core boot path in deterministic headless mode, and reports a stable execution summary suitable for scripted verification.

## Files Changed
- `emulator/include/emulator_core.h`
- `emulator/src/emulator_core.cpp`
- `emulator/src/main.cpp`
- `emulator/tests/integration/test_external_rom_execution.cpp`
- `emulator/CMakeLists.txt`
- `docs/showcase_state_snapshot.md`
- `artifacts/reports/m29h_rom_execution_contract.md`

## Commands Run
- `git status --short`
- `rg --files tasks/active docs | sort`
- `sed -n '1,220p' tasks/active/m29h_sdk_integration_validation.md`
- `sed -n '1,220p' docs/state_snapshot.md`
- `sed -n '1,260p' docs/plan.md`
- `rg -n "main\\(|argv|headless|rom|cartridge|EmulatorCore|SDL" emulator tests CMakeLists.txt`
- `rg --files emulator tests | sort`
- `sed -n '1,420p' emulator/src/main.cpp`
- `sed -n '1,220p' emulator/include/emulator_core.h`
- `sed -n '1,220p' emulator/src/emulator_core.cpp`
- `sed -n '1,220p' emulator/include/bus.h`
- `sed -n '1,220p' emulator/src/bus.cpp`
- `sed -n '1,760p' emulator/CMakeLists.txt`
- `sed -n '1,220p' emulator/include/crc32.h`
- `sed -n '1,220p' emulator/src/crc32.cpp`
- `cmake -S . -B build`
- `cmake --build build`
- `ctest --test-dir build --output-on-failure`
- `printf '\\x3A\\x00\\xC0\\x3C\\x32\\x00\\xC0\\x3A\\x01\\xC0\\x3C\\x3C\\x32\\x01\\xC0\\xC3\\x00\\x00' > build/m29h_manual_rom.bin`
- `./build/super_z80 --rom build/m29h_manual_rom.bin --headless --frames 2`
- `./build/super_z80 --rom build/m29h_manual_rom.bin --headless --frames 2`
- `./build/super_z80 --rom /does/not/exist.bin --headless --frames 1`

## Verification Results
- `cmake -S . -B build`: passed
- `cmake --build build`: passed
- `ctest --test-dir build --output-on-failure`: passed (`38/38`)
- `./build/super_z80 --rom build/m29h_manual_rom.bin --headless --frames 2`: passed
- repeated manual ROM command produced identical output:
  `HEADLESS_ROM_RESULT rom_crc32=0x56A11E2B ram_crc32=0xFFE82747 audio_crc32=0xCB7B98A6 frames=2 frame_counter=2 scanline_counter=0 audio_samples=1600`
- `./build/super_z80 --rom /does/not/exist.bin --headless --frames 1`: failed as expected with `Unable to open ROM file: /does/not/exist.bin`

## Issues Discovered
- The active packet filename is `tasks/active/m29h_sdk_integration_validation.md`, but its contents define `M29h - ROM Execution Contract`. The implementation followed the packet contents and `docs/plan.md`.

## Next-Step Recommendation
Proceed with `M29i - Minimal SDK Runtime Surface` so the repository contains the real SDK/runtime/resource surface needed to assemble a Showcase ROM against the new external ROM execution path.
