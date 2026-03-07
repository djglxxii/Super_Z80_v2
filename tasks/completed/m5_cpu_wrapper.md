# Task Packet

## Title

M5 — CPU Wrapper and Z80ex Integration

---

## Branch

```
m5_cpu_wrapper
```

---

# Objective

Integrate the **Z80ex CPU core** into the emulator and connect it to the Bus so that the CPU can execute instructions against ROM/RAM through bus reads and writes.

This milestone establishes:

* CPU wrapper layer
* Z80ex integration
* bus read/write callbacks
* deterministic single-step execution
* unit test verifying instruction execution

---

# Scope

Included:

```
Z80ex integration
CPU wrapper class
Bus memory callbacks
CPU reset
CPU single-step execution
unit tests
```

Excluded:

```
interrupts
DMA
timing model
scheduler
PPU
APU
IO registers
SDL integration changes
```

The CPU will be driven **manually by test code only**.

---

# Dependencies

Use the repository-approved dependency:

```
Z80ex
```

Vendor location:

```
emulator/vendor/z80ex/
```

If the library is not yet present, include it as a **vendor drop**.

Do not add new package managers or build systems.

---

# Files To Create

```
emulator/include/cpu.h
emulator/src/cpu.cpp
emulator/tests/unit/test_cpu.cpp
artifacts/reports/M5_cpu_wrapper_report.md
```

---

# CPU Wrapper Design

Create:

```
emulator/include/cpu.h
```

Suggested structure:

```cpp
#pragma once

#include <cstdint>
#include "bus.h"

namespace superz80 {

class CPU {
public:

    CPU(Bus& bus);

    void reset();

    void step();

private:

    Bus& bus_;
    void* cpu_; // opaque Z80ex pointer

};

}
```

---

# Bus Integration

Z80ex must perform all memory access through the Bus.

Memory callbacks must call:

```
bus.read(address)
bus.write(address, value)
```

No direct memory access allowed.

---

# CPU Reset Behavior

`CPU::reset()` must:

* reset Z80ex state
* set PC to `0x0000`

---

# CPU Step

`CPU::step()` must:

```
execute exactly one instruction
```

The function should internally call the Z80ex step function.

Return value is not required.

---

# Unit Test

Create:

```
emulator/tests/unit/test_cpu.cpp
```

The test should validate basic instruction execution.

---

## Test ROM

Use a minimal ROM:

```
0x0000: LD A,0x42
0x0002: HALT
```

Machine code:

```
3E 42
76
```

---

# Test Procedure

1. Load ROM into Bus
2. Create CPU instance
3. Reset CPU
4. Execute two steps

Expected behavior:

* CPU loads `A = 0x42`
* HALT instruction executed

The test should verify register A equals `0x42`.

---

# CMake Integration

Modify:

```
emulator/CMakeLists.txt
```

Add:

```
src/cpu.cpp
tests/unit/test_cpu.cpp
```

Link Z80ex sources.

Ensure a test executable exists:

```
super_z80_test_cpu
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
Milestone: M5
CPU wrapper integrated
Z80ex connected to bus
single instruction stepping verified
```

Next milestone:

```
M6 — IO Register Skeleton
```

---

# Commit Message

```
M5: integrate Z80ex CPU wrapper and bus memory callbacks
```

---

# Completion Report

Create:

```
artifacts/reports/M5_cpu_wrapper_report.md
```

Include:

* files created
* files modified
* CPU wrapper description
* Z80ex integration notes
* build output summary
* test results
* commit hash

---

# Expected Result

After M5 the emulator architecture becomes:

```
Application
   ↓
CPU
   ↓
Bus
   ↓
ROM / RAM
```

The system will now execute real Z80 instructions.

---

# After M5

Next milestones should progress as:

```
M6 IO register skeleton
M7 scheduler and timing
M8 IRQ system
M9 VBlank
M10 DMA
M11 video pipeline
M12 audio system
```

---
