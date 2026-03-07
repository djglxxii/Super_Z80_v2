# Task Packet

## Title

M12 — Tile Renderer (Background Plane Only)

## Branch

```text
m12_tile_renderer
```

## Objective

Implement the first real rendering path for the VDP by drawing a single background tile plane from VRAM into the internal framebuffer.

This milestone establishes:

* tile pattern interpretation
* tilemap interpretation
* framebuffer pixel generation
* deterministic background rendering
* unit tests for rendered output

## Scope

Included:

* one background tile plane
* tile pattern fetch from VRAM
* tilemap lookup from VRAM
* framebuffer generation
* unit tests

Excluded:

* sprites
* scrolling
* multiple planes
* palette RAM
* DMA-to-VRAM coupling changes
* SDL presentation changes

## Assumptions for This Milestone

Use a deliberately simple temporary format:

### Screen

* resolution: `256 x 192`
* tiles: `8 x 8`
* tile grid: `32 x 24`

### Tile format

* each tile is `8x8`
* one byte per pixel
* tile size = `64 bytes`
* tile value written directly into framebuffer as an 8-bit pixel index

### Tilemap format

* one byte per tile entry
* value = tile index
* no attributes yet

### VRAM layout for now

* tile patterns base: `0x0000`
* tilemap base: `0x4000`

These are temporary/simple bring-up rules for M12 only.

## Files To Create

```text
emulator/tests/unit/test_tile_renderer.cpp
artifacts/reports/M12_tile_renderer_report.md
```

## Files To Modify

```text
emulator/include/vdp.h
emulator/src/vdp.cpp
emulator/CMakeLists.txt
docs/state_snapshot.md
```

## VDP Rendering Requirements

Add a render entry point to the VDP, for example:

```cpp
void render_frame();
```

Behavior:

* iterate tilemap entries for `32 x 24`
* fetch tile index from tilemap at `0x4000 + tile_y * 32 + tile_x`
* fetch tile pattern from `0x0000 + tile_index * 64`
* write the 8x8 tile pixels into framebuffer
* framebuffer is `256 x 192`

No transparency rules needed yet.

## Unit Tests

Create:

```text
emulator/tests/unit/test_tile_renderer.cpp
```

Required tests:

### Test A — single tile render

* write one tile pattern into VRAM
* place tile index `0` in top-left tilemap entry
* call `render_frame()`
* verify framebuffer pixels `[0..7, 0..7]` match expected tile bytes

### Test B — tilemap placement

* define two different tiles
* place tile 0 at `(0,0)` and tile 1 at `(1,0)`
* render frame
* verify left tile and adjacent right tile occupy correct framebuffer positions

### Test C — untouched tiles default to zero

* after reset and render, framebuffer should remain zeroed if tilemap and tiles are zero

### Test D — full-tile stride correctness

* verify tile row addressing is correct across multiple rows of the same tile

## CMake Integration

Modify:

```text
emulator/CMakeLists.txt
```

Add:

```text
emulator/tests/unit/test_tile_renderer.cpp
```

Create target:

```text
super_z80_test_tile_renderer
```

## Verification Commands

Run:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

All tests must pass.

## State Snapshot Update

Update:

```text
docs/state_snapshot.md
```

Record:

```text
Milestone: M12
Background tile renderer implemented
Framebuffer now populated from VRAM tile/tilemap data
```

Next milestone:

```text
M13 — Sprite Engine Skeleton
```

## Commit Message

```text
M12: implement background tile renderer
```

## Completion Report

Create:

```text
artifacts/reports/M12_tile_renderer_report.md
```

Include:

* files created
* files modified
* temporary tile/tilemap format used
* rendering behavior summary
* build output
* test results
* commit hash

## Expected Result

After M12, the emulator will produce the first deterministic framebuffer content derived from VRAM.

That is the point where the emulator stops being only a motherboard skeleton and starts being a real graphics system.
