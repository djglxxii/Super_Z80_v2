# Task Packet — M29h ROM Execution Contract

## Goal

Implement the minimum emulator support required to load and execute an external ROM artifact through the current emulator CLI and deterministic verification flow.

This task establishes the execution foundation required for all Showcase ROM work and must align with the current `docs/plan.md` milestone ordering.

## Milestone

**M29h – ROM Execution Contract**

## Why This Task Exists

Showcase development is currently blocked because repository truth does not yet provide:

* an emulator CLI path that accepts an external ROM
* a headless ROM execution flow
* an external ROM loading contract in the emulator core

Under AWOS, this prerequisite must be implemented before SDK-backed ROM integration or Showcase ROM validation can proceed. 

## Scope

Implement only the minimum functionality needed to:

* accept an external ROM path via the emulator CLI
* load ROM bytes from disk
* map the ROM into the existing emulator boot/execution path
* run the ROM through the emulator without requiring SDL input/audio
* support a deterministic, non-interactive execution flow suitable for later ROM milestone verification
* document the exact command used to run an external ROM

## Non-Goals

Do not implement:

* Showcase ROM content
* SDK runtime assets
* splash screens
* fonts
* text rendering
* scene management
* audio feature work
* new hardware behavior
* new emulator architecture unrelated to ROM loading
* refactors for future flexibility
* debugger enhancements unless strictly required for ROM execution

## Repository Truth and Ordering Constraints

* Follow the current `docs/plan.md` milestone order
* Do not skip ahead to SDK integration validation
* Do not create Showcase ROM runtime assumptions that are not backed by repository code
* If external ROM execution reveals a genuine hardware or emulator contract bug, report it clearly

## Files Expected to Change

Likely files include only those needed for ROM loading and execution contract support, for example:

* emulator CLI entrypoint files
* emulator ROM-loading / cartridge-loading path
* minimal configuration or argument parsing files
* tests or verification scripts related to external ROM execution
* `docs/showcase_state_snapshot.md`
* `artifacts/reports/m29h_rom_execution_contract.md`

Only modify other files if required, and explain why.

## Implementation Requirements

Implement a minimal, restartable execution contract for external ROMs:

1. **CLI ROM Path Support**

   * Add support for passing an external ROM path to the emulator executable
   * Preserve existing flags and behavior where possible

2. **ROM File Loading**

   * Load a ROM artifact from disk
   * Validate failure behavior for missing/unreadable ROM input
   * Keep behavior deterministic

3. **EmulatorCore / Execution Path Integration**

   * Route the loaded ROM into the normal emulator execution path
   * Avoid hardcoding Showcase-specific behavior
   * Avoid introducing a second unrelated boot path

4. **Deterministic Verification Path**

   * Provide a non-interactive run mode suitable for verifying external ROM execution
   * This may be headless or existing minimal-output execution, but it must be scriptable and deterministic

5. **Documentation of Contract**

   * Record the command used to execute an external ROM
   * Update Showcase state snapshot with milestone status and next dependency

## Verification Commands

Run the standard repository verification commands where applicable:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build
```

Also run ROM execution verification using an external ROM artifact from disk.

At minimum, verification must demonstrate:

* emulator accepts a ROM path
* emulator runs the ROM without crashing
* repeated runs are deterministic under the same command

If a temporary minimal ROM artifact is required to validate the path, keep it as small as possible and explain exactly how it is produced and used.

## Acceptance Criteria

This task is complete only if all of the following are true:

* the emulator accepts an external ROM path
* an external ROM artifact can be loaded from disk
* the emulator executes that ROM through the normal runtime path
* there is a deterministic command for non-interactive ROM execution
* standard build/tests still pass
* a completion report is written
* `docs/showcase_state_snapshot.md` is updated
* no SDK integration work is performed yet
* no unrelated emulator refactors are introduced

## Deliverables

* emulator support for external ROM execution
* deterministic ROM execution command documented in the report
* completion report containing:

  * summary
  * files changed
  * commands run
  * verification results
  * issues discovered
  * recommendation for the next milestone

## Completion Report Path

```text
artifacts/reports/m29h_rom_execution_contract.md
```

## Next Milestone After Completion

If M29h completes successfully, the next task packet should target:

**M29i – Minimal SDK Runtime Surface**

---
