# Task Packet

## Title

M10 — DMA Engine Infrastructure

---

## Branch

```
m10_dma_engine
```

---

# Objective

Introduce a deterministic **DMA engine subsystem** capable of copying blocks of memory between address spaces under scheduler control.

This milestone establishes:

```
DMA registers
DMA request mechanism
scheduler-driven DMA execution
deterministic transfer rules
unit tests
```

No video hardware or VRAM is required yet.

---

# Scope

Included:

```
DMA controller subsystem
DMA register interface
block transfer logic
scheduler-driven execution
unit tests
```

Excluded:

```
VRAM
sprite engine
tilemaps
audio streaming
```

DMA will operate **RAM → RAM only** for now.

---

# Files To Create

```
emulator/include/dma.h
emulator/src/dma.cpp
emulator/tests/unit/test_dma.cpp
artifacts/reports/M10_dma_report.md
```

---

# DMA Register Map

Expose DMA registers through the bus.

```
0xF4  DMA_SRC_LOW
0xF5  DMA_SRC_HIGH
0xF6  DMA_DST_LOW
0xF7  DMA_DST_HIGH
0xF8  DMA_LEN
0xF9  DMA_CONTROL
```

### Control Register

```
bit0 = start DMA
bit7 = busy
```

Writing bit0 begins a transfer.

---

# DMA Class

Create:

```
emulator/include/dma.h
```

Example structure:

```cpp
#pragma once

#include <cstdint>

namespace superz80 {

class Bus;

class DMA {
public:

    DMA(Bus& bus);

    void reset();

    void write_register(uint8_t port, uint8_t value);
    uint8_t read_register(uint8_t port) const;

    void step();

    bool busy() const;

private:

    Bus& bus_;

    uint16_t src_;
    uint16_t dst_;
    uint8_t length_;

    bool active_;

};

}
```

---

# DMA Behavior

### Start DMA

Writing to `DMA_CONTROL` with bit0 set:

```
active = true
```

---

### Transfer Model

DMA executes **one byte per scheduler step**.

Pseudo behavior:

```
mem[dst] = mem[src]

src++
dst++
length--

if length == 0
    active = false
```

---

### Busy Flag

Reading `DMA_CONTROL` returns:

```
bit7 = active
```

---

# Scheduler Integration

Add DMA stepping to the scheduler.

Each scanline:

```
dma.step()
```

This produces deterministic timing.

---

# Bus Integration

The bus must route DMA register access.

Example:

```
0xF4–0xF9 → DMA subsystem
```

---

# Unit Tests

Create:

```
emulator/tests/unit/test_dma.cpp
```

Required tests:

---

### Test A — register write/read

```
write src/dst/length

read values back
```

---

### Test B — DMA start

```
write control start bit

dma.busy() == true
```

---

### Test C — single byte transfer

Initialize:

```
RAM[0x1000] = 0xAA
```

Transfer:

```
src = 0x1000
dst = 0x2000
len = 1
```

After step:

```
RAM[0x2000] == 0xAA
busy == false
```

---

### Test D — multi-byte transfer

Transfer length `4`.

Verify:

```
dst bytes match source bytes
```

---

# CMake Integration

Modify:

```
emulator/CMakeLists.txt
```

Add:

```
src/dma.cpp
tests/unit/test_dma.cpp
```

Add test target:

```
super_z80_test_dma
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
Milestone: M10
DMA engine infrastructure implemented
scheduler-driven memory transfers
```

Next milestone:

```
M11 — Video Pipeline
```

---

# Commit Message

```
M10: introduce DMA engine subsystem and scheduler integration
```

---

# Completion Report

Create:

```
artifacts/reports/M10_dma_report.md
```

Include:

```
files created
files modified
DMA register map
transfer behavior
build output
test results
commit hash
```

---

# Expected Result

After M10 the emulator will have:

```
Scheduler
   ↓
DMA engine
   ↓
Bus
   ↓
Memory
```

Meaning the emulator now supports **hardware-driven memory transfers**.

---

# System Status After M10

At that point the emulator core will include:

```
CPU
Bus
Memory
IO
Scheduler
IRQ
VBlank
DMA
```

Which is essentially the **complete motherboard skeleton**.

---

Next milestone after M10 will begin constructing the **video pipeline (VDP)**.
