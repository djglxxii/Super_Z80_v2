# Next Task Packet

## Title

M29a — YM2151 Register Interface

## Objective

Implement the **YM2151 communication interface** inside the emulator so the CPU can communicate with the FM chip using the classic **address/data register model**.

This milestone **does not generate audio yet**.
It only establishes the **register interface and internal register storage**.

---

# Scope

Included

* YM2151 device skeleton
* register storage
* address/data port interface
* integration with CPU I/O bus
* deterministic scheduling hooks

Excluded

* FM synthesis
* operator algorithms
* timers
* IRQ behavior
* audio mixing

Those are implemented in later milestones.

---

# Hardware Model

The YM2151 uses a **two-port interface**.

| Port | Function        |
| ---- | --------------- |
| 0x70 | register select |
| 0x71 | register data   |

Communication sequence:

```
OUT (0x70), register_number
OUT (0x71), register_value
```

The emulator must reproduce this model exactly.

---

# Implementation Requirements

## 1. Create Device

Create a new device module.

Expected structure:

```
emulator/
  audio/
    ym2151/
      ym2151.h
      ym2151.cpp
```

Device state should include:

```
selected_register
registers[256]
```

Registers are stored but not yet interpreted.

---

## 2. Port Map

Add I/O mapping.

```
0x70 → YM2151 register select
0x71 → YM2151 register write
```

Behavior:

### Port 0x70

```
selected_register = value
```

### Port 0x71

```
registers[selected_register] = value
```

---

## 3. Bus Integration

Hook the YM2151 device into the emulator I/O dispatch.

Expected structure:

```
IOBus
 ├─ PSG
 └─ YM2151
```

The device must respond only to ports **0x70–0x71**.

---

## 4. Deterministic Scheduler Hook

Register the device with the emulator scheduler so future FM timing logic can be inserted.

For now this function may be empty but must exist.

Example:

```
void YM2151::tick(uint32_t cycles)
```

---

## 5. Debug Logging (Optional)

Add debug output for register writes.

Example:

```
YM2151: REG 0x20 ← 0x7F
```

This helps validation later.

---

# Documentation Update

After implementation update:

```
docs/state_snapshot.md
```

Add entry:

```
M29a complete
YM2151 register interface implemented
No FM synthesis yet
```

---

# Verification Tests

Create a simple test ROM or unit test that performs:

```
OUT (0x70), 0x20
OUT (0x71), 0x7F
```

Verify:

```
ym2151.registers[0x20] == 0x7F
```

Also verify:

* port 0x70 changes selected register
* port 0x71 writes to that register

---

# Deliverables

Source files:

```
emulator/audio/ym2151/*
```

Documentation:

```
docs/state_snapshot.md
```

Report:

```
artifacts/reports/m29a_ym2151_register_interface.md
```

---

# Completion Criteria

The milestone is complete when:

* YM2151 responds to ports **0x70–0x71**
* register writes are stored correctly
* emulator compiles and runs
* no audio output required yet
* PSG functionality unchanged

---

# What Happens Next

After this milestone:

```
M29b — FM Operator and Channel State
```

That milestone begins implementing the **actual FM synthesis model**.

---
