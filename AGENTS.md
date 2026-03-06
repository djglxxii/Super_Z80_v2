# Super_Z80_v2 Agent Instructions

## Mission
Super_Z80_v2 is a deterministic emulator and tooling project for a Z80-based retro console development environment. Optimize for correctness, determinism, maintainability, and small verifiable progress.

## Authority Order
When working, treat these as authoritative in this order:
1. tasks/active/<current task packet>
2. docs/state_snapshot.md
3. docs/plan.md
4. docs/spec.md
5. docs/architecture.md
6. docs/decisions.md

Chat history is not authoritative.

## Workflow Rules
- Work only from explicit task packets.
- Keep changes narrowly scoped.
- Do not invent features not requested by the task.
- Do not perform unrelated refactors.
- Stop and report if the task conflicts with repository truth.
- Prefer simple working implementations over speculative abstractions.

## Determinism Rules
- Preserve deterministic behavior.
- Prefer explicit verification commands.
- Do not claim completion without running verification when applicable.

## Documentation Rules
- Update repository truth when a task changes project state.
- Keep documentation concise and canonical.
- Do not create redundant docs unless the task explicitly requires them.

## Dependency Rules
- Do not add dependencies without explicit approval.
- Prefer vendored or already-approved dependencies.
- Third-party libraries may be used when justified for correctness and reliability.
- Do not reinvent solved problems in complex subsystems.
- Z80 CPU emulation should use Z80ex unless a decision record states otherwise.
- Platform shell functionality is expected to use SDL2.
- Any additional dependency must be justified and logged in docs/decisions.md.
- Avoid unnecessary dependency growth.
- Keep the build surface minimal.

## Reporting Rules
- Every task must produce a completion report in artifacts/reports/.
- Report status as Completed, Partially Completed, or Blocked.
- Include files changed, commands run, verification results, and next-step recommendations.

## Build and Verify
During bootstrap, verification is limited to file/directory existence and git status.
Future implementation tasks must define explicit build/test commands.
