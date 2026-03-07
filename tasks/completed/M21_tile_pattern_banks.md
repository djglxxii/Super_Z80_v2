# Task Packet

## Title

M21 — Tile Plane Pattern Banks

## Branch

```
m21_tile_pattern_banks
```

---

# Objective

Extend tile rendering so the **background and foreground planes can reference multiple pattern banks**, similar to the sprite system.

This increases available tile graphics without altering rendering order or timing.

Capabilities added:

```
BG pattern bank selection
FG pattern bank selection
extended tile addressing
bank-aware tile rendering
```

No new rendering layers or sprite semantics are introduced.

---

# Scope

Included:

```
background pattern bank register
foreground pattern bank register
bank-aware tile pattern fetch
unit tests
```

Excluded:

```
per-tile bank selection
tile palette changes
sprite changes
DMA changes
scrolling changes
timing changes
```

The change should remain deterministic and minimal.

---

# Pattern Memory Layout

Pattern memory currently:

```
0x0000 – 0x1FFF  pattern bank 0
0x2000 – 0x3FFF  pattern bank 1
```

Tile size remains:

```
64 bytes
```

Each bank therefore supports:

```
0x2000 / 64 = 128 tiles
```

---

# New VDP Registers

Add two new registers.

```
0xEC = BG_PATTERN_BANK
0xED = FG_PATTERN_BANK
```

Behavior:

```
bit0 = bank select
other bits ignored
```

Reset state:

```
BG_PATTERN_BANK = 0
FG_PATTERN_BANK = 0
```

---

# Pattern Address Resolution

Current tile lookup:

```
pattern_addr = tile_index * 64
```

New rule:

```
pattern_base =
    bank == 0 → 0x0000
    bank == 1 → 0x2000

pattern_addr = pattern_base + tile_index * 64
```

Background uses `BG_PATTERN_BANK`.

Foreground uses `FG_PATTERN_BANK`.

Sprite addressing from M20 remains unchanged.

---

# Rendering Pipeline (unchanged)

```
BG plane
↓
low-priority sprites
↓
FG plane
↓
high-priority sprites
↓
palette resolve
↓
framebuffer
```

Only tile pattern lookup changes.

---

# Files To Create

```
emulator/tests/unit/test_tile_pattern_banks.cpp
artifacts/reports/M21_tile_pattern_banks_report.md
```

---

# Files To Modify

```
emulator/include/vdp.h
emulator/src/vdp.cpp
emulator/include/bus.h
emulator/src/bus.cpp
emulator/CMakeLists.txt
docs/state_snapshot.md
```

Bus routing must now include:

```
0xE0–0xED
```

---

# Unit Tests

Create:

```
emulator/tests/unit/test_tile_pattern_banks.cpp
```

Required tests:

### Test A — BG bank 0 fetch

* load tile pattern into bank 0
* set BG bank register to 0
* verify BG renders expected pixels

---

### Test B — BG bank 1 fetch

* load different tile pattern into bank 1
* set BG bank register to 1
* verify BG renders bank 1 data

---

### Test C — FG bank selection

* load FG tile patterns into both banks
* toggle `FG_PATTERN_BANK`
* verify FG rendering changes

---

### Test D — bank switch changes rendering

* render same tile index under two banks
* verify visual difference

---

### Test E — sprite rendering unaffected

* confirm sprite rendering from M20 still works regardless of tile plane banks

---

# CMake Integration

Modify:

```
emulator/CMakeLists.txt
```

Add:

```
emulator/tests/unit/test_tile_pattern_banks.cpp
```

Create target:

```
super_z80_test_tile_pattern_banks
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
Milestone: M21
Tile planes now support pattern bank selection
Background and foreground pattern addressing extended
```

Next milestone:

```
M22 — Sprite Collision and Sprite Status Flags
```

---

# Commit Message

```
M21: implement tile plane pattern bank selection
```

---

# Completion Report

Create:

```
artifacts/reports/M21_tile_pattern_banks_report.md
```

Include:

```
files created
files modified
tile bank register map
pattern address resolution
build output
test results
commit hash
```

---

# Expected Result

After M21 the VDP will support:

```
banked sprite patterns
banked background tiles
banked foreground tiles
```

This gives the system a much more flexible graphics memory layout while keeping the hardware simple.

---
