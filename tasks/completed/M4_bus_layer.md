# Task Packet

## Title

M4 — Bus Layer and Initial CPU-Visible Memory Map

---

## Goal

Implement the first **Bus** layer for Super_Z80_v2 so that memory access is routed through a central hardware boundary instead of directly through the raw memory subsystem.

This milestone must establish:

* a standalone `Bus` module
* ROM region support
* RAM region support
* deterministic address decode behavior
* open-bus behavior for unmapped regions
* unit tests for routing correctness

This milestone does **not** integrate the CPU yet. It prepares the architecture so CPU integration can happen cleanly in the next milestone.

---

## Scope

This task includes:

* creating a `Bus` module
* routing reads/writes by address range
* adding ROM storage/loading support sufficient for bus tests
* exposing RAM access through the existing memory subsystem
* implementing open-bus reads and ignored unmapped writes
* adding unit tests
* updating the project state snapshot

This task excludes:

* Z80ex integration
* CPU stepping
* IO register implementation
* DMA
* VDP
* APU
* scheduler/timing
* bank switching
* cartridge mapper logic beyond a fixed ROM window

Keep the implementation minimal and deterministic.

---

## Context

Super_Z80_v2 has completed:

* bootstrap
* dependency policy clarification
* M1 emulator skeleton
* M2 CRC utilities and unit test foundation
* M3 deterministic memory subsystem

The next correct architectural layer is the **Bus**.

The Bus exists to prevent future CPU code from coupling directly to RAM/ROM internals. All future CPU-visible reads and writes must go through this layer.

The initial bus memory map for this milestone is:

```text
0x0000–0x7FFF  ROM
0x8000–0xBFFF  unmapped / open bus
0xC000–0xFFFF  RAM
```

Behavior rules:

```text
unmapped reads  -> 0xFF
unmapped writes -> ignored
ROM writes      -> ignored
RAM reads/writes operate normally
```

This milestone is intentionally narrow and should match the implementation-first philosophy already established in repository truth.

---

## Files Expected to Change

Create or modify only the following areas:

```text
emulator/include/
emulator/src/
emulator/tests/unit/
emulator/CMakeLists.txt
docs/state_snapshot.md
artifacts/reports/M4_bus_layer_report.md
```

Expected files:

```text
emulator/include/bus.h
emulator/src/bus.cpp
emulator/tests/unit/test_bus.cpp
artifacts/reports/M4_bus_layer_report.md
```

You may also modify the existing memory module only if strictly necessary for clean integration, but avoid broad refactoring.

---

## Implementation Requirements

### 1. Create Bus Header

Create:

```text
emulator/include/bus.h
```

Recommended shape:

```cpp
#pragma once

#include <cstddef>
#include <cstdint>
#include <vector>
#include "memory.h"

namespace superz80 {

class Bus {
public:
    Bus();

    void reset();

    void load_rom(const void* data, std::size_t size);

    uint8_t read(uint16_t address) const;
    void write(uint16_t address, uint8_t value);

private:
    std::vector<uint8_t> rom_;
    Memory memory_;
};

}
```

This API may be adjusted slightly if needed, but keep it small and obvious.

---

### 2. ROM Behavior

Implement ROM support inside the Bus.

Requirements:

* ROM region is `0x0000–0x7FFF`
* `load_rom()` copies up to `0x8000` bytes into ROM storage
* if provided ROM data is smaller than `0x8000`, unread portions return `0xFF`
* ROM reads return ROM bytes
* ROM writes are ignored

Do not implement bank switching or cartridge abstractions yet.

A simple fixed ROM window is enough.

---

### 3. RAM Behavior

Use the existing memory subsystem for RAM behavior.

Requirements:

* RAM region is `0xC000–0xFFFF`
* Bus reads/writes in this range map into the underlying memory subsystem
* address translation should be deterministic and explicit

Preferred translation:

* CPU-visible `0xC000–0xFFFF`
* mapped to memory offsets `0x0000–0x3FFF` inside RAM storage

If the existing `Memory` class currently models full 64KB flat memory, adjust carefully and minimally so that the bus can own the external address decode cleanly.

Do not over-engineer.

---

### 4. Open Bus / Unmapped Region

For addresses in:

```text
0x8000–0xBFFF
```

