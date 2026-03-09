# Task Packet

## Title

M27 — Platform Deterministic Validation

## Goal

Validate that the full Super_Z80 emulator platform operates deterministically and that all implemented subsystems interact correctly under scheduler-driven timing.

This milestone must verify correct integration of:

* CPU execution
* memory subsystem
* bus routing
* DMA engine
* IRQ controller
* VBlank interrupt source
* VDP rendering pipeline
* controller input
* audio subsystem
* scheduler timing

No new hardware features are introduced in this milestone.

---

# Scope

Included

* cross-subsystem integration tests
* deterministic execution validation
* end-to-end emulator stepping tests
* subsystem interaction verification
* headless emulator validation
* milestone report
* state snapshot update

Excluded

* new hardware features
* ROM showcase development
* graphical enhancements
* emulator performance tuning
* debugger tooling

---

# Files Expected to Change

```
emulator/CMakeLists.txt
docs/state_snapshot.md
```

---

# Files to Create

```
emulator/tests/integration/test_platform_determinism.cpp
emulator/tests/integration/test_cpu_dma_irq_integration.cpp
emulator/tests/integration/test_vdp_vblank_irq.cpp
emulator/tests/integration/test_input_audio_integration.cpp
artifacts/reports/M27_platform_validation_report.md
```

---

# Constraints

* Follow all authoritative documentation:

  * `docs/audio_spec.md`
  * `docs/vdp_baseline_v1.md`
  * `docs/state_snapshot.md`
  * `docs/plan.md`

* Do not change emulator behavior to make tests pass.

* Tests must run deterministically on CI.

* Tests must not depend on real SDL devices.

* Platform timing must remain owned by the **scheduler**, not host time.

* Do not introduce cross-threading or asynchronous behavior.

---

# Implementation Requirements

## 1. Full Emulator Determinism Test

Create a test verifying deterministic execution of the full emulator.

Procedure:

1. Initialize emulator with known memory state.
2. Execute a fixed number of scheduler ticks.
3. Capture platform state snapshot.

Repeat the same procedure twice.

Expected result:

* all platform state values must match.

State to compare may include:

```
CPU registers
VRAM
APU register state
DMA registers
IRQ state
scheduler tick counters
```

---

## 2. CPU + DMA + IRQ Integration

Create a test verifying correct interaction between:

* CPU execution
* DMA transfer
* IRQ triggering

Test must validate:

* DMA transfer updates memory correctly
* IRQ fires at correct timing boundary
* CPU resumes execution properly after IRQ

---

## 3. VDP + VBlank IRQ Integration

Create an integration test verifying:

* scanline progression
* VBlank interrupt generation
* IRQ controller interaction

Expected behavior:

* VBlank occurs at the correct scanline boundary
* IRQ controller receives the interrupt
* CPU interrupt vector is triggered deterministically

---

## 4. Input + Platform Interaction

Create a test validating controller input interaction with the platform.

Procedure:

1. Set controller state.
2. Execute platform ticks.
3. Verify bus reads reflect correct controller state.

Ensure deterministic behavior across repeated runs.

---

## 5. Audio + Platform Timing

Create an integration test verifying:

* audio generation advances with scheduler timing
* audio sample counts correspond to emulator ticks
* sample buffers behave deterministically

Tests must confirm that:

```
same emulator tick sequence → identical audio output
```

---

## Verification Commands

```
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Optional SDL validation:

```
env SDL_AUDIODRIVER=dummy SDL_VIDEODRIVER=dummy ./build/super_z80 --sdl-audio
```

---

# Deliverables

* platform integration tests
* deterministic execution validation
* passing build and test suite
* updated `docs/state_snapshot.md`
* completion report

```
artifacts/reports/M27_platform_validation_report.md
```

---

# Completion Criteria

The milestone is complete when:

* build succeeds
* all tests pass
* emulator execution is deterministic
* cross-subsystem interaction is verified
* no subsystem behavior was modified
* milestone report is written
* repository state snapshot updated

---

# Recommended Next Step

After M27, the emulator platform will be **fully feature-complete and validated**.

The next logical milestones would be:

```
M28 — Tooling & Debug Utilities
M29 — Performance Optimization
M30 — SDK / Development Documentation
```

These are optional depending on whether the project moves toward:

* ROM development
* emulator distribution
* development tooling for the platform.
