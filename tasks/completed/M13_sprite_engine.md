# Task Packet

## Title

M13 — Sprite Engine Skeleton

## Branch

```
m13_sprite_engine
```

---

# Objective

Introduce sprite rendering on top of the existing tile renderer.

The sprite system will:

```
read sprite attributes from VRAM
fetch sprite patterns
draw sprites into framebuffer
```

Sprites will render **after the background**, overwriting background pixels.

---

# Scope

Included:

```
sprite attribute table
sprite pattern fetch
basic sprite drawing
framebuffer composition
unit tests
```

Excluded:

```
priority system
transparency
sprite limits
scrolling
palette
DMA sprite upload
```

---

# Temporary Sprite Format

### Sprite size

```
8 x 8
```

### Sprite attributes

Each sprite entry uses **4 bytes**:

```
byte0 = Y
byte1 = X
byte2 = tile index
byte3 = flags (unused for now)
```

---

# VRAM Layout Additions

```
sprite attribute table base: 0x6000
sprite patterns: reuse tile patterns at 0x0000
```

Number of sprites supported for this milestone:

```
64 sprites
```

Total SAT size:

```
64 * 4 = 256 bytes
```

---

# Rendering Behavior

During `VDP::render_frame()`:

1. Background tiles render first (existing behavior)
2. Sprite pass runs after background

Pseudo behavior:

```
for sprite in 0..63
    read sprite entry from SAT
    if Y >= 192 skip

    draw 8x8 sprite using tile index
```

---

# Files To Create

```
emulator/tests/unit/test_sprite_renderer.cpp
artifacts/reports/M13_sprite_engine_report.md
```

---

# Files To Modify

```
emulator/include/vdp.h
emulator/src/vdp.cpp
emulator/CMakeLists.txt
docs/state_snapshot.md
```

---

# Sprite Rendering Rules (for M13)

Simplified behavior:

```
sprite pixels overwrite framebuffer
pixel value 0 is treated as transparent
```

This single rule allows basic composition.

---

# VDP Changes

Add helper function:

```
render_sprites()
```

Called after tile rendering.

Pseudo logic:

```
sat_base = 0x6000

for i in 0..63
    y = vram[sat_base + i*4]
    x = vram[sat_base + i*4 + 1]
    tile = vram[sat_base + i*4 + 2]

    draw 8x8 sprite
```

Pattern address:

```
tile * 64
```

---

# Unit Tests

Create:

```
emulator/tests/unit/test_sprite_renderer.cpp
```

Required tests:

### Test A — single sprite render

* write sprite pattern
* place sprite at `(0,0)`
* call `render_frame()`
* verify framebuffer contains sprite pixels

---

### Test B — transparency behavior

* sprite pixel value `0` should not overwrite background

---

### Test C — sprite position

* place sprite at `(16,16)`
* verify correct framebuffer region updated

---

### Test D — multiple sprites

* define two sprites
* verify both render

---

# CMake Integration

Modify:

```
emulator/CMakeLists.txt
```

Add:

```
emulator/tests/unit/test_sprite_renderer.cpp
```

Create target:

```
super_z80_test_sprite_renderer
```

---

# Verification Commands

```
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

All tests must pass.

---

# State Snapshot Update

Update:

```
docs/state_snapshot.md
```

Add:

```
Milestone: M13
Sprite engine skeleton implemented
Background + sprite composition working
```

Next milestone:

```
M14 — Palette + Color System
```

---

# Commit Message

```
M13: introduce sprite engine and framebuffer composition
```

---

# Completion Report

Create:

```
artifacts/reports/M13_sprite_engine_report.md
```

Include:

```
files created
files modified
sprite format
SAT layout
rendering rules
build output
test results
commit hash
```

---

# Expected Result

After M13 the emulator will support:

```
background tiles
+
sprites
=
complete 2D scene composition
```

At that point the graphics system becomes capable of rendering **real game scenes**.

---
