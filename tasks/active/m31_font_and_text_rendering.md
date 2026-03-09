# Task Packet — M31 Font and Text Rendering

## Goal

Implement the first real reusable text rendering layer for the Showcase ROM on top of the M30 scaffold.

This milestone must turn the current minimal text path into a clear developer-reference implementation for rendering ASCII text using the local Showcase font asset.

---

# Milestone

**M31 – Font and Text Rendering**

---

# Scope

Implement a reusable tile-based text renderer for `rom/showcase/`.

The renderer must support:

* writing ASCII text using the local 8×8 font
* positioning text by tile coordinates
* writing multiple lines
* clearing or overwriting text lines deterministically

This milestone is about **text rendering only**.

---

# Non-Goals

Do not implement:

* splash screen sequencing beyond what is required to show text
* menus
* scene manager
* scrolling
* sprites
* audio
* animation
* input-driven UI behavior
* generalized UI frameworks
* SDK changes unless a real defect is found

---

# Files Expected to Change

Likely files:

* `rom/showcase/src/text.asm`
* `rom/showcase/src/init.asm`
* `rom/showcase/src/main.asm`
* `rom/showcase/src/main_loop.asm`
* `rom/showcase/inc/constants.inc`
* `rom/showcase/inc/memory_map.inc`
* `rom/showcase/inc/vdp_helpers.inc`
* `docs/showcase_state_snapshot.md`
* `docs/state_snapshot.md`
* `artifacts/reports/m31_font_and_text_rendering.md`

Only modify other files if required and explain why.

---

# Implementation Requirements

## 1. Reusable Text API

Implement a small, stable text-rendering interface in `rom/showcase/src/text.asm`.

It must support at minimum:

* write zero-terminated ASCII string to BG tilemap
* write string at tile X/Y position
* write multiple fixed strings deterministically

Keep the interface simple and obvious for future ROM developers.

---

## 2. ASCII-to-Tile Mapping

Use the local Showcase font asset under:

```text
rom/showcase/assets/font_8x8.asm
```

Map printable ASCII characters to the correct tile indices.

Behavior for unsupported characters should be deterministic and documented in comments.

---

## 3. Visible Validation Output

Update the Showcase ROM so that it visibly renders at least:

```text
SUPER Z80 SHOWCASE
FONT AND TEXT OK
```

These lines should be placed intentionally on screen using the reusable text path.

---

## 4. Deterministic Tilemap Writes

Text rendering must produce deterministic tilemap contents across repeated runs.

Avoid hidden state, implicit cursor behavior, or timing-dependent writes.

---

## 5. Developer-Reference Clarity

Code should read like a reference implementation:

* clear entry points
* simple calling pattern
* minimal indirection
* comments where needed to explain usage

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

* the Showcase ROM contains a reusable text-rendering path
* ASCII strings render using the local font asset
* the ROM visibly renders the required validation text
* repeated headless runs produce identical output
* repository build/tests remain green
* `docs/showcase_state_snapshot.md` is updated
* a completion report is written

---

# Deliverables

* reusable text-rendering implementation in `rom/showcase/`
* updated Showcase ROM binary
* completion report:

```text
artifacts/reports/m31_font_and_text_rendering.md
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

After successful completion of M31, proceed to:

**M32 – System Splash Screen**
