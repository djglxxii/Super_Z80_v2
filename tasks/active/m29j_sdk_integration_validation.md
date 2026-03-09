# Task Packet — M29j SDK Integration Validation

## Goal

Validate that a ROM built against the repository SDK runtime boots successfully in the emulator.

This milestone proves that the SDK runtime surface implemented in M29i is sufficient to support ROM development.

---

# Milestone

**M29j – SDK Integration Validation**

---

# Scope

Create a minimal SDK-backed ROM that:

* includes the SDK runtime
* boots through `reset.asm`
* initializes minimal hardware
* renders a simple splash or text message
* executes deterministically through the emulator

---

# Non-Goals

Do not implement:

* Showcase ROM scenes
* scrolling demos
* sprites
* audio demonstrations
* menus
* scene engines
* gameplay logic

Only validate **SDK runtime integration**.

---

# Files Expected to Change

Expected additions:

```
rom/showcase/
rom/showcase/src/main.asm
rom/showcase/Makefile
```

Possible modifications:

```
docs/showcase_state_snapshot.md
artifacts/reports/m29j_sdk_integration_validation.md
```

Avoid modifying SDK files unless an integration defect is discovered.

---

# Implementation Requirements

The ROM must assemble using the SDK includes:

```
.include "sdk/inc/super_z80.inc"
.include "sdk/runtime/inc/runtime.inc"
```

Runtime flow:

```
reset.asm
 → runtime_init.asm
 → ROM entry point
```

The ROM entry point should:

1. clear tilemap
2. load font
3. print a simple message

Example message:

```
SUPER Z80 SDK
BOOT VALIDATED
```

Rendering logic should remain extremely simple.

---

# Verification Commands

Run repository verification:

```
cmake -S . -B build
cmake --build build
ctest --test-dir build
```

Assemble the Showcase validation ROM:

```
sjasmplus --nologo -I . --raw=build/showcase_validation.bin rom/showcase/src/main.asm
```

Run ROM through emulator:

```
./build/super_z80 --rom build/showcase_validation.bin --headless --frames 4
```

Repeat the command to verify deterministic output.

---

# Acceptance Criteria

The milestone is complete when:

* ROM builds successfully using SDK includes
* ROM boots via SDK runtime
* ROM executes through emulator
* ROM reaches deterministic output state
* headless verification output is stable
* repository build/tests remain green
* completion report is written
* `docs/showcase_state_snapshot.md` updated

---

# Deliverables

```
rom/showcase/
```

and report:

```
artifacts/reports/m29j_sdk_integration_validation.md
```

---

# Completion Report Requirements

The report must include:

* summary
* files changed
* commands run
* verification results
* issues discovered
* next milestone recommendation

---

# Next Milestone

After M29j completes:

**M30 – Showcase ROM Project Scaffold**

This begins actual Showcase ROM development.

---
