# Task Packet

## Title

M7 — Scheduler and Timing System

---

## Branch

```
m7_scheduler
```

---

# Objective

Introduce a **Scheduler** responsible for advancing emulator time in deterministic units.

The scheduler will drive:

```
CPU execution
scanline progression
frame progression
```

No hardware reactions (IRQ, VBlank, DMA) occur yet.

This milestone introduces only **time progression**.

---

# Timing Model

Use a simplified placeholder timing model.

For now:

```
scanlines per frame = 262
visible scanlines   = 0–191
```

Exact cycle counts are not required yet.

The scheduler will simply:

```
advance scanline counter
advance frame counter
```

CPU execution will be tied to scheduler steps.

---

# Scope

Included:

```
Scheduler subsystem
scanline counter
frame counter
CPU stepping through scheduler
unit tests
```

Excluded:

```
interrupts
VBlank behavior
DMA
video rendering
audio
hardware reactions
```

---

# Files To Create

```
emulator/include/scheduler.h
emulator/src/scheduler.cpp
emulator/tests/unit/test_scheduler.cpp
artifacts/reports/M7_scheduler_report.md
```

---

# Scheduler Class

Create:

`emulator/include/scheduler.h`

Example structure:

```cpp
#pragma once

#include <cstdint>
#include "cpu.h"

namespace superz80 {

class Scheduler {
public:

    Scheduler(CPU& cpu);

    void reset();

    void step_scanline();

    uint32_t frame() const;
    uint32_t scanline() const;

private:

    CPU& cpu_;

    uint32_t frame_;
    uint32_t scanline_;

};

}
```

---

# Behavior

### Reset

```
frame = 0
scanline = 0
```

---

### Step Scanline

`step_scanline()` must:

```
execute some CPU instructions
advance scanline
wrap after 262
increment frame
```

Example:

```
scanline++
if scanline == 262
    scanline = 0
    frame++
```

For now the CPU may simply execute a **fixed small number of instructions** per scanline.

Example:

```
for i in 0..9
    cpu.step()
```

This is a placeholder until the real timing model is implemented.

---

# Unit Tests

Create:

`emulator/tests/unit/test_scheduler.cpp`

Required tests:

---

### Test A — reset state

```
reset()
frame == 0
scanline == 0
```

---

### Test B — scanline increment

```
step_scanline()

scanline == 1
frame == 0
```

---

### Test C — frame wrap

```
step_scanline() 262 times

scanline == 0
frame == 1
```

---

### Test D — CPU stepping occurs

Use a small ROM and verify the CPU executes instructions when the scheduler advances.

---

# Bus / CPU Integration

The scheduler should interact only with:

```
CPU
```

It must **not** interact with:

```
Bus
IO
memory
```

Those interactions occur through CPU execution.

---

# CMake Integration

Modify:

```
emulator/CMakeLists.txt
```

Add:

```
src/scheduler.cpp
tests/unit/test_scheduler.cpp
```

Add test executable:

```
super_z80_test_scheduler
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

`docs/state_snapshot.md`

Add:

```
Milestone: M7
Scheduler introduced
scanline timing model active
frame progression implemented
```

Next milestone:

```
M8 — IRQ Controller
```

---

# Commit Message

```
M7: introduce scheduler and scanline timing progression
```

---

# Completion Report

Create:

```
artifacts/reports/M7_scheduler_report.md
```

Include:

```
files created
files modified
scheduler design
timing model implemented
build output
test results
commit hash
```

---

# Expected Result

After M7 the emulator architecture becomes:

```
Scheduler
   ↓
CPU
   ↓
Bus
   ↓
ROM / RAM / IO
```

The emulator now has **deterministic time progression**.

---

# Next Milestones

The hardware bring-up path will then be:

```
M8  IRQ controller
M9  VBlank timing
M10 DMA engine
M11 video pipeline
M12 audio subsystem
```

