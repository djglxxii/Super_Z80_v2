# Super_Z80_v2 Execution Plan

## Strategy

The project follows deterministic, milestone-driven development with explicit verification at each step.  Work proceeds in phases:

1. **Infrastructure & VDP Completion (M0–M23)** – Already completed.
2. **Platform Completion (M24 and onwards)** – Current focus.
3. **Future Extensions (optional)** – Only if ROM‑driven or bug‑fixing needs arise.

Each milestone must remain narrow in scope, produce a report in `artifacts/reports/`, update `docs/state_snapshot.md` appropriately, and include deterministic build/tests commands.

---

## Phase 1 – Infrastructure & VDP Completion (Completed)

All core emulator subsystems and graphics features are implemented.

| Milestone  | Description                                                                                                                                                                                                                                                                               |
| ---------- | ----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------- |
| **M0–M17** | Repository bootstrap; emulator skeleton; CRC utilities; memory, bus, CPU; I/O routing; scheduler/timing; IRQ controller; VBlank; DMA; video pipeline skeleton; audio subsystem placeholder; emulator coordinator; headless runner; regression harness; SDL shell; replay/snapshot parity. |
| **M16**    | Foreground tile plane (second layer) with independent scrolling.                                                                                                                                                                                                                          |
| **M17**    | Sprite priority (front/behind FG) and deterministic ordering.                                                                                                                                                                                                                             |
| **M18**    | Sprite scanline evaluation and 16‑sprite limit per scanline; overflow flag.                                                                                                                                                                                                               |
| **M19**    | Sprite attributes: horizontal & vertical flipping.                                                                                                                                                                                                                                        |
| **M20**    | Sprite pattern banks: bank select flag; bank 0 (0x0000), bank 1 (0x2000).                                                                                                                                                                                                                 |
| **M21**    | Tile pattern banks for BG and FG planes via `0xEC`/`0xED` registers.                                                                                                                                                                                                                      |
| **M22**    | Sprite collision detection; status register bit for sprite collision.                                                                                                                                                                                                                     |
| **M23**    | VDP baseline freeze.  A canonical graphics spec (`docs/vdp_baseline_v1.md`) was added and the VDP contract is now locked; future changes require a bug fix, ROM‑driven feature, or explicit revision.                                                                                     |

**Result:** The emulator now has a complete memory/bus/CPU/IRQ/DMA infrastructure and a stable, fully‑featured VDP (background, foreground, sprites with priority, flipping, pattern banks, scanline limits, collision detection, scrolling, palette system).

---

## Phase 2 – Platform Completion (Planned)

With the VDP frozen, focus shifts to finishing the remaining hardware subsystems—primarily controller input and audio—and validating the entire emulator.

### M24 – Controller Input Hardware

* Wire PAD1/PAD1_SYS controller ports through the bus and I/O layers.
* Implement deterministic button reading and expose them via the SDL shell.
* Update unit tests to cover various button states.

### M25 – Audio System Design

* Produce `docs/audio_spec.md` that defines the audio architecture: number of channels, noise vs. tone, volume control, sample format, and register map.
* Specify how audio integrates with the scheduler and bus.

### M26 – Audio Implementation (sub‑milestones)

1. **M26a – Audio Registers & State:** Implement the audio registers in the bus, including reset behaviour and write semantics.
2. **M26b – Tone Generators:** Create tone and noise generators according to the spec; integrate with the scheduler.
3. **M26c – Mixer & Output:** Mix channels into a single PCM output; support both a headless audio dump for tests and real‑time output via SDL.
4. **M26d – Audio Tests:** Add deterministic unit tests for frequencies, register writes, mixing, and noise behaviour.

### M27 – Platform Validation

* Run an end‑to‑end validation harness that exercises CPU, DMA, IRQ, VBlank, input, and audio together.
* Create or reuse small ROM snippets to verify correctness.
* Fix any remaining bugs in hardware behaviour.

### M28 – Documentation & SDK Update

* Update `docs/plan.md`, `docs/state_snapshot.md`, `AGENTS.md` and other docs to reflect completed subsystems.
* Provide clear guidance for ROM developers about the VDP and audio contract.
* Ensure the SDK libraries/examples align with the final hardware model.

---

## Phase 3 – Future Extensions (Optional)

Only pursue these if ROM developers or tests reveal a need.  Each would be scoped as its own milestone:

* **Window/HUD Plane:** An overlay plane for HUD elements.
* **Mid‑frame Palette/Scroll Changes:** For raster effects.
* **Advanced Audio Features:** Sample playback or FM synthesis.
* **Additional VDP Registers:** e.g. sprite‑0 hit flag or scanline interrupts.

The previously planned **Showcase ROM** is intentionally deferred and will be treated as a separate project once the emulator platform is complete.

---

## Verification & Rollback Strategy

* Verification commands for each milestone must include building (`cmake -S . -B build`, `cmake --build build`) and running tests (`ctest --test-dir build`).  Milestones only complete when tests pass and a report is generated.
* Any milestone work must be reversible via git history; scope must remain narrow to allow rollback.
* The plan may be updated if new subsystems are discovered, but only through explicit task packets that also update this plan.

---
