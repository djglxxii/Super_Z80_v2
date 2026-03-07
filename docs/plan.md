# Super_Z80_v2 Execution Plan

## Strategy
Build the project in small deterministic slices. Start with repository bootstrap, then the minimal emulator skeleton, then subsystem-level increments with explicit verification.

## Milestones
- M0: Repository bootstrap and project truth setup
- M1: Emulator skeleton and build system
- M2: CRC utilities
- M3: Memory subsystem
- M4: Bus layer
- M5: CPU wrapper
- M6: I/O port routing
- M7: Scheduler and timing system
- M8: IRQ controller
- M9: VBlank interrupt source
- M10: DMA Engine
- M11: Video pipeline
- M12: Audio subsystem
- M13: Emulator coordinator
- M14: Headless runner
- M15: Regression harness
- M16: SDL shell
- M17: Replay/snapshot parity

## Verification Strategy
Each milestone must define explicit commands and observable pass/fail output.

## Rollback Strategy
Incorrect milestone work must be reversible through git history and narrow task boundaries.

## Planning Rule
Do not expand milestone scope without updating repository truth first.
