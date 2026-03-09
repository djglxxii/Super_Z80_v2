# Showcase State Snapshot

## Current Milestone
M29i

## Status
Completed.

## SDK Runtime Surface
- `sdk/inc/` now contains real public include files for the base platform, VDP, and controller input contracts.
- `sdk/runtime/inc/runtime.inc` now injects a minimal reset path, stack setup, deterministic hardware init, and an IRQ acknowledge stub before jumping to `sdk_main`.
- `sdk/assets/` now contains deterministic checked-in font and splash assets using the platform's `8 x 8` / `64-byte` tile format.
- `rom/starter/src/main.asm` is the minimal starter ROM source proving that ROM code can assemble directly against the checked-in SDK surface.

## Result
M29i establishes the first real repository-truth SDK surface for ROM work. ROMs can now include the checked-in SDK headers and runtime, assemble into a flat binary, and execute deterministically through the external ROM contract added in M29h.

## Recommendation
Proceed to `M29j - SDK Integration Validation`. The next dependency is validating that the minimal SDK runtime and assets boot cleanly inside the first Showcase-oriented ROM integration step.
