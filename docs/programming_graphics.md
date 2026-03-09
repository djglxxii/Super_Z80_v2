# Super_Z80 Graphics Programming Guide

## Overview
Graphics programming targets the frozen VDP contract in [docs/vdp_baseline_v1.md](/home/djglxxii/src/Super_Z80_v2/docs/vdp_baseline_v1.md). This guide focuses on how software should arrange VRAM and ports for the implemented renderer.

## Tile Format
- One tile is `8 x 8` pixels.
- One tile consumes `64` bytes.
- Each byte is one palette index.
- Tile `N` begins at `bank_base + (N * 64)`.

Pattern banks:
- Bank `0`: `0x0000-0x1FFF`
- Bank `1`: `0x2000-0x3FFF`

## Tilemaps
- Background tilemap base: `0x4000`
- Foreground tilemap base: `0x5000`
- Tilemap size: `32 x 24`
- Each tilemap entry is one byte containing a tile index

The visible playfield is exactly one tilemap wide and high: `256 x 192`.

## VRAM Programming Model
- Port `0xE1` is only a stored control byte in the current implementation.
- VRAM reads and writes happen through port `0xE2`.
- Each `0xE2` access uses the current internal VRAM pointer, then auto-increments it.
- The internal VRAM pointer resets to `0x0000` and cannot currently be reloaded from software.

Practical implication:
- Sequential VRAM streaming from reset works.
- Random-access VRAM updates are not exposed as a stable CPU-visible feature yet, so ROM code should treat the current VDP data path as sequential from reset unless a future milestone adds explicit pointer-load semantics.

## Palette Usage
- Select an entry through `0xE3`.
- Write `R`, `G`, `B`, `A` bytes through `0xE4-0xE7`.
- Palette entry `0` is not globally transparent by itself. Transparency depends on layer rules below.

## Plane Behavior
### Background Plane
- Tilemap base: `0x4000`
- Scroll ports: `0xE8` and `0xE9`
- Pattern bank port: `0xEC`
- Pixel value `0` is visible like any other index

### Foreground Plane
- Tilemap base: `0x5000`
- Scroll ports: `0xEA` and `0xEB`
- Pattern bank port: `0xED`
- Pixel value `0` is transparent

### Scrolling
- Scroll registers are 8-bit.
- Sampling wraps around the full `256 x 192` tile plane.
- Scroll operates at pixel granularity, not tile granularity.

## Sprite Engine
The sprite attribute table starts at `0x6000`. Each sprite is four bytes:

```text
byte0 = Y
byte1 = X
byte2 = tile index
byte3 = flags
```

Flags:
- Bit `0`: priority
- Bit `1`: horizontal flip
- Bit `2`: vertical flip
- Bit `3`: sprite pattern bank
- Bits `4-7`: reserved

Rules:
- Sprites are `8 x 8`.
- There are `64` SAT entries.
- Only the first `16` visible sprites on a scanline render.
- If more than `16` are visible on one scanline, status bit `2` latches overflow.
- Later SAT entries overwrite earlier ones inside the same priority pass.

## Layering and Priority
Composition order is fixed:

```text
BG
sprites with flags bit0 = 1
FG
sprites with flags bit0 = 0
```

Interpretation:
- `bit0 = 1` means the sprite is behind the foreground plane.
- `bit0 = 0` means the sprite is in front of the foreground plane.

## Transparency Rules
- BG: no transparency rule for pixel `0`
- FG: pixel `0` is transparent
- Sprites: pixel `0` is transparent

This means FG can reveal BG and low-priority sprites, while high-priority sprites always draw after FG.

## Collision and Status
- Sprite collision is sprite-only.
- Collision latches when two non-zero sprite pixels overlap on the same output coordinate.
- BG and FG pixels never trigger sprite collision.
- Reading status port `0xE0` clears collision and overflow latches, and also clears `frame_ready`.

Status bits:
- Bit `0`: VBlank
- Bit `1`: frame ready
- Bit `2`: sprite overflow
- Bit `3`: sprite collision

## Recommended Patterns
- Keep tile data banked by usage so BG and FG bank switching is cheap.
- Use FG pixel `0` for HUD cutouts and text overlays.
- Reserve SAT order intentionally because later sprites win within a pass.
- Poll `0xE0` once per frame if software depends on frame-ready, overflow, or collision latches.
