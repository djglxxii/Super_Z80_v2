# Showcase State Snapshot

## Current Milestone
M29j

## Status
Completed.

## SDK Runtime Surface
- `sdk/inc/` now contains real public include files for the base platform, VDP, and controller input contracts.
- `sdk/runtime/inc/runtime.inc` now injects a minimal reset path, stack setup, deterministic hardware init, and an IRQ acknowledge stub before jumping to `sdk_main`.
- `sdk/assets/` now contains deterministic checked-in font and splash assets using the platform's `8 x 8` / `64-byte` tile format.
- `rom/starter/src/main.asm` is the minimal starter ROM source proving that ROM code can assemble directly against the checked-in SDK surface.

## SDK Integration Validation
- `rom/showcase/src/main.asm` now builds a minimal Showcase-oriented ROM against `sdk/inc/super_z80.inc` and `sdk/runtime/inc/runtime.inc`.
- The ROM clears a background tilemap buffer, streams the checked-in font and splash tiles into VRAM through the current sequential VDP data path, and prints `SUPER Z80 SDK` / `BOOT VALIDATED`.
- `rom/showcase/Makefile` now provides a repository-local build entry for the validation ROM that emits `build/showcase_validation.bin`.

## Result
M29j proves that the checked-in SDK runtime is sufficient for real ROM integration work. A minimal Showcase ROM now assembles against the repository SDK, boots through the runtime reset/init flow, streams deterministic graphics data, and executes through the emulator's external-ROM headless path.

## Recommendation
Proceed to `M30 - Showcase ROM Project Scaffold`. The next step is expanding `rom/showcase/` into the canonical runnable project skeleton for later text, splash, and subsystem demonstration milestones.
