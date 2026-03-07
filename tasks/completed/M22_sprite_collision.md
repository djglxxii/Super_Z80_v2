# Task Packet

## Title

M22 — Sprite Collision Detection

## Branch

```
m22_sprite_collision
```

---

# Objective

Implement **hardware sprite collision detection** and expose the result through the VDP status register.

Capabilities added:

```
sprite-to-sprite collision detection
collision status flag
status flag clear-on-read behavior
unit tests for collision scenarios
```

No rendering changes are introduced.

---

# Scope

Included:

```
sprite pixel overlap detection
VDP collision status flag
deterministic detection order
unit tests
```

Excluded:

```
sprite-background collision
collision interrupts
new rendering rules
palette interactions
timing changes
```

Collision detection should remain **frame deterministic**.

---

# Collision Definition

A collision occurs when:

```
two visible sprites
render non-zero pixels
on the same screen coordinate
```

Transparency rule:

```
pixel value 0 → transparent
pixel value ≠0 → solid
```

Collision only counts when **two solid pixels overlap**.

---

# Status Register Update

Current VDP status bits:

```
bit0 = VBlank
bit1 = frame_ready
bit2 = sprite_overflow
```

Add:

```
bit3 = sprite_collision
```

Behavior:

```
set when collision occurs during frame render
cleared when status register is read
```

---

# Collision Evaluation

Perform collision detection **during sprite rendering**.

Pseudo logic:

```
for each rendered sprite pixel
    if framebuffer already contains sprite pixel
        set collision flag
```

Important constraint:

```
only sprite pixels count
BG/FG pixels do not trigger collision
```

Implementation hint:

Maintain a small **sprite mask buffer** during rendering.

Example concept:

```
sprite_mask[x][y] = true when sprite pixel rendered
```

If a sprite attempts to render where mask already true → collision.

---

# Files To Create

```
emulator/tests/unit/test_sprite_collision.cpp
artifacts/reports/M22_sprite_collision_report.md
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
emulator/tests/unit/test_sprite_collision.cpp
```

Required tests:

### Test A — no collision

```
place two sprites apart
render frame
verify collision flag = 0
```

---

### Test B — simple collision

```
two sprites overlap
render frame
verify collision flag = 1
```

---

### Test C — transparency prevents collision

```
overlap transparent pixels
verify collision flag = 0
```

---

### Test D — collision flag clears on read

```
trigger collision
read VDP status
verify bit clears afterward
```

---

### Test E — collision detection independent of priority

```
sprites with different priority flags
overlap pixels
verify collision still detected
```

---

# CMake Integration

Modify:

```
emulator/CMakeLists.txt
```

Add:

```
emulator/tests/unit/test_sprite_collision.cpp
```

Create test target:

```
super_z80_test_sprite_collision
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
Milestone: M22
Sprite collision detection implemented
Collision flag added to VDP status register
```

Next milestone:

```
M23 — Window / HUD Tile Plane
```

---

# Commit Message

```
M22: implement sprite collision detection and status flag
```

---

# Completion Report

Create:

```
artifacts/reports/M22_sprite_collision_report.md
```

Include:

```
files created
files modified
collision detection algorithm
status register semantics
build output
test results
commit hash
```

---

# Expected Result

After M22 the VDP will support:

```
sprite overflow detection
sprite collision detection
```

This completes the **core sprite hardware semantics** typical of classic consoles.

---
