# Super_Z80_v2 Agent Instructions

## Mission

Super_Z80_v2 is a deterministic emulator and tooling project for a Z80-based retro console development environment. Optimize for correctness, determinism, maintainability, and small verifiable progress.

---

# Authority Order

When working, treat these as authoritative in this order:

1. tasks/active/<current task packet>
2. docs/state_snapshot.md
3. docs/plan.md
4. docs/spec.md
5. docs/architecture.md
6. docs/decisions.md

Chat history is not authoritative.

---

# Workflow Rules

* Work only from explicit task packets.
* Keep changes narrowly scoped.
* Do not invent features not requested by the task.
* Do not perform unrelated refactors.
* Stop and report if the task conflicts with repository truth.
* Prefer simple working implementations over speculative abstractions.

---

Revised section with the requested rule added. The change explicitly allows modification of `docs/plan.md` **only when a task packet explicitly instructs the agent to do so**, preserving the plan as the default authority.

---

# Milestone Alignment Rule

When a task involves creating or advancing a milestone (especially any task that would produce a new `M##_something_report.md` file):

* The milestone identifier and scope **must** align with the **next expected milestone(s)** defined in `docs/plan.md`.
* "Next expected" means:

  * The lowest uncompleted `M##` from `plan.md`, or
  * The next logical sub-step if `plan.md` uses numbered sub-milestones (e.g. M11.1, M11.2).

The coding agent **must treat `docs/plan.md` as authoritative** for milestone sequencing.

The agent **may not modify `docs/plan.md` during normal milestone execution.**

The **only exception** is when a task packet **explicitly instructs the agent to update or revise `docs/plan.md`**. In that case, the agent may perform the modification exactly as directed by the task.

If the task proposes or would create a milestone that skips, duplicates, or significantly deviates from the current next item(s) in `plan.md`:

1. **Do not proceed** with implementation or report generation.
2. Immediately stop and report the conflict to the human composer, including:

   * The proposed milestone
   * The actual next expected milestone(s) from `plan.md`
   * A recommendation to either update `plan.md` first or adjust the task

This rule takes precedence over proceeding with narrow-scope work when milestones are involved.

---

# Determinism Rules

* Preserve deterministic behavior.
* Prefer explicit verification commands.
* Do not claim completion without running verification when applicable.

---

# Documentation Rules

* Update repository truth when a task changes project state.
* Keep documentation concise and canonical.
* Do not create redundant docs unless the task explicitly requires them.

---

# Dependency Rules

* Do not add dependencies without explicit approval.
* Prefer vendored or already-approved dependencies.
* Third-party libraries may be used when justified for correctness and reliability.
* Do not reinvent solved problems in complex subsystems.
* Z80 CPU emulation should use **Z80ex** unless a decision record states otherwise.
* Platform shell functionality is expected to use **SDL2**.
* Any additional dependency must be justified and logged in `docs/decisions.md`.
* Avoid unnecessary dependency growth.
* Keep the build surface minimal.

---

# Reporting Rules

* Every task must produce a completion report in `artifacts/reports/`.
* Report status as **Completed**, **Partially Completed**, or **Blocked**.
* Include:

  * files changed
  * commands run
  * verification results
  * next-step recommendations

---

# Git Operations Rule

After completing work for an active task packet, the agent **must perform repository housekeeping operations** to preserve project state.

The required sequence is:

### 1. Stage All Changes

```
git add .
```

### 2. Create Commit

Create a commit summarizing the completed task.

The commit message should:

* reference the task or milestone when applicable
* summarize the work performed
* be concise and descriptive

Example:

```
git commit -m "M24: Implement controller input hardware and update tests"
```

### 3. Move Completed Task Packet

After a successful commit:

Move the task packet from:

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

### 4. Commit Task State Update

The task relocation must also be committed:

```
git add tasks/completed/<task_file>.md
git rm tasks/active/<task_file>.md
git commit -m "Move completed task packet <task_file> to tasks/completed"
```

### 5. Repository State After Task Completion

After these steps:

* All changes produced by the task must be committed.
* The completed task packet must reside in `tasks/completed/`.
* The repository should be in a **clean working state** (`git status` shows no pending changes).

Agents must **not leave uncommitted work** after completing a task.

---

# Build and Verify

During bootstrap, verification is limited to:

* file/directory existence
* `git status`

Future implementation tasks must define explicit build/test commands.
