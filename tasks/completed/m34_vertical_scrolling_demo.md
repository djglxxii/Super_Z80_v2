# Task Packet — M34 Vertical Scrolling Demo

## Goal

Extend the Showcase ROM to demonstrate deterministic **vertical scrolling** using the existing Showcase scaffold, splash/text composition path, and scrolling main loop established in prior milestones.

This milestone must validate the Super_Z80 vertical scroll registers in a clear, reusable way for future ROM developers.

## Milestone

**M34 – Vertical Scrolling Demo**

## Scope

Implement vertical background scrolling in `rom/showcase/`.

The ROM should:

- maintain RAM-backed scroll state for vertical movement
- update vertical scroll once per frame in the main loop
- write the vertical scroll value to the VDP background vertical scroll register during render
- preserve the existing horizontal scrolling demonstration unless a simpler combined presentation is clearer
- remain deterministic under repeated headless execution

This milestone is about **vertical scrolling only**.

## Non-Goals

Do not implement:

- parallax scrolling
- foreground-plane independent motion
- sprites
- sprite animation
- input-driven movement
- audio
- scene manager
- menu systems
- generalized rendering abstractions
- SDK changes unless a real defect is discovered

## Files Expected to Change

Likely files:

- `rom/showcase/inc/memory_map.inc`
- `rom/showcase/src/init.asm`
- `rom/showcase/src/vdp_init.asm`
- `rom/showcase/src/main_loop.asm`
- `rom/showcase/src/text.asm`
- `docs/showcase_state_snapshot.md`
- `docs/state_snapshot.md`
- `artifacts/reports/m34_vertical_scrolling_demo.md`

Modify other files only if required and explain why.

## Constraints

- Follow `docs/plan.md` milestone ordering and repository truth
- Keep scope narrow and deterministic
- Do not skip ahead to parallax or sprite work
- Do not refactor unrelated ROM structure
- Do not modify emulator architecture or hardware behavior unless a real bug is found
- Preserve the reusable text path and existing Showcase scaffold clarity

## Implementation Requirements

### 1. Vertical Scroll State

Maintain vertical scroll state in RAM.

At minimum:

- `scroll_y` state exists
- it is initialized deterministically
- it increments each frame or otherwise changes deterministically

### 2. VDP Vertical Scroll Register Usage

Write the vertical scroll value each frame through the correct background vertical scroll register.

This should happen in the render/update portion of the main loop, not as a one-time init-only write.

### 3. Visible Demonstration

The Showcase ROM must visibly demonstrate vertical motion.

It may:

- scroll the existing splash-derived tilemap vertically, or
- use a derived/background tilemap better suited to visible vertical movement

If a larger or more obviously scrollable tilemap is needed, keep it minimal and deterministic.

### 4. Preserve Existing Composition

Do not break:

- boot-to-splash behavior
- text rendering path from M31
- deterministic horizontal scroll behavior from M33, unless the implementation intentionally replaces it with a cleaner combined X/Y scrolling demo and explains the reason in the report

### 5. Developer-Reference Clarity

The resulting code should remain easy for ROM developers to study:

- obvious RAM state
- obvious per-frame scroll update
- obvious VDP register write path

## Verification Commands

Run standard repository verification:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
````

Build the Showcase ROM:

```bash
make -C rom/showcase clean
make -C rom/showcase
```

Run deterministic headless verification twice:

```bash
./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 8
./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 8
```

## Acceptance Criteria

This milestone is complete only if all of the following are true:

* the Showcase ROM visibly demonstrates vertical scrolling
* vertical scroll is updated through deterministic per-frame logic
* the ROM builds successfully
* repository tests remain green
* repeated headless runs produce identical output
* `docs/showcase_state_snapshot.md` is updated
* a completion report is written

## Deliverables

* updated Showcase ROM with vertical scrolling behavior
* completion report:

`artifacts/reports/m34_vertical_scrolling_demo.md`

## Completion Report Requirements

The report must include:

* summary
* files changed
* commands run
* verification results
* issues discovered
* next-step recommendation

## Next Milestone

After successful completion of M34, proceed to:

**M35 – Parallax Scrolling Demo**
