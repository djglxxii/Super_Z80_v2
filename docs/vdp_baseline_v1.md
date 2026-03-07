# Super_Z80 VDP Baseline v1

## Status

This document defines the stable VDP graphics contract frozen at milestone M23.

VDP behavior is now considered stable. Future changes must not alter the behavior defined here unless a new hardware contract revision is created. Acceptable exceptions are bug fixes and ROM-driven requirements that preserve the declared contract.

## Overview

The Super_Z80 VDP provides a deterministic `256 x 192` indexed-pixel framebuffer backed by `64 KiB` of VRAM and a `256` entry RGBA palette.

The supported baseline feature set is:

- One background tile plane
- One foreground tile plane
- Independent BG and FG scrolling
- Two tile pattern banks
- A 64-sprite SAT-based sprite system
- Deterministic per-scanline sprite selection
- Sprite overflow and sprite collision status flags
- Palette-based pixel resolve from indexed framebuffer values to `0xRRGGBBAA`

This milestone does not add hardware features beyond the implemented M22 VDP.

## Memory Layout

VRAM regions:

- `0x0000-0x1FFF`: Pattern Bank 0
- `0x2000-0x3FFF`: Pattern Bank 1
- `0x4000`: Background tilemap base
- `0x5000`: Foreground tilemap base
- `0x6000`: Sprite Attribute Table base

Display geometry:

- Framebuffer: `256 x 192`
- Tile size: `8 x 8`
- Tile size in memory: `64` bytes
- Tilemap dimensions: `32 x 24` tiles
- Sprite size: `8 x 8`
- Sprite entry size: `4` bytes
- Sprite count: `64`
- Maximum sprites per scanline: `16`

Pattern fetch rule:

```text
pattern_address = bank_base + (tile_index * 64)
```

## Port Map

Core VDP ports:

- `0xE0`: status
- `0xE1`: control
- `0xE2`: data

Palette ports:

- `0xE3`: palette index
- `0xE4`: palette red
- `0xE5`: palette green
- `0xE6`: palette blue
- `0xE7`: palette alpha

Scroll ports:

- `0xE8`: BG scroll X
- `0xE9`: BG scroll Y
- `0xEA`: FG scroll X
- `0xEB`: FG scroll Y

Pattern bank ports:

- `0xEC`: BG pattern bank
- `0xED`: FG pattern bank

Bank registers mask to bit `0`. Valid bank values are `0` and `1`.

## Tile Planes

Background and foreground both render from `8 x 8` indexed tiles. Each tilemap entry is a one-byte tile index.

Plane behavior:

- BG reads tile indices from `0x4000`
- FG reads tile indices from `0x5000`
- BG scrolls through ports `0xE8-0xE9`
- FG scrolls through ports `0xEA-0xEB`
- World sampling wraps across the full `256 x 192` plane
- FG treats pixel value `0` as transparent
- BG writes pixel value `0` as an ordinary indexed pixel

Pattern bank behavior:

- BG pattern bank is selected through `0xEC`
- FG pattern bank is selected through `0xED`
- Bank `0` base is `0x0000`
- Bank `1` base is `0x2000`

## Sprite System

The sprite attribute table begins at `0x6000`. Each sprite entry is:

```text
byte0 = Y
byte1 = X
byte2 = tile index
byte3 = flags
```

Sprite flags:

- `bit0`: priority select
- `bit1`: horizontal flip
- `bit2`: vertical flip
- `bit3`: pattern bank select
- `bit4-7`: reserved

Priority mapping:

- `bit0 = 1`: sprite renders in the low-priority pass between BG and FG
- `bit0 = 0`: sprite renders in the high-priority pass after FG

Pattern bank mapping:

- `bit3 = 0`: sprite pattern bank `0` at `0x0000`
- `bit3 = 1`: sprite pattern bank `1` at `0x2000`

Sprite limits and selection:

- The VDP evaluates sprites per scanline
- Visible sprites are selected in SAT order
- Only the first `16` visible sprites on a scanline are rendered
- If more than `16` are visible, sprite overflow latches
- Within a render pass, later SAT entries overwrite earlier ones

## Render Pipeline

The final indexed-pixel composition order is:

```text
Background plane
Low-priority sprites
Foreground plane
High-priority sprites
Palette resolve
Framebuffer output
```

Transparency rules:

- Pixel value `0` is transparent for FG and sprites
- Pixel value `!= 0` is visible for FG and sprites
- FG transparency reveals BG and low-priority sprites behind it
- High-priority sprites render after FG and therefore appear in front of FG

## Collision Detection

Sprite collision is sprite-only.

Collision occurs when:

- Two sprites both render non-zero pixels
- The non-zero pixels land on the same screen coordinate

Collision behavior:

- BG and FG pixels never contribute to collision
- Collision detection is deterministic because sprite selection and rendering remain SAT-ordered within the two sprite passes
- Collision latches until the status register is read

## Status Register Semantics

Status port `0xE0` exposes:

- `bit0`: VBlank active
- `bit1`: frame ready
- `bit2`: sprite overflow
- `bit3`: sprite collision

Status clear behavior on read:

- Reading `0xE0` clears `frame_ready`
- Reading `0xE0` clears `sprite_overflow`
- Reading `0xE0` clears `sprite_collision`
- Reading `0xE0` does not clear `VBlank`

Timing semantics:

- `frame_ready` latches at scanline `191`
- `VBlank` becomes active at scanline `192`

## ROM Developer Contract

ROM software may target this VDP baseline as stable behavior.

Future emulator work must treat this document as the authoritative graphics reference unless and until a new documented VDP contract revision supersedes it.
