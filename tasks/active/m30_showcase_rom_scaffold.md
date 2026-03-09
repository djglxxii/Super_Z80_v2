# Task Packet — M30 Showcase ROM Project Scaffold

## Goal

Transform `rom/showcase/` into the canonical Showcase ROM project structure that will support future content milestones.

This milestone introduces the structural layout and build system used for all subsequent Showcase ROM work.

---

# Milestone

**M30 – Showcase ROM Project Scaffold**

---

# Scope

Restructure `rom/showcase/` into a proper ROM project with modular source organization.

Create a maintainable structure for:

* ROM entry point
* runtime integration
* graphics assets
* tilemaps
* input handling
* scene logic

This milestone focuses only on **project structure**, not content.

---

# Target Directory Structure

The Showcase ROM should be reorganized to:

```
rom/showcase/

src/
    main.asm
    init.asm
    vdp_init.asm
    text.asm
    main_loop.asm

assets/
    font_8x8.asm
    splash_tiles.asm
    splash_tilemap.asm

inc/
    constants.inc
    memory_map.inc
    vdp_helpers.inc

build/

Makefile
```

---

# Structural Requirements

## ROM Entry

`main.asm` must:

```
include runtime
call init
enter main_loop
```

---

## Initialization Module

`init.asm`

Responsibilities:

* initialize VDP registers
* initialize palette
* clear VRAM
* upload font
* upload splash assets

---

## VDP Helpers

Create helpers for:

```
vdp_write_vram
vdp_set_address
vdp_upload_block
vdp_clear_tilemap
```

---

## Main Loop

`main_loop.asm`

Must contain:

```
forever loop
    wait_vblank
    poll_input
    update
    render
```

Even if update/render are empty placeholders.

---

## Asset Organization

Move assets out of SDK references and into the ROM project:

```
rom/showcase/assets/
```

The ROM project should explicitly include them.

---

# Makefile Requirements

`rom/showcase/Makefile` must support:

```
make
make clean
```

Expected output:

```
build/showcase.bin
```

Assembler:

```
sjasmplus
```

Include paths:

```
-I .
-I ../../sdk
```

---

# Verification

Build ROM:

```
make -C rom/showcase
```

Run emulator:

```
./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 4
```

Run twice to confirm deterministic output.

---

# Acceptance Criteria

Milestone is complete when:

* ROM project structure exists
* ROM assembles successfully
* ROM runs in emulator
* ROM loop executes deterministically
* build system works via Makefile
* repository tests remain green
* `docs/showcase_state_snapshot.md` updated
* completion report written

---

# Deliverables

New project layout:

```
rom/showcase/
```

Completion report:

```
artifacts/reports/m30_showcase_rom_project_scaffold.md
```

---

# Next Milestone

After M30:

**M31 – Showcase ROM Scene Framework**

This introduces:

* scene manager
* splash screen
* timed transitions
* basic input control

---
