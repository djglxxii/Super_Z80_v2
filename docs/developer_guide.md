# Super_Z80 Developer Guide

## Read Order
1. [docs/hardware_reference.md](/home/djglxxii/src/Super_Z80_v2/docs/hardware_reference.md)
2. [docs/programming_graphics.md](/home/djglxxii/src/Super_Z80_v2/docs/programming_graphics.md)
3. [docs/programming_audio.md](/home/djglxxii/src/Super_Z80_v2/docs/programming_audio.md)
4. [docs/programming_input.md](/home/djglxxii/src/Super_Z80_v2/docs/programming_input.md)
5. [docs/platform_timing.md](/home/djglxxii/src/Super_Z80_v2/docs/platform_timing.md)

## Platform Summary
Super_Z80 is a Z80-based development target with:
- `32 KiB` fixed ROM at `0x0000-0x7FFF`
- `16 KiB` RAM at `0xC000-0xFFFF`
- a deterministic tile-and-sprite VDP
- a deterministic PSG-style mono APU
- active-low digital controller input on `0xDC-0xDD`

Current validated platform:
- PSG audio

Planned expansion:
- YM2151 FM audio in future milestones

## Recommended Toolchain
The repository does not yet ship a canonical SDK or assembler wrapper. For ROM work, use a Z80 assembler and linker that can emit a flat binary image no larger than `32 KiB`.

Practical requirements:
- output format must be raw binary
- ROM image must target load address `0x0000`
- interrupt handler code should populate `0x0038` if interrupts are enabled
- code and read-only data must fit within `0x0000-0x7FFF`
- writable state should live in RAM at `0xC000-0xFFFF`

## ROM Creation Workflow
1. Assemble and link a flat binary ROM.
2. Place startup code at `0x0000`.
3. If using interrupts, install a handler at `0x0038`.
4. Initialize RAM, VDP state, palette state, controller handling, and audio registers explicitly.
5. Run and validate the ROM through the available core or test harness entrypoints.

The repository does not yet define a stable end-user ROM-loading CLI, cartridge header, or packaging format.

## Emulator Usage
Core developer-facing modes currently exposed by the shell:
- `super_z80 --help`
- `super_z80 --version`
- `super_z80 --sdl-input`
- `super_z80 --sdl-audio`

Shell mode details:
- `--sdl-input` opens a window and maps keyboard or controller input to `PAD1`/`PAD1_SYS`.
- `--sdl-audio` plays a built-in Tone A demo through SDL using the same deterministic core audio path as headless tests.

These SDL modes are useful for bring-up and manual inspection, but they are not authoritative for timing.

## Development Pattern
- Keep frame logic deterministic and driven from emulated state.
- Treat VBlank as the primary synchronization point.
- Use the VDP status port for frame-ready and sprite status latches.
- Keep game state in RAM and stream assets into VRAM through the current sequential VDP data path.
- Program the audio block through `0xD0-0xDB`; there is no separate FM coprocessor.

## Deterministic Debugging Approach
- Reproduce issues with fixed ROM input and fixed controller sequences.
- Compare RAM, port-visible state, and output samples across repeated runs.
- Prefer scanline-count-based logging or assertions over wall-clock profiling.
- When testing IRQ paths, validate both `0xF0-0xF2` and the handler entry at `0x0038`.

## Current Limits
- No banked ROM model beyond the fixed `32 KiB` window
- No second controller port
- No YM2151/FM audio hardware
- No SDK utilities or example ROM library in this milestone

Those limits are intentional repository truth, not omitted documentation.
