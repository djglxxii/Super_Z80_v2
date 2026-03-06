# Super_Z80_v2 Specification

## User Problem
Super_Z80_v2 needs a clean, deterministic, modular reboot of the earlier emulator/project approach so that long-running agent-assisted development can proceed safely with durable repository memory and small verifiable increments.

## Goals
- G1: Build a deterministic, modular emulator/tooling codebase.
- G2: Support stateless agent-driven development through repository truth.
- G3: Favor implementation-first progress over speculative architecture.
- G4: Preserve strong verification discipline from the beginning.

## Non-Goals
- NG1: Do not design unimplemented future abstractions prematurely.
- NG2: Do not create a broad asset pipeline during bootstrap.
- NG3: Do not implement emulator features during bootstrap.
- NG4: Do not optimize for multi-agent orchestration.

## Acceptance Criteria
- AC1: Repository bootstrap structure exists.
- AC2: Canonical project truth docs exist.
- AC3: Codex CLI persistent instruction file exists.
- AC4: Bootstrap state is committed and reported.

## Interfaces
Initial interfaces are not yet defined. They will emerge through implementation milestones.

## Data Model
Initial project state is document-driven only. Implementation data models are deferred to milestone work.
