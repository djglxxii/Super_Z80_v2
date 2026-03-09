# Task Packet — M32 System Splash Screen

## Goal

Implement the first proper Showcase ROM splash screen using the M30 scaffold and the M31 text-rendering path.

This milestone should make the ROM boot into a clean, intentional startup presentation that future milestones can build on.

---

# Milestone

**M32 – System Splash Screen**

---

# Scope

Implement a deterministic system splash screen for `rom/showcase/`.

The splash screen should combine:

* local splash tile/tilemap assets
* the reusable text-rendering path from M31
* minimal palette setup already present in the scaffold

This milestone is about **boot presentation and static startup composition only**.

---

# Non-Goals

Do not implement:

* scene manager
* timed scene transitions
* input-driven menu behavior
* scrolling
* sprites
* audio
* animation
* attract mode
* generalized UI systems
* SDK changes unless a real integration defect is found

---

# Files Expected to Change

Likely files:

* `rom/showcase/src/main.asm`
* `rom/showcase/src/init.asm`
* `rom/showcase/src/text.asm`
* `rom/showcase/src/main_loop.asm`
* `rom/showcase/src/vdp_init.asm`
* `rom/showcase/inc/constants.inc`
* `rom/showcase/inc/memory_map.inc`
* `rom/showcase/inc/vdp_helpers.inc`
* `rom/showcase/assets/splash_tiles.asm`
* `rom/showcase/assets/splash_tilemap.asm`
* `docs/showcase_state_snapshot.md`
* `docs/state_snapshot.md`
* `artifacts/reports/m32_system_splash_screen.md`

Only modify other files if required and explain why.

---

# Implementation Requirements

## 1. Boot-to-Splash Behavior

On ROM boot, the Showcase project should present a stable splash screen rather than only raw validation text placement.

The splash should be the default visible state for the milestone verification run.

---

## 2. Splash Composition

Use the local Showcase assets under:

```text
rom/showcase/assets/splash_tiles.asm
rom/showcase/assets/splash_tilemap.asm
```

Compose a startup screen that visibly reads as a system splash.

It should include at minimum:

* the splash graphic/tilemap
* text rendered through the M31 text path
* a clear title identity for the ROM

Required visible text:

```text
SUPER Z80 SHOWCASE
SYSTEM SPLASH OK
```

These may be layered with the splash assets in a readable way.

---

## 3. Deterministic Static Presentation

The splash screen must remain deterministic and stable across repeated runs.

Avoid timing-based transitions or hidden mutable state.

A static splash is acceptable for this milestone.

---

## 4. Developer-Reference Clarity

Keep the implementation easy to understand for future ROM developers:

* clear init path
* obvious asset upload flow
* obvious tilemap/text composition order
* minimal indirection

The code should demonstrate how to boot into a structured presentation screen.

---

## 5. Preserve Existing Text Path

Use the M31 reusable text helpers rather than bypassing them with one-off direct tilemap writes for the required text.

This milestone should reinforce the established text-rendering path.

---

# Verification Commands

Run standard verification:

```bash
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Build the Showcase ROM:

```bash
make -C rom/showcase clean
make -C rom/showcase
```

Run deterministic headless verification twice:

```bash
./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 4
./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 4
```

---

# Acceptance Criteria

This milestone is complete only if all of the following are true:

* the ROM boots to a recognizable splash screen
* the splash uses the local Showcase splash assets
* the required validation text is rendered via the reusable text path
* repeated headless runs produce identical output
* repository build/tests remain green
* `docs/showcase_state_snapshot.md` is updated
* a completion report is written

---

# Deliverables

* updated Showcase ROM splash screen implementation
* updated Showcase ROM binary
* completion report:

```text
artifacts/reports/m32_system_splash_screen.md
```

---

# Completion Report Requirements

The report must include:

* summary
* files changed
* commands run
* verification results
* issues discovered
* next-step recommendation

---

# Next Milestone

After successful completion of M32, proceed to:

**M33 – Scrolling Tilemap Demo**
