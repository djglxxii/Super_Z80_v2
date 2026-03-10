# Super_Z80_v2 Agent Instructions

## Mission

Super_Z80_v2 is a deterministic emulator and tooling project for a Z80-based retro console development environment.

Agents must optimize for:

* correctness
* determinism
* maintainability
* small, verifiable progress

---

# Execution Checklist (Mandatory Before Any Work)

Before performing any implementation work, the agent **must complete the following checklist in order**:

1. **Read `docs/plan.md`** to determine the next expected milestone.
2. **Read the active task packet** in `tasks/active/`.
3. **Perform the Milestone Preflight Check** to verify milestone alignment.
4. **Validate the task packet against repository truth** (`docs/state_snapshot.md` and `docs/plan.md`).
5. **Confirm determinism constraints** will not be violated by the planned work.

The agent must **not begin implementation until this checklist is complete**.

If any step fails or reveals a conflict, the agent must **stop and report the issue to the human composer**.

---

# Authority Order

When working, treat these sources as authoritative in this order:

1. `tasks/active/<current task packet>`
2. `docs/state_snapshot.md`
3. `docs/plan.md`
4. `docs/spec.md`
5. `docs/architecture.md`
6. `docs/decisions.md`

Chat history is **not authoritative**.

---

# Workflow Rules

* Work **only** from explicit task packets.
* Keep changes **narrowly scoped** to the task.
* Do **not invent features** not requested by the task.
* Do **not perform unrelated refactors**.
* Prefer **simple working implementations** over speculative abstractions.
* Stop and report if the task conflicts with repository truth.

---

# Task Packet Validation Rule

Before performing any implementation work, the agent must validate that the active task packet is consistent with repository truth.

The agent **must not assume that a file in `tasks/active/` is correct or current**.

The agent must verify:

1. The task packet milestone identifier.
2. The next expected milestone from `docs/plan.md`.
3. That the task packet does not skip or contradict repository sequencing.

If the task packet conflicts with repository truth, the agent must **stop and report the mismatch** before performing any work.

---

# Milestone Alignment Rule

When a task involves creating or advancing a milestone (especially any task that would produce a new `M##_something_report.md` file):

* The milestone identifier and scope **must align with the next expected milestone(s)** defined in `docs/plan.md`.

"Next expected" means:

* The **lowest uncompleted `M##` milestone** in `docs/plan.md`, or
* The **next logical sub-step** if sub-milestones exist (e.g. `M11.1`, `M11.2`).

The coding agent **must treat `docs/plan.md` as authoritative** for milestone sequencing.

---

## Mandatory Milestone Preflight Check

Before implementing any task packet referencing a milestone, the agent **must perform an explicit milestone verification**.

The agent must:

1. Read `docs/plan.md`.
2. Determine the **next expected milestone**.
3. Extract the **milestone identifier from the active task packet**.
4. Compare the two values **before making any repository changes**.

The result must be recorded in the task output or report using the following format:

```
TASK_PACKET_MILESTONE: <value>
NEXT_EXPECTED_MILESTONE_FROM_PLAN: <value>
ALIGNMENT: PASS | FAIL
```

### If Alignment Fails

If the milestone values do not match:

```
TASK_PACKET_MILESTONE: <value>
NEXT_EXPECTED_MILESTONE_FROM_PLAN: <value>
ALIGNMENT: FAIL
REASON: milestone mismatch with repository truth
```

The agent must then:

* **Stop immediately**
* **Make no repository changes**
* **Report the conflict to the human composer**

A milestone appearing in a task packet **is not sufficient authority** by itself.

`docs/plan.md` remains the **canonical milestone source**.

---

# Plan Modification Rule

The agent **may not modify `docs/plan.md` during normal milestone execution**.

The **only exception** is when a task packet **explicitly instructs the agent to update or revise `docs/plan.md`**.

If such an instruction exists:

* The change must be performed **exactly as directed**.
* The modification must be **documented in the task completion report**.

---

# Conflict Handling

If the task proposes or would create a milestone that:

* skips milestones
* duplicates milestones
* changes milestone meaning
* contradicts `docs/plan.md`

The agent must:

1. **Stop execution**.
2. Report the conflict to the human composer.
3. Include:

   * the proposed milestone
   * the expected milestone from `docs/plan.md`
   * the nature of the conflict
   * a recommended correction.

This rule **takes precedence over continuing work**.

---

# Determinism Rules

The project prioritizes deterministic behavior.

Agents must:

* preserve deterministic emulator behavior
* avoid introducing nondeterministic execution paths
* prefer explicit deterministic logic
* run verification commands when applicable

Agents must **not claim completion without verification** when the task defines verification steps.

---

# Documentation Rules

When a task changes project state:

* repository truth **must be updated**
* documentation must remain **canonical and concise**

Agents must:

* update `docs/state_snapshot.md` when system state changes
* avoid redundant documentation unless explicitly required by the task
* maintain consistency with existing documentation structure

---

# Dependency Rules

Dependencies must remain minimal.

Agents must:

* not add dependencies without explicit approval
* prefer vendored or already-approved dependencies
* avoid unnecessary dependency growth

Third-party libraries may be used **when justified for correctness and reliability**.

Current expectations:

* Z80 CPU emulation should use **Z80ex** unless a decision record states otherwise.
* Platform shell functionality should use **SDL2**.

Any additional dependency must:

* be justified
* be recorded in `docs/decisions.md`.

---

# Reporting Rules

Every task must produce a completion report in:

```
artifacts/reports/
```

Reports must contain:

* task status

  * Completed
  * Partially Completed
  * Blocked

* files changed

* commands run

* verification results

* next-step recommendation

Reports should be **clear, concise, and deterministic**.

---

# Git Operations Rule

After completing work for an active task packet, the agent **must perform repository housekeeping**.

## 1. Stage All Changes

```
git add .
```

## 2. Create Commit

Create a commit summarizing the completed work.

The commit message should:

* reference the task or milestone when applicable
* summarize the work performed
* be concise

Example:

```
git commit -m "M24: Implement controller input hardware and update tests"
```

---

## 3. Move Completed Task Packet

After committing work, move the task packet from:

```
tasks/active/
```

to:

```
tasks/completed/
```

Example:

```
mv tasks/active/<task_file>.md tasks/completed/
```

---

## 4. Commit Task Packet Relocation

```
git add tasks/completed/<task_file>.md
git rm tasks/active/<task_file>.md
git commit -m "Move completed task packet <task_file> to tasks/completed"
```

---

## 5. Repository State After Completion

After the above steps:

* All task changes must be committed.
* The completed task packet must reside in `tasks/completed/`.
* The repository must be in a **clean working state**.

Verification:

```
git status
```

should show **no pending changes**.

Agents must **never leave uncommitted work** after completing a task.

---

# Build and Verify

During bootstrap stages, verification may be limited to:

* file or directory existence
* repository state checks (`git status`)

Future implementation tasks must define explicit **build and test commands**.

Agents must run the defined verification steps **before reporting completion**.
