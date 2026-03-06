# Super_Z80_v2 Getting Started

## Read First
Start with `docs/state_snapshot.md`, then `docs/plan.md`, `docs/spec.md`, `docs/architecture.md`, and `docs/decisions.md`.

## Authority Order
1. Active task packet in `tasks/active/`
2. `docs/state_snapshot.md`
3. `docs/plan.md`
4. `docs/spec.md`
5. `docs/architecture.md`
6. `docs/decisions.md`

## Task Packets
All work must be executed from explicit task packets. Keep scope narrow and restartable.

## Reports
Store completion reports in `artifacts/reports/` using the workflow report template.

## Verification
Future milestone tasks must define explicit commands with observable pass/fail output.

## Chat Authority
Chat is not authoritative. Repository files are the source of truth.
