# Task Packet

## Title

M14 — Palette and Color System

## Branch

```text
m14_palette_system
```

---

## Objective

Implement the first palette-based color system for the VDP so that tile and sprite rendering produce **indexed pixels** that can be resolved through a **palette table** into concrete framebuffer colors.

This milestone establishes:

* palette RAM / palette storage
* palette register access
* indexed-color lookup
* deterministic framebuffer color conversion
* unit tests for palette behavior

---

## Scope

Included:

* palette storage
* VDP palette ports/register behavior
* index-to-color lookup
* deterministic color conversion path
* unit tests

Excluded:

* multiple palette banks
* color blending
* per-sprite/per-tile palette selection
* RGB hardware-accurate color packing
* SDL renderer upgrades
* scrolling
* priority rules

This milestone introduces only the **base palette system**.

---

## Temporary Color Model for M14

Use a deliberately simple bring-up model.

### Palette entry count

```text
256 entries
```

### Palette entry format

Each palette entry is:

```text
32-bit RGBA
```

Suggested type:

```cpp
uint32_t
```

### Initial default

After reset:

```text
palette[i] = 0x000000FF for all entries
```

or another clearly documented deterministic default.

Recommended alternative for easier tests:

```text
palette[i] = 0x00000000
```

Choose one and document it clearly in the report and tests.

### Indexed framebuffer model

Keep the existing framebuffer as **8-bit pixel indices**.

Add a second conversion/output path that resolves these indices through the palette.

---

## Files To Create

```text
emulator/tests/unit/test_palette.cpp
artifacts/reports/M14_palette_report.md
```

---

## Files To Modify

```text
emulator/include/vdp.h
emulator/src/vdp.cpp
emulator/CMakeLists.txt
docs/state_snapshot.md
```

If needed, you may also touch bus routing for palette ports, but keep it minimal.

---

## VDP Port Additions

Add a simple palette register interface via ports.

Recommended ports:

```text
0xE3  PAL_INDEX
0xE4  PAL_DATA_R
0xE5  PAL_DATA_G
0xE6  PAL_DATA_B
0xE7  PAL_DATA_A
```

Behavior:

### `PAL_INDEX`

* selects the active palette entry index

### `PAL_DATA_R/G/B/A`

* write components into the selected palette entry
* reads return the stored component values

This is intentionally simple and software-friendly for bring-up.

No auto-increment required yet unless you want it and can keep it deterministic and well-tested.

---

## VDP API Changes

Add palette-facing helpers to `VDP`, for example:

```cpp
uint32_t palette_entry(uint8_t index) const;
uint32_t framebuffer_color(std::size_t pixel_index) const;
void set_palette_entry(uint8_t index, uint32_t rgba);
```

Also add a conversion function, for example:

```cpp
void resolve_framebuffer_colors();
```

or

```cpp
uint32_t resolve_pixel(uint8_t palette_index) const;
```

Keep the design small and direct.

---

## Rendering Behavior

Tile and sprite rendering should continue writing **8-bit palette indices** into the framebuffer exactly as they do now.

The new palette system should provide deterministic resolution from those indices to actual colors.

This means:

```text
framebuffer_index[pixel] -> palette[framebuffer_index[pixel]] -> RGBA color
```

You may store resolved colors in a second internal buffer if useful, but do not overbuild this.

---

## Unit Tests

Create:

```text
emulator/tests/unit/test_palette.cpp
```

Required tests:

### Test A — palette write/read

* select palette index
* write RGBA components
* read them back
* verify entry matches expected 32-bit color

### Test B — palette entry update

* write one palette entry
* overwrite it
* verify new value replaces old value

### Test C — pixel index resolves through palette

* set palette entry 5 to a known RGBA value
* write framebuffer index 5 at a known pixel
* verify resolved color equals palette[5]

### Test D — different indices resolve independently

* set palette[1] and palette[2] to different colors
* write those indices into different framebuffer locations
* verify the resolved colors differ correctly

### Test E — reset clears palette deterministically

* modify palette
* reset VDP
* verify entries return to documented default

---

## CMake Integration

Modify:

```text
emulator/CMakeLists.txt
```

Add:

```text
emulator/tests/unit/test_palette.cpp
```

Create target:

```text
super_z80_test_palette
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
Milestone: M14
Palette and color system implemented
Indexed framebuffer now resolves through palette entries
```

Next milestone:

```text
M15 — Scrolling and Viewport Movement
```

---

## Commit Message

```text
M14: implement palette storage and indexed color resolution
```

---

## Completion Report

Create:

```text
artifacts/reports/M14_palette_report.md
```

Include:

* files created
* files modified
* palette register map
* palette storage format
* default reset state
* color resolution behavior
* build output
* test results
* commit hash

---

## Expected Result

After M14, the emulator will support:

```text
tile/sprite pixel indices
  ↓
palette lookup
  ↓
resolved display colors
```

This is the first real color model for the Super_Z80_v2 VDP.

---

