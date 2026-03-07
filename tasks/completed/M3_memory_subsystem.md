# Codex Task Packet

## Task ID

M3

## Title

Memory Subsystem Implementation

## Branch

```
m3_memory_subsystem
```

---

# Codex CLI Prompt

You are operating inside the **Super_Z80_v2** repository.

Follow the repository truth defined in:

* `AGENTS.md`
* `docs/spec.md`
* `docs/architecture.md`
* `docs/state_snapshot.md`

Follow the workflow rules defined in:

```
workflow/task_template.md
workflow/report_template.md
```

---

# Objective

Implement the **first deterministic memory subsystem** for the emulator.

This milestone creates the **emulator addressable RAM model** and read/write primitives that future components (CPU, IO, DMA) will depend on.

No CPU integration occurs in this milestone.

---

# Scope

Implement:

```
emulator memory model
RAM allocation
deterministic read/write API
unit tests
```

Do **NOT** implement:

```
CPU
Bus
IO registers
DMA
video
audio
cartridge mappers
```

This milestone only provides **raw memory behavior**.

---

# Memory Model Requirements

The emulator must provide **64KB addressable memory**.

Memory characteristics:

```
Address space: 0x0000–0xFFFF
Size: 65536 bytes
Type: byte-addressable
```

Initial behavior:

```
All memory initialized to 0x00
Reads return stored value
Writes overwrite value
```

No mirroring or mapping yet.

---

# Files To Create

Create:

```
emulator/include/memory.h
emulator/src/memory.cpp
emulator/tests/unit/test_memory.cpp
artifacts/reports/M3_memory_subsystem_report.md
```

---

# Header Specification

Create:

```
emulator/include/memory.h
```

Example structure:

```cpp
#pragma once

#include <cstdint>
#include <array>

namespace superz80
{

class Memory
{
public:

    static constexpr std::size_t kSize = 65536;

    Memory();

    uint8_t read(uint16_t address) const;

    void write(uint16_t address, uint8_t value);

    void reset();

private:

    std::array<uint8_t, kSize> ram;
};

}
```

---

# Implementation

Create:

```
emulator/src/memory.cpp
```

Requirements:

```
reset() clears RAM to 0x00
read() returns value
write() stores value
```

All behavior must be **deterministic**.

No undefined behavior.

---

# Unit Tests

Create:

```
emulator/tests/unit/test_memory.cpp
```

Test cases required:

### Test 1

```
memory initializes to zero
```

Verify:

```
read(0x0000) == 0x00
read(0xFFFF) == 0x00
```

---

### Test 2

```
write then read returns same value
```

Example:

```
write(0x1234, 0xAB)
read(0x1234) == 0xAB
```

---

### Test 3

```
multiple addresses independent
```

Example:

```
write(0x1000, 0x11)
write(0x2000, 0x22)
read(0x1000) == 0x11
read(0x2000) == 0x22
```

---

### Test 4

```
reset clears memory
```

Example:

```
write(0x3000, 0x55)
reset()
read(0x3000) == 0x00
```

---

# Build Integration

Modify:

```
emulator/CMakeLists.txt
```

Add new sources:

```
src/memory.cpp
tests/unit/test_memory.cpp
```

Ensure test target compiles.

---

# Build Verification

Run:

```
cmake -S . -B build
cmake --build build
```

---

# Test Execution

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
Milestone: M3
Memory subsystem implemented
Unit tests passing
```

---

# Report

Create:

```
artifacts/reports/M3_memory_subsystem_report.md
```

Include:

```
summary
files created
files modified
build output summary
test results
commit hash
```

---

# Commit

Commit message:

```
M3: Implement deterministic memory subsystem and unit tests
```

---

# Expected Result

After this milestone:

The emulator will contain:

```
build system
CLI executable
CRC verification utilities
unit test harness
deterministic memory subsystem
```

The repository will now support **safe CPU integration in the next milestone**.

---
