Create this as the next active task packet.

Save as:

```
tasks/active/m29i_minimal_sdk_runtime_surface.md
```

---

# Task Packet — M29i Minimal SDK Runtime Surface

## Goal

Create the **minimum real SDK runtime surface** required for ROM development.

This milestone replaces the placeholder-only SDK directories with real, repository-truth SDK code and assets that ROMs can assemble against.

The SDK must provide a stable foundation for future ROM projects, including the Showcase ROM.

---

# Milestone

**M29i – Minimal SDK Runtime Surface**

Defined in `docs/plan.md` immediately after **M29h – ROM Execution Contract**.

---

# Why This Task Exists

The repository currently contains placeholder-only SDK directories:

```
sdk/inc/
sdk/runtime/inc/
sdk/runtime/src/
```

These contain `.gitkeep` files only and therefore do not provide:

* a runtime startup path
* public SDK include files
* font resources
* splash resources

Without these components, ROMs cannot assemble against the SDK.

This milestone establishes the **minimum real SDK foundation** required for ROM development.

---

# Scope

Implement the minimal SDK runtime surface needed for ROM assembly and boot.

The SDK must provide:

1. **Public SDK Include Surface**

Header / include files that ROMs can use.

Examples:

```
sdk/inc/super_z80.inc
sdk/inc/vdp.inc
sdk/inc/input.inc
```

These should contain:

* hardware register definitions
* basic macros for common operations
* documented include structure

Keep the surface small and stable.

---

2. **Runtime Startup Path**

Provide the minimal runtime needed to boot a ROM.

Expected structure:

```
sdk/runtime/inc/runtime.inc
sdk/runtime/src/reset.asm
sdk/runtime/src/runtime_init.asm
```

Responsibilities:

* CPU reset entry
* basic hardware initialization
* stack initialization
* jump to ROM main entry

Avoid adding high-level frameworks.

---

3. **Default Font Asset**

Provide a simple font tileset for text rendering.

Location example:

```
sdk/assets/font_8x8.asm
```

Requirements:

* 8×8 tile font
* ASCII-compatible layout
* deterministic binary representation

Do not implement text rendering routines yet.

---

4. **Splash Resource Contract**

Provide a minimal splash resource that ROMs may use.

Location example:

```
sdk/assets/splash_tiles.asm
sdk/assets/splash_tilemap.asm
```

Purpose:

* establish resource loading pattern
* provide example tilemap asset

Keep assets small and deterministic.

---

# Non-Goals

Do not implement:

* Showcase ROM
* text rendering engine
* sprite systems
* scrolling demos
* audio
* scene management
* asset pipelines
* emulator changes
* SDK abstractions for hypothetical future features

The goal is **only the minimal SDK surface**.

---

# Repository Constraints

Follow repository truth:

* adhere to `docs/plan.md`
* keep implementation deterministic
* do not modify emulator behavior
* do not introduce speculative abstractions

If repository assumptions conflict with SDK implementation, stop and report the conflict.

---

# Files Expected to Change

Expected additions:

```
sdk/inc/super_z80.inc
sdk/inc/vdp.inc
sdk/inc/input.inc

sdk/runtime/inc/runtime.inc

sdk/runtime/src/reset.asm
sdk/runtime/src/runtime_init.asm

sdk/assets/font_8x8.asm
sdk/assets/splash_tiles.asm
sdk/assets/splash_tilemap.asm

docs/showcase_state_snapshot.md
artifacts/reports/m29i_minimal_sdk_runtime_surface.md
```

Modify other files only if strictly required.

---

# Implementation Requirements

The SDK must enable the following minimal ROM structure:

```
rom/
  example_rom/
    src/main.asm
```

Where the ROM can include:

```
.include "sdk/inc/super_z80.inc"
.include "sdk/runtime/inc/runtime.inc"
```

And successfully assemble.

The runtime startup must:

1. initialize CPU state
2. configure minimal hardware state
3. jump to the ROM entry point

---

# Verification Commands

Run standard repository verification:

```
cmake -S . -B build
cmake --build build
ctest --test-dir build
```

SDK verification must also include:

1. assembling a minimal test ROM
2. executing the ROM through the emulator using the new ROM execution contract

Example command:

```
./build/super_z80 --rom build/test_sdk_rom.bin --headless --frames 2
```

Execution must succeed deterministically.

---

# Acceptance Criteria

This milestone is complete when:

* SDK directories contain real code and assets
* ROMs can assemble against SDK includes
* runtime startup path exists
* font asset exists
* splash asset exists
* minimal ROM assembles successfully
* ROM executes in emulator using `--rom`
* repository build/tests remain green
* completion report is written
* `docs/showcase_state_snapshot.md` is updated

---

# Deliverables

```
sdk/
  inc/
  runtime/
  assets/
```

plus:

```
artifacts/reports/m29i_minimal_sdk_runtime_surface.md
```

The report must include:

* summary
* files changed
* commands run
* verification results
* issues discovered
* next milestone recommendation

---

# Completion Report Path

```
artifacts/reports/m29i_minimal_sdk_runtime_surface.md
```

---

# Next Milestone

After successful completion of M29i, proceed to:

**M29j – SDK Integration Validation**

This milestone will build the first minimal Showcase ROM using the SDK runtime and verify it boots successfully in the emulator.
