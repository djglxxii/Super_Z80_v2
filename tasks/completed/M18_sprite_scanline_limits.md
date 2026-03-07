# Task Packet

## Title

M18 — Hardware Sprite Limits and Scanline Evaluation

## Branch

```
m18_sprite_scanline_limits
```

---

# Objective

Introduce a **scanline sprite evaluation stage** that limits the number of sprites rendered per scanline and performs deterministic sprite selection.

This milestone establishes:

```
sprite-per-scanline evaluation
sprite-per-scanline limits
deterministic sprite selection order
optional overflow flag
```

This is the final major step in making the sprite system behave like hardware.

---

# Scope

Included:

```
scanline sprite evaluation
sprite-per-scanline limit enforcement
sprite overflow flag
unit tests
```

Excluded:

```
sprite collision detection
sprite priority changes
palette banks
new interrupts
sprite flipping or scaling
```

Keep the implementation deterministic and minimal.

---

# Hardware Limits for Super_Z80

Use the limits defined in the specification:

```
Total sprites in SAT: 64
Sprites allowed per scanline: 16
```

Only the **first 16 visible sprites for the scanline** are rendered.

Sprites beyond this limit are ignored for that scanline.

---

# Evaluation Stage

Add a deterministic sprite evaluation step before rendering.

Pseudo process:

```
for each scanline
    evaluate SAT
    collect sprites that intersect scanline
    keep first 16
    set overflow flag if more exist
```

Intersection rule:

```
sprite_top = Y
sprite_bottom = Y + 7
scanline in [sprite_top, sprite_bottom]
```

---

# VDP State Additions

Add fields similar to:

```
sprite_scanline_list[16]
sprite_scanline_count
sprite_overflow_flag
```

Reset behavior:

```
overflow_flag = false
scanline_count = 0
```

---

# Status Register Update

Extend `VDP_STATUS`.

Example:

```
bit0 = VBlank
bit1 = frame_ready
bit2 = sprite_overflow
```

The overflow flag should clear on status read.

---

# Rendering Changes

Instead of scanning the entire SAT during rendering, perform sprite evaluation first.

Rendering should use the evaluated list.

Pseudo pipeline:

```
render_background()
render_sprites(priority=1)
render_foreground()
render_sprites(priority=0)
```

But the sprite passes should now iterate over the **scanline sprite list**.

This keeps behavior deterministic and closer to real hardware.

---

# Files To Create

```
emulator/tests/unit/test_sprite_scanline_limits.cpp
artifacts/reports/M18_sprite_scanline_limits_report.md
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
emulator/tests/unit/test_sprite_scanline_limits.cpp
```

Required tests:

### Test A — sprites under limit render normally

* place 5 sprites on same scanline
* render frame
* verify all 5 appear

---

### Test B — sprites beyond limit ignored

* place 20 sprites overlapping the same scanline
* render frame
* verify only first 16 appear

---

### Test C — overflow flag set

* create >16 sprites on scanline
* read status register
* verify overflow bit set

---

### Test D — overflow flag clears on read

* trigger overflow
* read status
* verify bit clears afterward

---

### Test E — sprite ordering preserved

* create overlapping sprites
* verify that the first 16 selected follow SAT order

---

# CMake Integration

Modify:

```
emulator/CMakeLists.txt
```

Add:

```
emulator/tests/unit/test_sprite_scanline_limits.cpp
```

Create target:

```
super_z80_test_sprite_scanline_limits
```

---

# Verification Commands

Run:

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

Record:

```
Milestone: M18
Scanline sprite evaluation implemented
16-sprite-per-scanline hardware limit enforced
Sprite overflow flag added
```

Next milestone:

```
M19 — Sprite flipping and attribute expansion
```

---

# Commit Message

```
M18: implement sprite scanline evaluation and hardware limits
```

---

# Completion Report

Create:

```
artifacts/reports/M18_sprite_scanline_limits_report.md
```

Include:

```
files created
files modified
scanline evaluation algorithm
sprite limit behavior
overflow flag semantics
build output
test results
commit hash
```

---

# Expected Result

After M18 the sprite system will behave like real hardware:

```
SAT → scanline evaluation → limited sprite list → rendering
```

This ensures the emulator supports **deterministic sprite limits**, which many real games rely on.

---
