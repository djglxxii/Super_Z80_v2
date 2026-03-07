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

## Milestone Alignment Rule
When a task involves creating or advancing a milestone (especially any task that would produce a new M##_something_report.md file):

- The milestone identifier and scope **must** align with the **next expected milestone(s)** defined in docs/plan.md.
- "Next expected" means:
  - The lowest uncompleted M## from plan.md, or
  - The next logical sub-step if plan.md uses numbered sub-milestones (e.g. M11.1, M11.2).
- If the task proposes or would create a milestone that skips, duplicates, or significantly deviates from the current next item(s) in plan.md:
  1. **Do not proceed** with implementation or report generation.
  2. Immediately stop and report the conflict to the human composer, including:
     - The proposed milestone
     - The actual next expected milestone(s) from plan.md
     - A recommendation to either update plan.md first or adjust the task
- This rule takes precedence over proceeding with narrow-scope work when milestones are involved.

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
