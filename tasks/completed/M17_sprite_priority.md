# Task Packet

## Title

M17 — Sprite Priority and Layer Ordering

## Branch

```
m17_sprite_priority
```

---

# Objective

Extend the sprite engine to support **priority ordering relative to the foreground plane** and add **sprite ordering within the sprite list**.

This milestone establishes:

```
sprite priority flag
foreground-vs-sprite ordering rules
stable sprite draw ordering
unit tests validating composition behavior
```

No hardware sprite limits or scanline limits yet.

---

# Scope

Included:

```
sprite priority flag
foreground priority interaction
stable sprite ordering
unit tests
```

Excluded:

```
sprite-per-scanline limits
sprite collision detection
sprite palette banks
hardware clipping rules
```

Keep the implementation minimal and deterministic.

---

# Sprite Attribute Extension

Current sprite entry format:

```
byte0 = Y
byte1 = X
byte2 = tile index
byte3 = flags
```

Interpret the **flags byte**:

```
bit0 = sprite priority
bit1–7 reserved
```

Priority meaning:

```
0 = sprite above FG
1 = sprite behind FG
```

Background always remains the lowest layer.

---

# Layer Model After M17

Rendering order becomes:

```
Background
→ Sprites with priority=1 (behind FG)
→ Foreground
→ Sprites with priority=0 (in front)
```

This produces the following visible stack:

```
BG
sprites (low priority)
FG
sprites (high priority)
```

This allows foreground tiles to occlude sprites.

---

# Rendering Changes

Modify the sprite renderer so that it runs **two passes**.

Pseudo structure:

```
render_background()
render_sprites(priority = 1)
render_foreground()
render_sprites(priority = 0)
```

Where the sprite renderer filters entries based on the priority flag.

Sprite ordering should remain **stable based on SAT order**.

Meaning:

```
lower index sprite draws first
higher index sprite draws later
```

Later sprites overwrite earlier ones.

---

# Files To Create

```
emulator/tests/unit/test_sprite_priority.cpp
artifacts/reports/M17_sprite_priority_report.md
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

# Rendering Rules

When drawing a sprite pixel:

```
pixel == 0 → transparent
pixel != 0 → overwrite framebuffer
```

The priority system controls **which pass the sprite is drawn in**, not how pixels are blended.

---

# Unit Tests

Create:

```
emulator/tests/unit/test_sprite_priority.cpp
```

Required tests:

### Test A — high priority sprite appears above FG

* BG tile visible
* FG tile visible
* sprite with `priority=0`
* verify sprite pixel overwrites FG

---

### Test B — low priority sprite appears behind FG

* BG tile visible
* FG tile visible
* sprite with `priority=1`
* verify FG remains visible over sprite

---

### Test C — sprite still appears above BG when behind FG

* FG transparent
* sprite priority=1
* verify sprite visible over BG

---

### Test D — sprite ordering stable

* two sprites overlap
* later SAT entry should overwrite earlier one

---

### Test E — mixed priority sprites

* one low-priority sprite
* one high-priority sprite
* verify ordering relative to FG and each other

---

# CMake Integration

Modify:

```
emulator/CMakeLists.txt
```

Add:

```
emulator/tests/unit/test_sprite_priority.cpp
```

Create test target:

```
super_z80_test_sprite_priority
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

Add:

```
Milestone: M17
Sprite priority implemented
Sprites can render above or behind foreground plane
Stable sprite ordering established
```

Next milestone:

```
M18 — Hardware Sprite Limits and Scanline Sprite Evaluation
```

---

# Commit Message

```
M17: implement sprite priority and layer ordering
```

---

# Completion Report

Create:

```
artifacts/reports/M17_sprite_priority_report.md
```

Include:

```
files created
files modified
priority flag semantics
rendering order rules
build output
test results
commit hash
```

---

# Expected Result

After M17 the VDP will support:

```
BG
+ FG
+ sprite priority
= fully layered 2D scene
```

This is the final major **layer composition feature** before implementing hardware sprite evaluation rules.

---
