# Task Packet

## Title

M15 — Scrolling and Viewport Movement

## Branch

```text
m15_scrolling
```

## Objective

Implement the first scrolling system for the background tile plane so the VDP can render a viewport into the tilemap rather than always rendering from tilemap origin `(0,0)`.

This milestone establishes:

* background scroll registers
* viewport offset during tile fetch
* wrapped tilemap addressing
* deterministic framebuffer movement as scroll values change
* unit tests for scroll behavior

## Scope

Included:

* background X scroll
* background Y scroll
* scroll register access through VDP ports
* wrapped tilemap lookup
* framebuffer render changes
* unit tests

Excluded:

* sprite scrolling rules
* foreground plane
* parallax
* per-line scroll
* hardware-accurate timing of register latching
* DMA changes
* windowing
* camera abstractions

Keep this milestone limited to **single background plane scrolling**.

## Scrolling Model for M15

Use a simple bring-up model:

* tilemap remains `32 x 24`
* tile size remains `8 x 8`
* screen remains `256 x 192`

Add two scroll registers:

```text
bg_scroll_x
bg_scroll_y
```

Treat both as pixel offsets.

Rendering rule:

```text
screen pixel (sx, sy)
  maps to
world pixel (sx + bg_scroll_x, sy + bg_scroll_y)
```

Then resolve:

* tile coordinates from world pixel
* intra-tile pixel coordinates from world pixel
* tilemap addressing wraps across the tilemap dimensions

## Files To Create

```text
emulator/tests/unit/test_scrolling.cpp
artifacts/reports/M15_scrolling_report.md
```

## Files To Modify

```text
emulator/include/vdp.h
emulator/src/vdp.cpp
emulator/include/bus.h
emulator/src/bus.cpp
emulator/CMakeLists.txt
docs/state_snapshot.md
```

## VDP Port Additions

Add simple background scroll ports:

```text
0xE8  BG_SCROLL_X
0xE9  BG_SCROLL_Y
```

Behavior:

* writes set the current scroll offsets
* reads return the stored values
* reset clears both to `0`

For this milestone, 8-bit scroll values are sufficient.

## Rendering Changes

Modify background rendering so that each framebuffer pixel is sampled from a scrolled world position.

Pseudo rule:

```text
world_x = (screen_x + bg_scroll_x) mod 256
world_y = (screen_y + bg_scroll_y) mod 192
```

Then:

```text
tile_x = world_x / 8
tile_y = world_y / 8
pixel_x = world_x % 8
pixel_y = world_y % 8
```

Tilemap lookup remains based at:

```text
0x4000
```

Tile pattern lookup remains based at:

```text
0x0000
```

This gives deterministic wrapped scrolling over the current tilemap.

## Unit Tests

Create:

```text
emulator/tests/unit/test_scrolling.cpp
```

Required tests:

### Test A — zero scroll preserves existing rendering

* with `bg_scroll_x = 0`, `bg_scroll_y = 0`
* render frame
* verify output matches expected unscrolled tile placement

### Test B — horizontal scroll shifts viewport

* define two adjacent tiles with different pixel values
* set `bg_scroll_x = 8`
* verify the tile that was previously at `(1,0)` now appears at screen origin

### Test C — vertical scroll shifts viewport

* define two vertically adjacent tiles
* set `bg_scroll_y = 8`
* verify the tile that was previously at `(0,1)` now appears at screen origin

### Test D — partial pixel scroll

* create a tile with distinguishable columns/rows
* set `bg_scroll_x = 1` and/or `bg_scroll_y = 1`
* verify intra-tile pixel fetch shifts correctly

### Test E — wraparound behavior

* set scroll near edge, e.g. `bg_scroll_x = 255`
* verify rendering wraps correctly instead of reading out of bounds

### Test F — register read/write

* write `BG_SCROLL_X` and `BG_SCROLL_Y`
* read back through VDP/bus
* verify values persist

## CMake Integration

Modify:

```text
emulator/CMakeLists.txt
```

Add:

```text
emulator/tests/unit/test_scrolling.cpp
```

Create target:

```text
super_z80_test_scrolling
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
Milestone: M15
Background scrolling implemented
Viewport now moves through tilemap using scroll registers
```

Next milestone:

```text
M16 — Foreground Plane / Second Background Layer
```

## Commit Message

```text
M15: implement background scrolling and viewport movement
```

## Completion Report

Create:

```text
artifacts/reports/M15_scrolling_report.md
```

Include:

* files created
* files modified
* scroll register map
* scrolling model used
* wraparound behavior
* build output
* test results
* commit hash

## Expected Result

After M15, the emulator will support:

```text
tilemap
  + scroll registers
  = moving background viewport
```
