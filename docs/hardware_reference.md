# Super_Z80 Hardware Reference

## Scope
This document is the canonical platform overview for ROM developers. It describes CPU-visible platform behavior only. Internal emulator structure is mentioned only when it affects deterministic software behavior.

## CPU
- CPU core: Z80 compatible CPU implemented through Z80ex.
- Reset entry point: `PC=0x0000`.
- Interrupt model: maskable `INT` line driven from the IRQ controller.
- Current interrupt vector behavior: the CPU wrapper supplies `0xFF` during interrupt acknowledge, so software should treat the platform as using the Z80 `RST 38h` path and place the IRQ handler at `0x0038`.
- Scheduler cadence: the emulator executes `10` CPU instruction steps per scanline.

## CPU Memory Map
| Address Range | Size | Behavior |
| --- | --- | --- |
| `0x0000-0x7FFF` | `32 KiB` | ROM window |
| `0x8000-0xBFFF` | `16 KiB` | Unmapped, reads as open bus `0xFF`, writes ignored |
| `0xC000-0xFFFF` | `16 KiB` | RAM |

Notes:
- ROM loads are capped at `0x8000` bytes.
- The RAM window is direct CPU work RAM and is the destination for most software state.

## I/O Port Summary
| Ports | Function |
| --- | --- |
| `0x70-0x71` | YM2151 FM expansion |
| `0xD0-0xDB` | Audio processing unit |
| `0xDC` | `PAD1` controller port |
| `0xDD` | `PAD1_SYS` controller system port |
| `0xE0-0xED` | VDP status, VRAM, palette, scroll, and pattern-bank ports |
| `0xF0` | IRQ status |
| `0xF1` | IRQ enable mask |
| `0xF2` | IRQ acknowledge |
| `0xF4-0xF9` | DMA registers |

YM2151 FM expansion is implemented on ports `0x70-0x71`.

## VDP Overview
- VRAM size: `64 KiB`.
- Output size: `256 x 192` indexed pixels.
- Palette size: `256` entries.
- Tile size: `8 x 8`, `64` bytes per tile.
- Tilemap size: `32 x 24`.
- Sprite size: `8 x 8`.
- Sprite count: `64`.
- Visible sprite limit: `16` sprites per scanline.

### VRAM Layout
| VRAM Range | Purpose |
| --- | --- |
| `0x0000-0x1FFF` | Pattern bank 0 |
| `0x2000-0x3FFF` | Pattern bank 1 |
| `0x4000-0x42FF` | Background tilemap |
| `0x5000-0x52FF` | Foreground tilemap |
| `0x6000-0x60FF` | Sprite attribute table |

Unused VRAM remains readable and writable through the VDP data port and is not given special CPU-visible meaning.

Current limitation:
- The internal VRAM pointer resets to `0x0000` and auto-increments on each `0xE2` access.
- Port `0xE1` does not currently program that pointer.

### VDP Port Map
| Port | Name | Notes |
| --- | --- | --- |
| `0xE0` | Status | Read clears `frame_ready`, `sprite_overflow`, `sprite_collision` |
| `0xE1` | Control | Read/write storage byte only in the current implementation |
| `0xE2` | Data | Read/write VRAM at the current internal VRAM pointer, then post-increment |
| `0xE3` | Palette index | Selects one of `256` palette entries |
| `0xE4` | Palette red | `0xRRGGBBAA` red byte |
| `0xE5` | Palette green | `0xRRGGBBAA` green byte |
| `0xE6` | Palette blue | `0xRRGGBBAA` blue byte |
| `0xE7` | Palette alpha | `0xRRGGBBAA` alpha byte |
| `0xE8` | BG scroll X | 8-bit wraparound scroll |
| `0xE9` | BG scroll Y | 8-bit wraparound scroll |
| `0xEA` | FG scroll X | 8-bit wraparound scroll |
| `0xEB` | FG scroll Y | 8-bit wraparound scroll |
| `0xEC` | BG pattern bank | Bit `0` only |
| `0xED` | FG pattern bank | Bit `0` only |

### Palette Format
- Each entry is stored as `0xRRGGBBAA`.
- Reset value for all palette entries is `0x00000000`.
- Indexed framebuffer pixels resolve directly through the selected palette entry.

## Controller Ports
`PAD1` (`0xDC`) is active-low:
- Bit `0`: Up
- Bit `1`: Down
- Bit `2`: Left
- Bit `3`: Right
- Bit `4`: A
- Bit `5`: B
- Bits `6-7`: always high in the current implementation

`PAD1_SYS` (`0xDD`) is active-low:
- Bit `0`: Start
- Bits `1-7`: always high in the current implementation

Idle state:
- `PAD1 = 0xFF`
- `PAD1_SYS = 0xFF`

Direct CPU writes to `0xDC` and `0xDD` are ignored.

## Audio Overview
- Implemented hardware: deterministic PSG-style APU on ports `0xD0-0xDB`.
- FM expansion: YM2151 register/status interface on `0x70-0x71`.
- Channels: `Tone A`, `Tone B`, `Tone C`, and `Noise`.
- Output: mono signed `16-bit` PCM at `48000 Hz`.
- Tone channels use 12-bit periods and 50% duty square waves.
- Noise uses a deterministic 15-bit LFSR.
- Volume registers are 4-bit attenuation values mapped through the fixed `15..0` lookup table from [docs/audio_spec.md](/home/djglxxii/src/Super_Z80_v2/docs/audio_spec.md).
- YM2151 FM expansion is present and mixed with the PSG output in the current hardware model.

## IRQ and VBlank
- IRQ status port: `0xF0`.
- IRQ enable port: `0xF1`.
- IRQ acknowledge port: `0xF2`.
- VBlank requests IRQ bit `0x01`.
- VBlank becomes active at scanline `192`.
- VBlank clears when the scheduler wraps back to scanline `0`.

## DMA
| Port | Meaning |
| --- | --- |
| `0xF4` | Source low byte |
| `0xF5` | Source high byte |
| `0xF6` | Destination low byte |
| `0xF7` | Destination high byte |
| `0xF8` | Length in bytes |
| `0xF9` | Control / busy |

DMA behavior:
- Writing bit `0` to `0xF9` starts a transfer if length is non-zero.
- Readback from `0xF9` exposes busy as bit `7`.
- DMA copies exactly one byte per scanline.
- DMA can target RAM or any other CPU-visible bus address.

## Determinism Contract
- CPU time, VBlank, DMA, and audio all advance from scanline stepping.
- Open-bus reads are fixed to `0xFF`.
- Host wall-clock time is non-authoritative.
- SDL input and SDL audio are optional shell layers, not part of the hardware contract.
