# Task Packet

## Title

M24 — Controller Input Hardware

## Goal

Implement deterministic controller input hardware through the emulator bus so PAD1 and PAD1_SYS become CPU-visible and usable by both tests and the SDL shell.

## Scope

Included:

* controller input state model
* bus/I-O port wiring for controller reads
* deterministic default controller state
* SDL keyboard/gamepad mapping into controller state
* unit tests for controller reads and button state transitions
* milestone report
* state snapshot update

Excluded:

* input recording/replay redesign
* analog input
* light gun / special peripherals
* gameplay ROM work
* audio changes
* VDP changes

## Files Expected to Change

* `emulator/include/bus.h`
* `emulator/src/bus.cpp`
* `emulator/include/io.h`
* `emulator/src/io.cpp`
* `emulator/src/main.cpp` or SDL shell file currently responsible for host input integration
* `emulator/CMakeLists.txt`
* `docs/state_snapshot.md`

## Files to Create

* `emulator/tests/unit/test_input.cpp`
* `artifacts/reports/M24_controller_input_report.md`

## Constraints

* Follow repository truth and current completed milestone state.
* Keep scope narrow to controller input only.
* Do not add new dependencies.
* Do not refactor unrelated emulator subsystems.
* Preserve deterministic behavior.
* Default unpressed controller state must be explicitly defined and tested.
* Keep host input handling outside core emulation logic as much as practical.
* If the current codebase already has partial input plumbing, extend it rather than replacing it.
* If a repository truth conflict is found, stop and report it instead of silently reinterpreting the task.

## Implementation Requirements

1. Define a deterministic controller state model for:

   * PAD1
   * PAD1_SYS

2. Expose controller reads through the CPU-visible bus/I-O path.

3. Ensure unpressed input returns the correct idle value consistently.

4. Add SDL host mapping for at least:

   * Up
   * Down
   * Left
   * Right
   * A
   * B
   * Start

5. Ensure unit tests cover:

   * default idle state
   * individual button press transitions
   * bus-visible read behavior
   * simultaneous button combinations where valid

6. Update the project state snapshot to record M24 completion.

## Verification Commands

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## Deliverables

* Controller input hardware implemented and readable by the emulator core
* SDL host input mapped into emulator controller state
* Passing unit tests
* Updated `docs/state_snapshot.md`
* Completion report at `artifacts/reports/M24_controller_input_report.md`

## Completion Criteria

The task is complete when:

* build succeeds
* all tests pass
* PAD1/PAD1_SYS are readable through the intended hardware path
* idle and pressed states are deterministic
* milestone report is written
* state snapshot is updated

## Report Template

The completion report must include:

* summary
* files changed
* files created
* commands executed
* verification results
* issues encountered
* recommended next step
