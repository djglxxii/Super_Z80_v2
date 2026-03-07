# Task Packet

## Title

M11 — Video Pipeline Skeleton

---

## Branch

```
m11_video_pipeline
```

---

# Objective

Introduce the **VDP subsystem** with:

```
VRAM storage
video registers
framebuffer storage
scanline callback
bus register interface
```

No sprite engine, tilemaps, or rendering yet.

---

# Scope

Included:

```
VRAM (64 KB)
VDP registers
framebuffer (internal)
scanline entry point
bus port mapping
unit tests
```

Excluded:

```
tile renderer
sprite engine
palette system
DMA to VRAM
video output
```

---

# Files To Create

```
emulator/include/vdp.h
emulator/src/vdp.cpp
emulator/tests/unit/test_vdp.cpp
artifacts/reports/M11_vdp_report.md
```

---

# VDP Register Map

Expose VDP registers via bus ports.

```
0xE0  VDP_STATUS
0xE1  VDP_CONTROL
0xE2  VDP_DATA
```

---

### VDP_STATUS

Read-only status register.

Bits:

```
bit0 = VBlank active
bit1 = frame ready
```

---

### VDP_CONTROL

Write-only control register.

Used to configure internal registers later.

For this milestone simply **store last written value**.

---

### VDP_DATA

Read/write data port used for VRAM access.

Behavior:

```
read  -> VRAM[vram_pointer]
write -> VRAM[vram_pointer] = value
```

Auto-increment pointer.

---

# VDP Class

Create:

```
emulator/include/vdp.h
```

Example structure:

```cpp
#pragma once

#include <cstdint>
#include <array>

namespace superz80 {

class VDP {
public:

    VDP();

    void reset();

    uint8_t read_port(uint8_t port);
    void write_port(uint8_t port, uint8_t value);

    void step_scanline(uint32_t scanline);

    bool frame_ready() const;

private:

    std::array<uint8_t, 65536> vram_;

    uint16_t vram_ptr_;

    uint8_t control_reg_;

    bool frame_ready_;

};

}
```

---

# VRAM

Allocate:

```
64 KB
```

```cpp
std::array<uint8_t, 65536>
```

Initialize to zero on reset.

---

# Framebuffer

Add internal framebuffer storage:

```
256 × 192
```

```cpp
std::array<uint8_t, 256 * 192>
```

No rendering yet.

Just allocate.

---

# Scheduler Integration

Add scanline callback.

In scheduler:

```
vdp.step_scanline(current_scanline)
```

---

# Frame Completion

At scanline:

```
191
```

Set:

```
frame_ready = true
```

Reset when read through status register.

---

# Bus Integration

Modify bus port routing:

```
0xE0–0xE2 → VDP subsystem
```

---

# Unit Tests

Create:

```
emulator/tests/unit/test_vdp.cpp
```

Required tests:

---

### Test A — VRAM write/read

```
write DATA
read DATA
value matches
```

---

### Test B — VRAM pointer increment

```
write 3 bytes

pointer increments correctly
```

---

### Test C — reset clears VRAM

After reset:

```
all bytes == 0
```

---

### Test D — frame ready flag

Simulate scanlines.

When scanline:

```
191
```

Then:

```
frame_ready == true
```

Reading status clears flag.

---

# CMake Integration

Modify:

```
emulator/CMakeLists.txt
```

Add:

```
src/vdp.cpp
tests/unit/test_vdp.cpp
```

Add test target:

```
super_z80_test_vdp
```

---

# Build Verification

Run:

```
cmake -S . -B build
cmake --build build
```

---

# Test Verification

Run:

```
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
Milestone: M11
VDP skeleton implemented
VRAM storage introduced
scanline callback connected
```

Next milestone:

```
M12 — Tile Renderer
```

---

# Commit Message

```
M11: introduce VDP subsystem with VRAM and scanline pipeline
```

---

# Completion Report

Create:

```
artifacts/reports/M11_vdp_report.md
```

Include:

```
files created
files modified
VRAM layout
VDP port map
build output
test results
commit hash
```

---

# Expected Result

After M11 the emulator will have:

```
Scheduler
   ↓
VDP scanline pipeline
   ↓
framebuffer
```

This introduces the **entire video pipeline skeleton**.

---

# After M11

Remaining bring-up milestones:

```
M12 Tile renderer
M13 Sprite engine
M14 Audio subsystem
```

---
