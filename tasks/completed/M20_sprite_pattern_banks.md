# Task Packet

## Title

M20 — Sprite Pattern Banks and Extended Tile Addressing

## Branch

```text
m20_sprite_pattern_banks
```

## Objective

Extend sprite tile addressing so sprite entries can reference a larger pattern space than the current simple `tile_index * 64` model alone.

This milestone establishes:

* sprite pattern bank selection
* extended sprite tile addressing
* deterministic sprite pattern fetch across banks
* unit tests for banked sprite lookup

This is still a focused graphics milestone. It does **not** add new rendering layers or new timing behavior.

---

## Scope

Included:

* sprite pattern bank selection
* expanded sprite pattern addressing
* bank-aware sprite rendering
* unit tests

Excluded:

* tile plane bank switching
* per-sprite palette selection
* sprite scaling
* sprite rotation
* DMA changes
* new interrupt behavior

Keep the change narrow and deterministic.

---

## Sprite Attribute Layout Extension

Sprite entry remains:

```text
byte0 = Y
byte1 = X
byte2 = tile index
byte3 = flags
```

Current flags:

```text
bit0 = priority
bit1 = horizontal flip
bit2 = vertical flip
bit3–7 = reserved
```

For this milestone, define:

```text
bit3 = sprite pattern bank select
bit4–7 = reserved
```

Bank meaning:

```text
0 = bank 0
1 = bank 1
```

This gives a simple 1-bit bank model without overbuilding.

---

## Pattern Addressing Model

Current sprite pattern base:

```text
0x0000
```

Current tile size:

```text
64 bytes per sprite tile
```

Add a second sprite pattern bank at:

```text
bank 0 base = 0x0000
bank 1 base = 0x2000
```

Sprite pattern fetch rule becomes:

```text
pattern_base = (bank_select == 0) ? 0x0000 : 0x2000
pattern_addr = pattern_base + tile_index * 64
```

This gives each sprite bank room for:

```text
0x2000 / 64 = 128 sprite tiles
```

That is enough for this milestone.

---

## Files To Create

```text
emulator/tests/unit/test_sprite_pattern_banks.cpp
artifacts/reports/M20_sprite_pattern_banks_report.md
```

## Files To Modify

```text
emulator/include/vdp.h
emulator/src/vdp.cpp
emulator/CMakeLists.txt
docs/state_snapshot.md
```

---

## Rendering Behavior

Sprite rendering logic remains otherwise unchanged.

The only change is how the tile pattern address is resolved.

Transparency, priority, scanline limits, and flip behavior must continue to work exactly as before.

---

## Unit Tests

Create:

```text
emulator/tests/unit/test_sprite_pattern_banks.cpp
```

Required tests:

### Test A — bank 0 sprite fetch

* define tile data for sprite tile 0 in bank 0
* create sprite using bank 0
* verify rendered pixels come from bank 0 pattern memory

### Test B — bank 1 sprite fetch

* define different tile data for sprite tile 0 in bank 1
* create sprite using bank 1
* verify rendered pixels come from bank 1 pattern memory

### Test C — same tile index, different bank, different output

* sprite A uses tile index 0, bank 0
* sprite B uses tile index 0, bank 1
* verify each renders from its own bank

### Test D — banked sprites still obey transparency

* use bank 1 tile containing zero and non-zero pixels
* verify zero remains transparent

### Test E — banked sprites still obey priority and flipping

* create banked sprite with flip bits and priority bit set
* verify rendering behavior remains correct

---

## CMake Integration

Modify:

```text
emulator/CMakeLists.txt
```

Add:

```text
emulator/tests/unit/test_sprite_pattern_banks.cpp
```

Create target:

```text
super_z80_test_sprite_pattern_banks
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
Milestone: M20
Sprite pattern bank selection implemented
Sprite tile addressing extended beyond a single base range
```

Next milestone:

```text
M21 — Tile Plane Pattern Banks and Extended Background Addressing
```

---

## Commit Message

```text
M20: implement sprite pattern banks and extended tile addressing
```

---

## Completion Report

Create:

```text
artifacts/reports/M20_sprite_pattern_banks_report.md
```

Include:

* files created
* files modified
* updated sprite flag layout
* bank selection semantics
* pattern address formula
* build output
* test results
* commit hash

---

## Expected Result

After M20, the sprite system will support:

```text
position
priority
horizontal flip
vertical flip
pattern bank select
```

This gives the Super_Z80_v2 sprite engine a much more realistic content-addressing model while preserving deterministic behavior.
