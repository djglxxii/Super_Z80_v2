# Task Packet

## Title

M16 — Foreground Plane / Second Tile Layer

## Branch

```text
m16_foreground_plane
```

## Objective

Extend the VDP from a single scrolling background plane to a **two-plane tile renderer** by adding a foreground plane rendered after the background and before sprites.

This milestone establishes:

* second tilemap plane
* separate foreground scroll registers
* deterministic plane composition
* independent foreground/background movement
* unit tests for layer ordering and scrolling behavior

## Scope

Included:

* foreground tilemap plane
* foreground X/Y scroll registers
* foreground rendering pass
* simple plane composition rules
* unit tests

Excluded:

* per-tile priority bits
* transparency beyond a simple rule
* palette-bank selection per plane
* windowing
* parallax abstractions beyond scroll values
* sprite priority interaction beyond existing “sprites after tiles” behavior

Keep this milestone limited to **two tile planes with simple ordering**.

---

## Plane Model for M16

Retain the current background plane exactly as implemented.

Add a second plane:

```text
Background plane (BG)
Foreground plane (FG)
```

Render order for this milestone:

```text
BG
→ FG
→ Sprites
```

### Temporary FG rules

* Foreground tilemap uses the same 8x8 / 1-byte-per-pixel tile format
* Foreground tilemap entries are still 1 byte = tile index
* Foreground pixel value `0` is transparent
* Non-zero foreground pixels overwrite background pixels
* Sprite behavior remains unchanged from M13

---

## VRAM Layout Additions

Keep existing layout:

```text
Tile patterns base:    0x0000
BG tilemap base:       0x4000
SAT base:              0x6000
```

Add:

```text
FG tilemap base:       0x5000
```

This gives both planes separate tilemap regions while sharing the same tile pattern memory.

---

## Files To Create

```text
emulator/tests/unit/test_foreground_plane.cpp
artifacts/reports/M16_foreground_plane_report.md
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

---

## VDP Port Additions

Add foreground scroll registers:

```text
0xEA  FG_SCROLL_X
0xEB  FG_SCROLL_Y
```

Behavior:

* 8-bit registers
* reads return stored value
* writes update stored value
* reset clears them to `0`

Retain BG scroll ports as-is:

```text
0xE8  BG_SCROLL_X
0xE9  BG_SCROLL_Y
```

Bus VDP routing should now forward:

```text
0xE0–0xEB
```

---

## Rendering Changes

Add a dedicated foreground render pass, for example:

```cpp
void render_foreground();
```

`render_frame()` should become:

```text
clear framebuffer
render background
render foreground
render sprites
```

### Background plane

Unchanged behavior from M15.

### Foreground plane

Use same world-pixel mapping logic as BG, but with FG registers:

```text
world_x = (screen_x + fg_scroll_x) mod 256
world_y = (screen_y + fg_scroll_y) mod 192
```

Then resolve:

* `tile_x = world_x / 8`
* `tile_y = world_y / 8`
* `pixel_x = world_x % 8`
* `pixel_y = world_y % 8`

Tilemap lookup base:

```text
0x5000
```

Pattern lookup remains:

```text
0x0000 + tile_index * 64
```

Foreground pixel rule:

```text
if fg_pixel != 0:
    overwrite framebuffer pixel
else:
    leave background pixel intact
```

---

## Unit Tests

Create:

```text
emulator/tests/unit/test_foreground_plane.cpp
```

Required tests:

### Test A — foreground overwrites background

* define one BG tile with non-zero values
* define one FG tile with different non-zero values
* place both at the same tilemap position
* render frame
* verify final framebuffer shows FG values

### Test B — foreground zero is transparent

* BG tile has visible data
* FG tile has zero-valued pixels
* render frame
* verify BG remains visible

### Test C — foreground tilemap placement

* place a non-zero FG tile at `(1,0)`
* verify it appears in the correct framebuffer region

### Test D — foreground horizontal scroll

* place two different FG tiles adjacent
* set `FG_SCROLL_X = 8`
* verify the second tile appears at screen origin

### Test E — independent BG and FG scroll

* set BG scroll to one value, FG scroll to another
* verify each plane samples from its own viewport

### Test F — FG register read/write through bus

* write `FG_SCROLL_X` / `FG_SCROLL_Y`
* read them back
* verify values persist and reset correctly

---

## CMake Integration

Modify:

```text
emulator/CMakeLists.txt
```

Add:

```text
emulator/tests/unit/test_foreground_plane.cpp
```

Create target:

```text
super_z80_test_foreground_plane
```

---

## Verification Commands

Run:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

All tests must pass.

---

## State Snapshot Update

Update:

```text
docs/state_snapshot.md
```

Record:

```text
Milestone: M16
Foreground plane implemented
Two-plane tile composition working
Independent BG/FG scroll registers active
```

Next milestone:

```text
M17 — Sprite Priority / Layer Ordering Refinement
```

---

## Commit Message

```text
M16: implement foreground plane and second tile layer
```

---

## Completion Report

Create:

```text
artifacts/reports/M16_foreground_plane_report.md
```

Include:

* files created
* files modified
* FG tilemap base and register map
* composition rules
* scrolling behavior
* build output
* test results
* commit hash

---

## Expected Result

After M16, the VDP will support:

```text
background plane
+ foreground plane
+ sprites
= layered 2D scene composition
```
