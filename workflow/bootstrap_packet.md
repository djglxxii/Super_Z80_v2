# Bootstrap Task Packet

## Title
Initialize Super_Z80_v2 bootstrap structure

## Goal
Set up a new Super_Z80_v2 repository with AWOS-style repository truth, Codex-focused instructions, workflow templates, and deterministic bootstrap verification.

## Scope
- Create required directory structure
- Create core docs under `docs/`
- Create workflow templates under `workflow/`
- Create root-level `AGENTS.md`, `README.md`, `Makefile`, and `.gitignore`
- Create bootstrap completion report
- Create initial bootstrap commit

## Out of Scope
- Emulator implementation
- Subsystem logic (CPU, memory, DMA, VDP, audio, input)
- SDL/frontend work
- CI setup and speculative tooling

## Constraints
- Keep documents concise and operational
- Do not add dependencies
- Do not perform unrelated refactors
- Keep implementation-first posture

## Verification Commands
- `git status --short`
- `find . -maxdepth 3 | sort`
- `make verify-bootstrap`
- `git rev-parse --short HEAD` (after commit)

## Deliverables
- Required repository structure and files
- Bootstrap report in `artifacts/reports/bootstrap_report.md`
- Initial commit: `Initialize Super_Z80_v2 bootstrap structure`
