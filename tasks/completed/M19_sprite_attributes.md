# Task Packet

## Title

M19 — Sprite Flipping and Attribute Expansion

## Branch

```
m19_sprite_attributes
```

---

# Objective

Extend the sprite attribute flags to support **horizontal and vertical flipping** and reserve additional attribute bits for future expansion.

This milestone establishes:

```
horizontal sprite flipping
vertical sprite flipping
extended attribute flag decoding
unit tests verifying flipped rendering
```

No new rendering layers or palette rules are introduced.

---

# Scope

Included:

```
sprite horizontal flip
sprite vertical flip
attribute bit decoding
unit tests
```

Excluded:

```
sprite scaling
sprite rotation
per-sprite palette selection
sprite collision detection
new interrupts
```

---

# Sprite Attribute Layout

Sprite entry remains:

```
byte0 = Y
byte1 = X
byte2 = tile index
byte3 = flags
```

Interpret `flags` as:

```
bit0 = priority
bit1 = horizontal flip
bit2 = vertical flip
bit3–7 = reserved
```

Existing priority behavior from M17 remains unchanged.

---

# Rendering Behavior

When fetching sprite pixel `(px, py)`:

Normal lookup:

```
tile_pixel = pattern[py][px]
```

With flipping:

Horizontal flip:

```
px = 7 - px
```

Vertical flip:

```
py = 7 - py
```

Apply flipping before sampling the tile pattern.

Transparency rules remain unchanged:

```
pixel == 0 → transparent
pixel != 0 → overwrite framebuffer
```

---

# Files To Create

```
emulator/tests/unit/test_sprite_flipping.cpp
artifacts/reports/M19_sprite_attributes_report.md
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

# Unit Tests

Create:

```
emulator/tests/unit/test_sprite_flipping.cpp
```

Required tests:

### Test A — horizontal flip

* define sprite tile with distinguishable left/right pixels
* set horizontal flip flag
* verify sprite appears mirrored horizontally

---

### Test B — vertical flip

* define sprite tile with distinguishable top/bottom pixels
* set vertical flip flag
* verify sprite appears mirrored vertically

---

### Test C — both flips

* set both flip flags
* verify sprite appears rotated 180°

---

### Test D — flip does not affect background

* ensure flipping only affects sprite pixels

---

### Test E — sprite priority still respected

* verify flipped sprite still respects priority rules from M17

---

# CMake Integration

Modify:

```
emulator/CMakeLists.txt
```

Add:

```
emulator/tests/unit/test_sprite_flipping.cpp
```

Create target:

```
super_z80_test_sprite_flipping
```

---

# Verification Commands

Run:

```bash
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
Milestone: M19
Sprite attribute expansion implemented
Horizontal and vertical flipping supported
```

Next milestone:

```
M20 — Sprite Pattern Banks and Extended Tile Addressing
```

---

# Commit Message

```
M19: implement sprite flipping and attribute decoding
```

---

# Completion Report

Create:

```
artifacts/reports/M19_sprite_attributes_report.md
```

Include:

```
files created
files modified
sprite flag layout
flipping behavior
build output
test results
commit hash
```

---

# Expected Result

After M19 the sprite system will support:

```
position
priority
horizontal flip
vertical flip
```

This brings the sprite attribute model closer to what real hardware exposes.

---
