# Task Packet — Reconcile Misclassified Showcase Controller-Input Work

## Goal

Apply repository cleanup so the unplanned controller-input Showcase work is no longer recorded as the official `M39` milestone.

The authoritative plan defines `M39` as **PSG Sound Effects**, not controller input. The Showcase milestone sequence explicitly lists `M38 – Metasprite Example` followed by `M39 – PSG Sound Effects`, then `M40 – YM2151 Music Playback`. 

Under AWOS, repository truth controls milestone identity, and conflicts between a task and repository truth must be corrected explicitly rather than silently accepted. 

## Scope

Perform only the cleanup needed to reconcile the milestone record.

Specifically:

1. Reclassify the completed controller-input Showcase work so it is **not** recorded as official `M39`.
2. Preserve the useful code changes already made.
3. Update repository documentation so the next official milestone remains:

   * `M39 – PSG Sound Effects`
4. Create a short reconciliation report documenting what was corrected and why.

## Non-Goals

Do not implement:

* PSG sound effects
* YM2151 music playback
* additional Showcase ROM features
* emulator changes
* SDK changes
* plan rewrites, unless absolutely required to correct a direct factual inconsistency

This is a **documentation and milestone-record reconciliation task only**.

## Repository Truth

Before editing, verify these facts from repository docs:

* `docs/plan.md` defines `M39 – PSG Sound Effects` as the next official Showcase milestone after `M38 – Metasprite Example`. 
* AWOS requires repository truth to govern execution and requires explicit correction when conflicts are found. 

## Files Expected to Change

Likely files:

* `artifacts/reports/m39_controller_input_demo.md`

  * rename or replace so it no longer claims to be official M39
* `docs/showcase_state_snapshot.md`
* `docs/state_snapshot.md`
* `artifacts/reports/showcase_milestone_reconciliation_controller_input.md`

If another file must change, explain why.

## Required Outcome

Reconcile the controller-input work using one of these acceptable approaches:

### Preferred approach

Rename/reclassify the work as an **unplanned enhancement** or **post-M38 enhancement**, for example:

* `SHOWCASE_CONTROLLER_INTERACTION_ENHANCEMENT`
* `M38a – Controller Interaction Enhancement`

Do **not** leave it recorded as `M39`.

### Snapshot updates

Update the state snapshots so they clearly say:

* controller-driven metasprite movement exists in the Showcase ROM
* it is **not** the official M39 milestone
* the next official milestone is still:

  * `M39 – PSG Sound Effects`

## Constraints

* Follow `docs/plan.md`
* Follow AWOS conflict-resolution rules
* Keep changes minimal
* Do not modify `docs/plan.md` unless you discover a direct factual contradiction that must be fixed
* Do not discard useful implemented code
* Do not rewrite history deceptively; record the correction honestly

## Verification Commands

Run at minimum:

```bash
git status --short
rg -n "M39|controller input|PSG Sound Effects|Metasprite" docs artifacts/reports rom/showcase -S
```

If the rename/update affects nothing executable, build/test is optional. If you touch executable code by mistake, then also run:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

## Acceptance Criteria

The task is complete only if:

* the controller-input work is no longer recorded as official `M39`
* repository docs clearly indicate the next official milestone is `M39 – PSG Sound Effects`
* the controller-input work remains documented as an enhancement or auxiliary Showcase improvement
* a reconciliation report is added
* no new Showcase features are implemented

## Deliverables

* corrected report naming/classification
* updated state snapshots
* reconciliation report:

```text
artifacts/reports/showcase_milestone_reconciliation_controller_input.md
```

## Completion Report Requirements

The report must include:

* summary
* files changed
* commands run
* verification results
* exact reconciliation performed
* next-step recommendation

## Final instruction

Before making edits, explicitly record in the report:

```text
Requested milestone in stale packet/report: M39 Controller Input Demo
Authoritative next milestone in docs/plan.md: M39 PSG Sound Effects
Alignment result: CONFLICT
Resolution: Reclassified controller-input work as non-milestone enhancement; preserved official M39 for PSG Sound Effects
```
