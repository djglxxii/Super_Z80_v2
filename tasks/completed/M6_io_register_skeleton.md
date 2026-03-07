# Task Packet

## Title

M6 — I/O Register Skeleton

---

## Branch

```
m6_io_register_skeleton
```

---

# Objective

Introduce an **I/O register subsystem** that the Bus routes port reads and writes to.

This establishes the hardware boundary used later by:

```
VDP
DMA
IRQ controller
audio
input devices
```

The registers will be **stubbed only**.

No hardware logic yet.

---

# Scope

Included:

```
IO register module
port read/write routing
stub register behavior
unit tests
```

Excluded:

```
IRQ logic
VBlank logic
DMA logic
video registers
audio registers
timing model
```

This milestone establishes only the **port interface**.

---

# Port Address Range

For now implement a minimal port space:

```
0x00–0xFF  IO register space
```

Behavior rules:

```
reads return stored value
writes update stored value
reset clears registers to 0x00
```

Later milestones will override specific registers.

---

# Files To Create

```
emulator/include/io.h
emulator/src/io.cpp
emulator/tests/unit/test_io.cpp
artifacts/reports/M6_io_register_report.md
```

---

# IO Register Class

Create:

```
emulator/include/io.h
```

Example design:

```cpp
#pragma once

#include <cstdint>
#include <array>

namespace superz80 {

class IO {
public:

    IO();

    void reset();

    uint8_t read(uint8_t port) const;

    void write(uint8_t port, uint8_t value);

private:

    std::array<uint8_t,256> ports_;

};

}
```

---

# Implementation

Create:

```
emulator/src/io.cpp
```

Requirements:

```
reset() clears all ports to 0x00
read() returns stored value
write() stores value
```

No device logic yet.

---

# Bus Integration

Extend the Bus so the CPU can access ports.

Add:

```
Bus::read_port(uint8_t port)
Bus::write_port(uint8_t port, uint8_t value)
```

These must call the IO subsystem.

The CPU wrapper must connect Z80ex **port callbacks** to these functions.

---

# CPU Integration

Z80ex provides callbacks for:

```
IN instruction
OUT instruction
```

Wire these to:

```
bus.read_port()
bus.write_port()
```

---

# Unit Tests

Create:

```
emulator/tests/unit/test_io.cpp
```

Required tests:

### Test A

```
write/read port
```

Example:

```
write(0x10, 0x55)
read(0x10) == 0x55
```

---

### Test B

```
independent ports
```

```
write(0x10,0x11)
write(0x20,0x22)

read(0x10)==0x11
read(0x20)==0x22
```

---

### Test C

```
reset clears ports
```

```
write(0x33,0x77)
reset()
read(0x33)==0x00
```

---

# CMake Changes

Modify:

```
emulator/CMakeLists.txt
```

Add:

```
src/io.cpp
tests/unit/test_io.cpp
```

Add test target:

```
super_z80_test_io
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
Milestone: M6
IO register subsystem introduced
CPU port access routed through bus
stub registers operational
```

Next milestone:

```
M7 — Scheduler and timing system
```

---

# Commit Message

```
M6: introduce IO register subsystem and port routing
```

---

# Completion Report

Create:

```
artifacts/reports/M6_io_register_report.md
```

Include:

```
files created
files modified
IO register design summary
port routing behavior
build output summary
test results
commit hash
```

---

# Expected Result

After M6 the emulator architecture becomes:

```
CPU
 ↓
Bus
 ↓
ROM / RAM
 ↓
IO registers
```

The system now supports **real CPU I/O instructions**.

---

# After M6

Next milestones will follow the hardware bring-up order:

```
M7 scheduler / timing
M8 IRQ controller
M9 VBlank logic
M10 DMA engine
M11 video pipeline
M12 audio system
```

---
