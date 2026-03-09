# Showcase State Snapshot

## Current Milestone
M29h

## Status
Completed.

## Execution Contract
- `super_z80` now accepts `--rom <path>` and defaults that path to deterministic headless execution when no SDL shell flag is selected.
- Headless ROM verification runs through the existing `Bus::load_rom(...)` boot path and executes for an explicit frame count via `--frames <count>`.
- The headless command prints a deterministic `HEADLESS_ROM_RESULT` summary containing ROM/RAM/audio CRC32 values plus frame, scanline, and audio sample counts.
- Missing or unreadable ROM paths now fail with a deterministic error message.

## Result
M29h establishes the external ROM execution prerequisite needed for Showcase ROM work. External ROM bytes can now be loaded from disk and executed through the normal emulator runtime without SDL input or audio requirements.

## Recommendation
Proceed to `M29i - Minimal SDK Runtime Surface`. The next dependency is providing the real SDK include/runtime/resource surface that can produce a ROM artifact for the new emulator execution contract.
