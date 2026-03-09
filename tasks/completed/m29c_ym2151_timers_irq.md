# Task Packet

## Title

M29c — YM2151 Timers, Status Bits, and IRQ Behavior

## Goal

Implement deterministic YM2151 timer behavior, status reporting, and IRQ-facing behavior on top of the existing register interface and channel/operator state model.

This milestone adds the chip’s timer and status machinery, but still does **not** add FM sample generation or mixer integration.

## Scope

Included:

* YM2151 timer A state
* YM2151 timer B state
* timer control register behavior
* status bit behavior
* IRQ request behavior from timer overflow
* deterministic timer progression from scheduler-owned ticking
* unit tests
* milestone report
* state snapshot update

Excluded:

* FM waveform generation
* operator envelope/audio synthesis
* PSG/YM mixer integration
* SDL audio changes
* ROM work
* speculative chip features outside the agreed YM2151 scope

## Files Expected to Change

* `emulator/audio/ym2151/ym2151.h`
* `emulator/audio/ym2151/ym2151.cpp`
* `emulator/src/bus.cpp`
* `emulator/include/bus.h`
* `emulator/src/scheduler.cpp`
* `emulator/include/scheduler.h`
* `emulator/tests/unit/test_ym2151.cpp`
* `docs/state_snapshot.md`

## Files to Create

* `artifacts/reports/m29c_ym2151_timers_irq.md`

## Constraints

* Preserve deterministic behavior.
* Timer progression must depend only on explicit emulator ticking.
* Do not make SDL or host wall-clock time authoritative.
* Do not introduce FM audio output yet.
* Do not modify PSG behavior.
* If any YM2151 timer detail is ambiguous in current repository truth, implement the minimal deterministic model required for future FM milestones and document it clearly in the report.

## Implementation Requirements

### 1. Timer State

Extend YM2151 state to include at minimum:

* Timer A latch / counter
* Timer B latch / counter
* timer enabled flags
* timer overflow flags
* IRQ enable bits for each timer
* status register bits derived from timer overflow state

### 2. Register Mapping

Implement deterministic handling for the agreed YM2151 timer-related registers through the existing `0x70-0x71` interface.

Required behaviors:

* writing timer latch registers updates stored timer values
* writing timer control updates enable/reset/mask behavior
* timer overflow updates status bits
* status reflects timer overflow state until cleared by the defined control path

Raw register storage from earlier milestones must remain intact.

### 3. Tick Progression

Extend `YM2151::tick(uint32_t cycles)` so timers advance deterministically with scheduler-driven cycles.

Requirements:

* disabled timers do not advance
* enabled timers count toward overflow
* overflow sets status bits
* repeated runs with identical cycle sequences produce identical timer state

### 4. IRQ-Facing Behavior

Add a deterministic IRQ-facing signal from the YM2151 device.

Acceptable model:

* `irq_pending()` or equivalent accessor on YM2151
* bus-visible aggregation hook if needed for future integration

Requirements:

* IRQ pending when an enabled timer overflows and IRQ for that timer is enabled
* no direct CPU interrupt delivery beyond the existing emulator interrupt architecture
* this milestone may stop at exposing the pending signal if full routing is planned for a later step

### 5. Status Behavior

Implement status read behavior appropriate to current repository truth.

At minimum:

* timer overflow bits visible
* bits clear only through the defined control/reset behavior
* deterministic readback

## Unit Test Requirements

Extend `emulator/tests/unit/test_ym2151.cpp` to cover at minimum:

1. **Timer register write/read storage**

   * writing latch registers updates device state correctly

2. **Disabled timer does not advance**

   * tick with timer disabled leaves state unchanged

3. **Enabled timer advances**

   * tick with timer enabled changes countdown/progression deterministically

4. **Overflow sets status**

   * timer overflow sets the appropriate status bit

5. **IRQ pending behavior**

   * overflow with timer IRQ enabled sets IRQ pending
   * overflow with IRQ disabled does not set IRQ pending

6. **Status clear/control behavior**

   * defined control path clears overflow/status as expected

7. **Repeatability**

   * identical setup plus identical tick sequence yields identical timer state and status

## Verification Commands

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure --tests-regex "super_z80_test_(ym2151|scheduler|bus)"
ctest --test-dir build --output-on-failure
```

## Deliverables

* deterministic YM2151 timers
* YM2151 status bits for timer overflow
* IRQ-facing pending behavior
* passing unit tests
* updated `docs/state_snapshot.md`
* completion report at `artifacts/reports/m29c_ym2151_timers_irq.md`

## Completion Criteria

The task is complete when:

* YM2151 timer state exists and advances deterministically
* timer overflow updates status bits correctly
* IRQ-facing pending behavior exists and is test-covered
* all tests pass
* no FM sample generation was added
* milestone report is written
* state snapshot is updated

## Report Template

The completion report must include:

* summary
* files changed
* commands run
* verification results
* timer/status behavior implemented
* issues encountered
* recommended next step

## Recommended Next Step

M29d — YM2151 FM Sample Generation
