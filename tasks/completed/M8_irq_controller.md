# Task Packet

## Title

M8 — IRQ Controller Infrastructure

---

## Branch

```
m8_irq_controller
```

---

# Objective

Introduce an **IRQ Controller subsystem** responsible for managing interrupt requests and driving the Z80 `/INT` line.

This milestone establishes:

* IRQ status register
* IRQ enable register
* IRQ acknowledge behavior
* CPU interrupt line wiring

No hardware sources (VBlank, DMA, etc.) will generate IRQs yet.

---

# Scope

Included:

```
IRQ controller subsystem
IRQ status tracking
IRQ enable mask
CPU interrupt line wiring
unit tests
```

Excluded:

```
VBlank interrupts
DMA interrupts
audio interrupts
IRQ sources from devices
```

This milestone only introduces **infrastructure**.

---

# Files To Create

```
emulator/include/irq_controller.h
emulator/src/irq_controller.cpp
emulator/tests/unit/test_irq.cpp
artifacts/reports/M8_irq_controller_report.md
```

---

# IRQ Controller Design

Create:

`emulator/include/irq_controller.h`

Example structure:

```cpp
#pragma once

#include <cstdint>

namespace superz80 {

class IRQController {
public:

    IRQController();

    void reset();

    void request(uint8_t irq_bit);

    uint8_t status() const;

    uint8_t enable() const;
    void set_enable(uint8_t mask);

    void acknowledge(uint8_t mask);

    bool irq_line() const;

private:

    uint8_t irq_status_;
    uint8_t irq_enable_;

};

}
```

---

# Behavior

### IRQ request

```
request(bit)
```

Sets the corresponding bit in `irq_status`.

---

### IRQ enable

```
irq_enable_
```

Defines which IRQ bits are allowed to trigger `/INT`.

---

### IRQ line state

```
irq_line() == true
```

When:

```
(irq_status & irq_enable) != 0
```

---

### Acknowledge

```
acknowledge(mask)
```

Clears bits in `irq_status`.

---

### Reset

Reset must clear:

```
irq_status = 0
irq_enable = 0
```

---

# Bus Integration

Extend the Bus to expose IRQ registers.

Example ports:

```
0xF0  IRQ_STATUS
0xF1  IRQ_ENABLE
0xF2  IRQ_ACK
```

Behavior:

```
read STATUS -> irq_status
read ENABLE -> irq_enable

write ENABLE -> set enable mask
write ACK -> clear bits
```

---

# CPU Wiring

The CPU wrapper must expose a method to drive the interrupt line.

Example:

```
cpu.set_int_line(bool level)
```

The scheduler or bus will update this based on the IRQ controller.

For now, simply connect:

```
cpu.INT <- irq_controller.irq_line()
```

---

# Unit Tests

Create:

`emulator/tests/unit/test_irq.cpp`

Required tests:

---

### Test A — request sets bit

```
request(0x01)

status == 0x01
```

---

### Test B — enable mask controls IRQ line

```
status = 0x01
enable = 0x00

irq_line == false

enable = 0x01

irq_line == true
```

---

### Test C — acknowledge clears bit

```
request(0x02)

acknowledge(0x02)

status == 0
```

---

### Test D — multiple IRQ bits

```
request(0x01)
request(0x04)

status == 0x05
```

---

# CMake Integration

Modify:

```
emulator/CMakeLists.txt
```

Add:

```
src/irq_controller.cpp
tests/unit/test_irq.cpp
```

Create test target:

```
super_z80_test_irq
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
Milestone: M8
IRQ controller infrastructure introduced
CPU interrupt line wired
IRQ registers accessible through bus
```

Next milestone:

```
M9 — VBlank interrupt source
```

---

# Commit Message

```
M8: introduce IRQ controller infrastructure and CPU interrupt wiring
```

---

# Completion Report

Create:

```
artifacts/reports/M8_irq_controller_report.md
```

Include:

```
files created
files modified
IRQ subsystem design
register map
build output
test results
commit hash
```

---

# Expected Result

After M8 the emulator architecture becomes:

```
Scheduler
   ↓
CPU
   ↓
Bus
   ↓
ROM / RAM / IO
   ↓
IRQ Controller
```

The system now supports **hardware interrupt delivery**.

---

# After M8

Remaining bring-up milestones:

```
M9  VBlank timing + interrupt source
M10 DMA engine
M11 video pipeline
M12 audio subsystem
```

---