Behavior must be:

* reads return `0xFF`
* writes do nothing

This behavior must be covered by tests.

---

### 5. Reset Behavior

`Bus::reset()` should:

* clear RAM through the memory subsystem
* leave ROM contents intact
* preserve deterministic initial state

---

### 6. Unit Tests

Create:

```text
emulator/tests/unit/test_bus.cpp
```

Use the same simple standalone test style already established in the repository.

Required test cases:

#### Case A — ROM read

Load a small ROM buffer and verify reads in `0x0000–0x7FFF` return expected bytes.

Example:

* ROM data `{0xAA, 0xBB, 0xCC}`
* `read(0x0000) == 0xAA`
* `read(0x0001) == 0xBB`
* `read(0x0002) == 0xCC`

#### Case B — ROM unwritten area defaults to `0xFF`

Load a short ROM and verify unread ROM region beyond loaded data returns `0xFF`.

Example:

* ROM size = 3
* `read(0x0100) == 0xFF`

#### Case C — ROM writes ignored

Write to a ROM address and verify the original ROM value does not change.

#### Case D — RAM read/write

Verify writes to `0xC000` and above persist and can be read back.

Example:

* `write(0xC000, 0x42)`
* `read(0xC000) == 0x42`

#### Case E — RAM addresses independent

Verify two distinct RAM addresses retain separate values.

#### Case F — Unmapped read returns `0xFF`

Examples:

* `read(0x8000) == 0xFF`
* `read(0xBFFF) == 0xFF`

#### Case G — Unmapped write ignored

Write to unmapped region and verify it does not affect ROM or RAM state.

#### Case H — Reset clears RAM but preserves ROM

* load ROM
* write RAM
* call `reset()`
* verify RAM cleared to `0x00`
* verify ROM still returns original bytes

---

### 7. CMake Integration

Update:

```text
emulator/CMakeLists.txt
```

Add:

* `src/bus.cpp`
* `tests/unit/test_bus.cpp`

Create a new test executable, for example:

```text
super_z80_test_bus
```

Keep CMake changes minimal and consistent with the existing test setup.

---

### 8. Module Boundaries

The Bus module must not depend on:

* SDL2
* Z80ex
* main executable
* timing/scheduler code
* DMA or graphics/audio code

This must remain a narrow hardware-routing layer.

---

### 9. Update State Snapshot

Update:

```text
docs/state_snapshot.md
```

Record:

* current milestone: `M4`
* bus layer implemented
* initial CPU-visible memory map implemented
* unit tests passing
* next milestone: `M5 — CPU Wrapper and Z80ex Integration`

Keep the update concise.

---

## Constraints

* Do not integrate CPU yet.
* Do not add IO register behavior.
* Do not add bank switching.
* Do not create speculative cartridge abstractions.
* Do not add new dependencies.
* Keep the bus deterministic and minimal.
* Avoid refactoring unrelated files.

---

## Verification Commands

Run:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

If needed, also run the bus test executable directly and record the path used.

---

## Deliverables

The task is complete when all of the following exist:

* `emulator/include/bus.h`
* `emulator/src/bus.cpp`
* `emulator/tests/unit/test_bus.cpp`
* bus test target builds successfully
* all tests pass
* `docs/state_snapshot.md` updated
* completion report written

---

## Commit Requirements

Create commit:

```text
M4: add bus layer and initial memory map routing
```

---

## Completion Criteria

Mark the task complete only if:

* project builds successfully
* bus tests pass
* open-bus behavior is verified
* ROM/RAM routing is verified
* state snapshot is updated
* report is generated
* commit created successfully

---

## Report Requirements

Create:

```text
artifacts/reports/M4_bus_layer_report.md
```

Include:

* summary of files created/modified
* brief description of bus API
* memory map implemented
* test cases covered
* build output summary
* test execution results
* commit hash
* confirmation that future CPU integration can now route through the bus

---

## Expected Outcome

After M4, Super_Z80_v2 will have:

* a deterministic Bus layer
* the first CPU-visible memory map
* ROM/RAM routing behavior
* open-bus behavior for unmapped regions
* a clean architectural boundary for future Z80 integration

This milestone prepares the project for:

```text
M5 — CPU Wrapper and Z80ex Integration
```
