# Super_Z80_v2 Execution Plan

## Strategy
Build the project in small deterministic slices. Start with repository bootstrap, then the minimal emulator skeleton, then subsystem-level increments with explicit verification.

## Milestones
- M0: Repository bootstrap and project truth setup
- M1: Emulator skeleton and build system
- M2: CRC utilities
- M3: Memory subsystem
- M4: Input subsystem
- M5: DMA subsystem
- M6: VDP core
- M7: I/O port routing
- M8: CPU wrapper
- M9: Emulator coordinator
- M10: Headless runner
- M11: Regression harness
- M12: SDL shell
- M13: Replay/snapshot parity

## Verification Strategy
Each milestone must define explicit commands and observable pass/fail output.

## Rollback Strategy
Incorrect milestone work must be reversible through git history and narrow task boundaries.

## Planning Rule
Do not expand milestone scope without updating repository truth first.
