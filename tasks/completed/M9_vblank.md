# Task Packet

## Title

M9 — VBlank Timing and Interrupt Source

---

## Branch

```
m9_vblank_irq
```

---

# Objective

Introduce **VBlank state transitions** based on scanline timing and generate a **VBlank interrupt request** through the IRQ controller.

This milestone establishes:

```
VBlank begin/end timing
VBlank status flag
IRQ request from scheduler
CPU interrupt delivery
```

No rendering or VRAM behavior is introduced yet.

---

# Timing Model

Use the timing model already established in the scheduler:

```
scanlines per frame = 262
visible scanlines   = 0–191
vblank scanlines    = 192–261
```

Transitions:

```
scanline 192 → VBlank begins
scanline 0   → VBlank ends
```

---

# Scope

Included:

```
VBlank state tracking
VBlank interrupt generation
VDP status bit
scheduler integration
unit tests
```

Excluded:

```
PPU rendering
sprite logic
tilemap logic
VRAM behavior
DMA
audio
```

This milestone only introduces **VBlank state and interrupt generation**.

---

# Files To Create

```
emulator/include/vblank.h
emulator/src/vblank.cpp
emulator/tests/unit/test_vblank.cpp
artifacts/reports/M9_vblank_report.md
```

---

# VBlank Class

Create:

```
emulator/include/vblank.h
```

Example structure:

```cpp
#pragma once

#include <cstdint>
#include "irq_controller.h"

namespace superz80 {

class VBlank {
public:

    VBlank(IRQController& irq);

    void reset();

    void update_scanline(uint32_t scanline);

    bool active() const;

private:

    IRQController& irq_;
    bool vblank_active_;

};

}
```

---

# Behavior

### Enter VBlank

When scanline becomes:

```
192
```

Then:

```
vblank_active = true
irq.request(VBLANK_BIT)
```

Choose a VBlank bit:

```
IRQ bit 0
```

---

### Exit VBlank

When scanline becomes:

```
0
```

Then:

```
vblank_active = false
```

---

# Scheduler Integration

Modify the scheduler.

When advancing scanlines:

```
scheduler.step_scanline()
```

Call:

```
vblank.update_scanline(current_scanline)
```

This allows the scheduler to drive hardware timing events.

---

# Bus Register

Expose a VBlank status bit.

Add a register read through the bus:

```
0xF3  VDP_STATUS
```

Bit layout:

```
bit0 = VBlank active
```

Reading this register must reflect the VBlank state.

Writes ignored.

---

# Unit Tests

Create:

```
emulator/tests/unit/test_vblank.cpp
```

Required tests:

---

### Test A — VBlank begins at scanline 192

```
scanline = 192

vblank.active() == true
```

---

### Test B — VBlank ends at scanline 0

```
scanline = 192 → active
scanline = 0 → inactive
```

---

### Test C — IRQ request on VBlank begin

```
scanline = 192

irq.status bit0 == 1
```

---

### Test D — no repeated IRQs within same VBlank

Advancing scanlines within the VBlank window should **not generate additional requests**.

---

# CMake Integration

Modify:

```
emulator/CMakeLists.txt
```

Add:

```
src/vblank.cpp
tests/unit/test_vblank.cpp
```

Add test target:

```
super_z80_test_vblank
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
Milestone: M9
VBlank timing implemented
VBlank interrupt source active
scheduler generating hardware events
```

Next milestone:

```
M10 — DMA Engine
```

---

# Commit Message

```
M9: implement VBlank timing and interrupt source
```

---

# Completion Report

Create:

```
artifacts/reports/M9_vblank_report.md
```

Include:

```
files created
files modified
VBlank timing model
IRQ integration
build output
test results
commit hash
```

---

# Expected Result

After M9 the emulator will have its **first real hardware event**:

```
Scheduler
   ↓
VBlank timing
   ↓
IRQ controller
   ↓
CPU interrupt line
```

This proves the entire **timing → hardware → interrupt → CPU pipeline** works.

---

# Remaining Bring-Up Milestones

```
M10 DMA engine
M11 video pipeline
M12 audio subsystem
```

---
