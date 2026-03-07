# Task Packet

## Title

M23 — VDP Baseline Freeze and Graphics Contract Lock

## Branch

```text
m23_vdp_baseline_freeze
```

---

# Objective

Formally declare the **Super_Z80 VDP feature set stable** and freeze the graphics contract so ROM software can safely target the system without fear of changing behavior.

This milestone **does not add hardware features**.

Instead it:

```text
documents the completed VDP
locks the graphics behavior contract
records rendering rules
creates a clear baseline for ROM development
```

---

# Rationale

After M22 the VDP now supports the full set of core capabilities expected from a classic 8-bit console-class video processor.

Continuing to add speculative graphics features risks:

```text
moving hardware targets
breaking ROM assumptions
creating unnecessary complexity
```

Freezing the graphics contract ensures:

```text
stable emulator behavior
stable SDK expectations
stable ROM targets
```

Future VDP changes must now be justified by:

```text
bug fixes
ROM-driven requirements
explicit hardware contract revisions
```

---

# Baseline VDP Feature Set

Document the final supported features.

### Tile Planes

```text
Background tile plane
Foreground tile plane
Independent scroll registers
Tile size: 8x8
Tile format: 64 bytes per tile
Tilemap size: 256x192 address space
```

---

### Pattern Memory

```text
Pattern Bank 0 : 0x0000–0x1FFF
Pattern Bank 1 : 0x2000–0x3FFF
```

Tile addressing:

```text
pattern_address = bank_base + (tile_index * 64)
```

---

### Tile Pattern Banks

```text
0xEC = BG_PATTERN_BANK
0xED = FG_PATTERN_BANK
```

---

### Sprite System

```text
Sprite count: 64
Sprite size: 8x8
Sprite entry size: 4 bytes
SAT base: 0x6000
```

Sprite attributes:

```text
byte0 = Y
byte1 = X
byte2 = tile index
byte3 = flags
```

Flags:

```text
bit0 = priority
bit1 = horizontal flip
bit2 = vertical flip
bit3 = pattern bank select
bit4–7 = reserved
```

---

### Sprite Limits

```text
Maximum sprites per scanline: 16
Selection order: SAT order
Overflow flag set if exceeded
```

---

### Rendering Order

Final rendering pipeline:

```text
Background plane
↓
Low-priority sprites
↓
Foreground plane
↓
High-priority sprites
↓
Palette resolve
↓
Framebuffer
```

---

### Transparency Rules

```text
Pixel value 0 = transparent
Pixel value ≠0 = visible
```

Foreground transparency reveals background and sprites behind it.

---

### Collision Detection

Sprite collision occurs when:

```text
two sprites render non-zero pixels
at the same screen coordinate
```

Collision is **sprite-only**.

---

### Status Register

VDP status bits:

```text
bit0 = VBlank
bit1 = frame_ready
bit2 = sprite_overflow
bit3 = sprite_collision
```

Reading status clears:

```text
frame_ready
sprite_overflow
sprite_collision
```

---

# Contract Rule

Add a section to the documentation:

```text
VDP behavior is now considered stable.
Future changes must not alter the behavior defined in this milestone unless a new hardware contract revision is created.
```

---

# Files To Create

```text
docs/vdp_baseline_v1.md
artifacts/reports/M23_vdp_baseline_freeze_report.md
```

---

# Files To Modify

```text
docs/state_snapshot.md
```

Add entry:

```text
Milestone: M23
VDP baseline frozen
Graphics contract locked for ROM development
```

---

# Documentation Content

`docs/vdp_baseline_v1.md` should include:

```text
VDP overview
memory layout
tile planes
sprite system
render pipeline
status register semantics
pattern bank rules
sprite attribute format
```

This becomes the **authoritative graphics reference for ROM developers**.

---

# Verification

No emulator code changes are required.

Verify:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build
```

All existing tests must pass unchanged.

---

# Commit Message

```text
M23: freeze VDP baseline and lock graphics contract
```

---

# Completion Report

Create:

```text
artifacts/reports/M23_vdp_baseline_freeze_report.md
```

Include:

```text
baseline VDP features
documentation added
confirmation no code changes required
test suite results
commit hash
```

---

# Expected Result

After M23:

```text
VDP is declared stable
ROM developers have a fixed graphics contract
future graphics changes require explicit revision
```

---
