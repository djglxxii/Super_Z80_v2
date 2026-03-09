# Super_Z80_v2 State Snapshot

## Current Milestone
M29d

## Audio Status
Current validated audio implementation:
- PSG-style APU only

Planned audio expansion:
- YM2151 FM subsystem register interface, deterministic internal channel/operator state, timer/status/IRQ-facing behavior, and internal deterministic mono FM sample generation implemented on ports `0x70-0x71`
- no PSG/YM mixer integration or SDL-facing YM path yet

PCM remains excluded from the platform design.

## Recent Changes
- M29d complete.
- YM2151 now generates deterministic internal mono FM samples during scheduler-owned `tick(uint32_t cycles)` calls using a shared `1024`-entry sine lookup table, placeholder phase increments, basic envelope progression, and channel algorithm routing.
- Per-operator YM2151 state now includes explicit phase, phase increment, envelope level/state, and last-output tracking so FM synthesis advances without relying on wall-clock time or external buffering.
- YM2151 key-on writes now transition operators into `ATTACK`, key-off writes transition them into `RELEASE`, and `current_sample()` exposes the latest internally generated signed `int16_t` sample for a future mixer milestone.
- YM2151 unit coverage now verifies reset sample state, stored phase increments, nonzero FM sample generation, deterministic repeated sample sequences, and release-to-silence behavior alongside the existing register/timer/bus/scheduler checks.
- M29c complete.
- YM2151 now stores deterministic Timer A and Timer B latch/counter state, enable bits, overflow flags, and IRQ enable bits alongside the existing channel/operator model.
- YM2151 timer control register writes now update timer start, IRQ-enable, and overflow-clear behavior while preserving raw register storage, and status reads on port `0x70` now expose timer overflow bits.
- `YM2151::tick(uint32_t cycles)` now advances timers deterministically from explicit scheduler-owned cycles, with Timer A using a `1024 - latch` countdown model and Timer B using a `(256 - latch) * 16` countdown model as the current minimal deterministic contract for future FM work.
- YM2151 now exposes an IRQ-facing pending signal when an enabled timer overflows with its IRQ-enable bit set, and the bus exposes that pending state through a dedicated accessor without routing it into CPU interrupt delivery yet.
- YM2151 unit coverage now verifies timer latch storage, disabled/enabled advancement, overflow/status behavior, IRQ gating, clear-control semantics, repeatability, bus-visible pending state, and the scheduler-owned nonzero YM2151 cycle budget.
- M29b complete.
- YM2151 now stores deterministic per-channel and per-operator FM state, including algorithm, feedback, frequency/block, key-on state, and placeholder phase counters for all 8 channels and 32 operators.
- YM2151 register writes now decode into channel/operator structures while preserving raw register storage, and `tick(uint32_t cycles)` advances keyed operators through deterministic placeholder phase progression without generating audio.
- YM2151 unit coverage now verifies channel/operator decode, CPU-visible key-on behavior, frequency/block updates, operator parameter storage, and deterministic tick progression.
- M29a complete.
- YM2151 register interface implemented with deterministic internal register storage, CPU-visible address/data ports on `0x70-0x71`, and a scheduler-owned `tick(uint32_t cycles)` hook for future FM timing work.
- Bus routing and unit-test coverage now verify direct device writes, CPU `OUT` sequencing through the YM2151 ports, and scheduler invocation of the placeholder timing hook without generating FM audio.
- Developer-facing platform documentation added for the implemented hardware model, including a canonical hardware reference plus separate graphics, audio, input, timing, and development workflow guides.
- The new M28 docs describe the validated platform exactly as implemented: fixed `32 KiB` ROM, `16 KiB` RAM, VDP ports `0xE0-0xED`, controller ports `0xDC-0xDD`, IRQ and DMA ports `0xF0-0xF9`, and the PSG-style audio block on `0xD0-0xDB`.
- Repository truth now explicitly documents the current platform limits for ROM authors, including the absence of YM2151/FM hardware, the fixed interrupt vector path through `RST 38h`, and SDL shell features as non-authoritative development aids.
- Platform validation coverage added for full deterministic end-to-end emulator stepping across CPU, DMA, IRQ, VBlank, VDP, controller input, audio, and scheduler-owned timing.
- New integration tests now verify byte-identical repeated platform snapshots, CPU resume behavior after VBlank IRQ delivery, DMA completion under scheduler stepping, and deterministic input-plus-audio interaction under headless execution.
- VBlank validation now confirms the scanline 192 boundary, IRQ controller request propagation, and deterministic CPU vector handling without relying on host timing or SDL devices.
- CMake test wiring now includes dedicated M27 integration executables for platform determinism, CPU/DMA/IRQ interaction, VDP/VBlank IRQ timing, and input/audio repeatability.
- Deterministic audio validation coverage now includes focused unit tests for register masking/boundaries, mixer lookup-table behavior, noise/LFSR progression, cadence repeatability, headless buffering, and SDL queue boundary checks.
- New long-running cadence tests verify the scanline accumulator contract directly: `262` scanlines produce `800` samples per frame, `15720` scanlines produce exactly `48000` samples per emulated second, and identical runs remain byte-identical.
- Buffer-boundary validation now confirms the fixed-capacity core audio ring stops at `4096` samples and latches the existing APU overrun flag without changing synthesis behavior.
- SDL dummy-driver validation now checks the runtime mono `48000 Hz` signed-16 little-endian contract, queue byte counts, and sample-width assumptions without requiring a real audio device.
- SDL runtime shell now supports `--sdl-audio`, opening a mono `48000 Hz` signed-16 little-endian SDL audio device and queueing deterministic PCM produced by the emulator core.
- `EmulatorCore` now owns a bounded headless-first audio sample ring buffer, scanline-driven sample production, and explicit sample-consumption APIs for SDL/frontends without moving timing ownership into SDL.
- Audio sample generation now follows deterministic integer accumulators for both samples-per-scanline and APU ticks-per-sample, preserving the `docs/audio_spec.md` cadence contract while keeping host playback non-authoritative.
- Producer-side audio queue overflow now latches the existing APU overrun bit when the fixed-capacity core buffer fills, while underruns remain SDL-local silence at the sink.
- Audio output integration tests added and passing for initialization, sample handoff, mute/disable silence, headless isolation, and repeatable outgoing sample blocks without requiring a real SDL device.
- APU now exposes deterministic mono signed-`int16` sample generation through `generate_current_sample()` and `advance_and_generate_sample(tick_count)`, with no SDL device or host-clock dependency.
- Baseline mixer behavior now maps the 4-bit attenuation registers through the fixed `15..0` lookup table from `docs/audio_spec.md`, sums the four channel contributions, normalizes by `60`, and emits signed 16-bit PCM.
- Global `AUD_CTRL` enable and mute bits now force deterministic silence exactly as specified while tone/noise runtime state remains scheduler-owned.
- APU reset-state writes now also clear internal sample output state alongside tone phases, divider counters, the noise LFSR seed, and the overrun flag.
- Mixer unit tests added and passing for reset silence, single-tone output, volume lookup effects, deterministic noise contribution, multi-channel mixing, full-scale output, repeatability, and mute behavior.
- APU runtime generator state now tracks deterministic tone divider counters, tone phase bits, a noise divider counter, and a 15-bit noise LFSR behind the existing audio register interface.
- `APU::advance_audio_ticks(count)` now advances tone and noise generators from explicit caller-owned tick counts only, with no wall-clock or SDL timing dependency.
- Tone generator progression now reloads from the current 12-bit period registers, toggles phase on divider expiry, treats period `0x000` as silent, and updates future stepping immediately when tone period registers change.
- Noise generator progression now supports fixed divisors `16`, `32`, and `64` ticks plus `Tone C` divider-event clocking through `AUD_NOISE_CTRL[1:0]`.
- White-noise mode now advances a deterministic 15-bit LFSR using bits `0` and `1` XOR feedback, while periodic-noise mode shifts toward zero and reseeds to `0x4000` when its cycle completes.
- Audio control reset-state writes now reinitialize tone phases, tone/noise divider counters, the noise LFSR seed, and the overrun flag while preserving the programmed register values.
- Generator unit tests added and passing for reset defaults, tone countdown cadence, tone phase toggling, white noise stepping, periodic noise reseeding, Tone-C-coupled noise stepping, register update effects, repeatability, and boundary values.
- Audio register state storage implemented in `emulator/include/apu.h`, `emulator/include/apu_registers.h`, and `emulator/src/apu.cpp`.
- Bus port routing now exposes audio ports `0xD0-0xDB` through a dedicated APU device while keeping the audio subsystem silent.
- Tone high-byte, noise control, volume, and global control writes now mask unused bits exactly as defined in `docs/audio_spec.md`.
- Audio reset behavior now initializes tone/noise/control registers to `0x00` and all four volume registers to `0x0F`.
- `AUD_CTRL` bit `2` now behaves as a self-clearing reset-state write, while CPU reads expose only bits `1:0` plus the read-only overrun bit.
- Audio unit tests added and passing for reset defaults, masked read/write behavior, bus routing, and CPU-visible `IN`/`OUT` semantics on the audio ports.
- Audio hardware baseline v1 defined in `docs/audio_spec.md` as the authoritative contract for future implementation work.
- Audio baseline specifies 3 square-wave tone channels, 1 LFSR-based noise channel, mono 16-bit PCM output, and a fixed `48000 Hz` sample rate.
- CPU-visible audio register ports reserved at `0xD0-0xDB`, including tone period, per-channel volume, noise control, and global audio control semantics.
- Audio timing contract now explicitly assigns time ownership to the scheduler and forbids host-clock or SDL-callback-driven synthesis.
- Mixer contract now defines equal-weight summing, deterministic integer normalization, signed 16-bit clamping, and headless-buffer-first integration.
- Deterministic audio verification strategy documented for future milestones, covering register behavior, tone timing, noise reproducibility, mixer output, and scheduler sample cadence.
- Controller input hardware implemented for CPU-visible ports `PAD1` (`0xDC`) and `PAD1_SYS` (`0xDD`).
- PAD1 now reports active-low button state for `Up`, `Down`, `Left`, `Right`, `A`, and `B`, while PAD1_SYS reports active-low `Start`.
- Default controller idle state is explicitly deterministic: both controller ports read back `0xFF` when no buttons are pressed.
- Generic I/O port storage now preserves controller ports as read-only hardware views while leaving unrelated ports writable.
- Bus helpers added so shell-layer code can update controller state without moving host input logic into the core hardware implementation.
- SDL shell entrypoint now supports `--sdl-input`, mapping keyboard input (`Arrow keys`, `Z`, `X`, `Enter`) and SDL gamepad controls into PAD1/PAD1_SYS state.
- Controller input unit tests added and passing for idle state, button press/release transitions, simultaneous valid combinations, bus-visible reads, and ignored direct writes to controller ports.
- VDP baseline documentation added in `docs/vdp_baseline_v1.md` as the authoritative graphics reference for ROM developers.
- Graphics behavior contract frozen at milestone M23 with no new hardware features added.
- VDP behavior is now considered stable and must not change without a bug fix, ROM-driven requirement, or explicit hardware contract revision.
- Hardware sprite collision detection now latches when two visible sprites render non-zero pixels on the same screen coordinate.
- VDP status bit `3` now reports sprite collision and clears on status read, matching the existing latched-status behavior.
- Sprite collision detection uses a sprite-only per-frame mask during rendering, so BG/FG pixels never trigger collisions and detection remains deterministic across priority passes.
- Sprite collision unit tests added and passing for no-collision, overlap detection, transparent overlap, clear-on-read semantics, and mixed-priority overlap.
- Background and foreground planes now use dedicated pattern bank registers on VDP ports `0xEC` and `0xED`.
- Tile plane pattern fetch now resolves bank-aware addresses with bank `0` at `0x0000` and bank `1` at `0x2000`.
- Bus VDP port routing now forwards `0xE0-0xED`.
- Tile pattern bank unit tests added and passing for BG bank `0`, BG bank `1`, FG bank selection, render changes across bank switches, and bus register routing.
- Sprite SAT flags now use bit `3` as a deterministic sprite pattern bank select, with bit `4-7` still reserved.
- Sprite pattern fetch now resolves bank-aware addresses with bank `0` at `0x0000` and bank `1` at `0x2000`.
- Sprite pattern bank unit tests added and passing for bank `0`, bank `1`, same-index cross-bank lookup, transparency preservation, and preserved priority/flip behavior.
- Sprite attribute decoding now treats SAT flags bit `1` as horizontal flip, bit `2` as vertical flip, bit `3` as sprite pattern bank select, and bits `4-7` as reserved for future expansion.
- Sprite renderer now mirrors sprite pattern sampling horizontally, vertically, or both before applying the existing transparency rules.
- Sprite flipping unit tests added and passing for horizontal flip, vertical flip, combined flip, background isolation, and preserved priority behavior.
- Scanline sprite evaluation added to the VDP for deterministic per-scanline SAT selection.
- Hardware sprite limits now enforce a maximum of 16 sprites per scanline, keeping the first 16 visible entries in SAT order.
- VDP status bit `2` now latches sprite overflow and clears on status read.
- Sprite rendering now evaluates SAT per scanline for both sprite priority passes instead of scanning the full SAT during each pass.
- Sprite scanline limit unit tests added and passing for under-limit rendering, limit clipping, overflow status behavior, and SAT-order selection.
- Sprite priority flag implemented in SAT flags bit `0`.
- VDP sprite composition now renders in two deterministic passes: `BG -> sprites(priority=1) -> FG -> sprites(priority=0)`.
- Sprite ordering remains stable by SAT order, with later entries overwriting earlier ones within the same pass.
- Sprite priority unit tests added and passing for FG occlusion, BG visibility, stable SAT overwrite order, and mixed-priority composition.
- Foreground plane implemented as a second tile layer using tilemap base `0x5000`.
- VDP now renders `BG -> FG -> sprites`, with foreground pixel value `0` treated as transparent.
- Foreground scroll registers added on ports `0xEA-0xEB`, with independent read/write/reset behavior from the background scroll registers.
- Bus VDP port routing now forwards `0xE0-0xEB`.
- Foreground plane unit tests added and passing for overwrite, transparency, placement, FG scrolling, independent BG/FG scroll sampling, and bus register read/write behavior.
- Background scrolling registers implemented for the VDP on ports `0xE8-0xE9`.
- Background rendering now samples framebuffer pixels from scrolled world positions with wrapped `32 x 24` tilemap addressing.
- Scroll unit tests added and passing for zero scroll, horizontal and vertical shifts, partial pixel offsets, wraparound, and register read/write behavior.
- Palette and color system implemented for the VDP.
- VDP now exposes palette register ports on `0xE3-0xE7` for palette index and RGBA component access.
- Indexed framebuffer pixels now resolve deterministically through 256 palette entries packed as `0xRRGGBBAA`.
- Palette unit tests added and passing for register reads/writes, overwrite behavior, color resolution, and reset defaults.
- Sprite renderer skeleton added to the VDP framebuffer pipeline.
- VDP now reads sprite attributes from `0x6000` and sprite patterns from the shared pattern table at `0x0000`.
- Sprite rendering now runs after background tiles, with non-zero sprite pixels overwriting the framebuffer.
- Sprite renderer unit tests added and passing for single-sprite rendering, transparency behavior, positioning, and multiple sprites.
- CRC32 utility module added.
- First unit test executable added for deterministic known-answer checks.
- CMake test wiring added for CRC32 validation.
- Deterministic 64KB memory subsystem added with reset, read, and write primitives.
- Memory unit tests added and passing.
- Bus layer added with deterministic ROM, open-bus, and RAM routing behavior.
- Initial CPU-visible memory map implemented: ROM at `0x0000-0x7FFF`, open bus at `0x8000-0xBFFF`, RAM at `0xC000-0xFFFF`.
- Bus unit tests added and passing.
- Z80ex vendored into `emulator/vendor/z80ex/` for deterministic local builds.
- CPU wrapper added with Bus-backed Z80ex memory callbacks, reset, and single-instruction stepping.
- CPU unit test added for `LD A,0x42` followed by `HALT`.
- Deterministic I/O register subsystem added for port space `0x00-0xFF` with reset, read, and write behavior.
- Bus port routing added through `read_port()` and `write_port()`.
- CPU wrapper now connects Z80ex `IN` and `OUT` callbacks to the Bus I/O subsystem.
- I/O unit test added and CPU test expanded to validate port I/O.
- Scheduler subsystem added with deterministic frame and scanline counters.
- Scheduler advances CPU execution by a fixed placeholder instruction count per scanline.
- Scheduler unit tests added for reset, scanline increment, frame wrap, and CPU stepping side effects.
- IRQ controller infrastructure added with status, enable, and acknowledge behavior.
- Bus port routing now exposes IRQ registers at `0xF0-0xF2`.
- CPU interrupt line wiring now follows the bus IRQ controller state.
- VBlank timing added with scanline-driven begin/end transitions.
- Bus port `0xF3` now exposes VBlank active state through the VDP status register.
- Scheduler now generates the first hardware timing event by driving VBlank scanline updates.
- VBlank begin requests IRQ bit 0 through the IRQ controller.
- DMA engine subsystem added with source, destination, length, and control registers on ports `0xF4-0xF9`.
- Bus port routing now exposes DMA registers and the DMA busy flag.
- Scheduler now advances DMA by one byte per scanline for deterministic transfer timing.
- DMA unit tests added and passing for register behavior, transfer progression, and scheduler integration.
- VDP skeleton implemented with deterministic 64KB VRAM storage and an internal `256 x 192` framebuffer allocation.
- Bus port routing now exposes VDP status, control, and data registers on `0xE0-0xE2`.
- Scheduler scanline stepping now drives the VDP scanline callback for VBlank status and frame-ready timing.
- VDP unit tests added and passing for VRAM access, pointer incrementing, reset behavior, and frame-ready status handling.
- Background tile renderer implemented.
- Framebuffer now populated from VRAM tile and tilemap data using the temporary M12 format.
- Tile renderer unit tests added and passing for single-tile output, placement, zeroed defaults, and row stride correctness.

## Known Issues
None yet.

## Verification Status
M29d YM2151 verification is passing with the deterministic build/test flow: `cmake -S . -B build`, `cmake --build build`, `ctest --test-dir build --output-on-failure --tests-regex "super_z80_test_(ym2151|scheduler|bus)"`, and `ctest --test-dir build --output-on-failure`. The expanded `super_z80_test_ym2151` coverage verifies reset sample state, stored phase increments, deterministic FM sample generation, sample repeatability, release-to-silence behavior, timer/status/IRQ behavior, bus-visible pending state, and the scheduler-owned fixed YM2151 cycle budget.

M29c YM2151 verification is passing with the deterministic build/test flow: `cmake -S . -B build`, `cmake --build build`, `ctest --test-dir build --output-on-failure --tests-regex "super_z80_test_(ym2151|scheduler|bus)"`, and `ctest --test-dir build --output-on-failure`. The expanded `super_z80_test_ym2151` coverage verifies timer latch storage, disabled/enabled deterministic progression, Timer A and Timer B overflow/status bits, IRQ-pending gating, control-path clears, repeatability, bus-visible pending state, and the scheduler-owned fixed YM2151 cycle budget.

M29b YM2151 verification is passing with the deterministic build/test flow: `cmake -S . -B build`, `cmake --build build`, `ctest --test-dir build --output-on-failure --tests-regex "super_z80_test_(ym2151|scheduler|bus)"`, and `ctest --test-dir build --output-on-failure`. The expanded `super_z80_test_ym2151` coverage verifies channel/operator state decode, key-on writes, frequency/block updates, operator parameter storage, CPU-visible `OUT` behavior on ports `0x70-0x71`, and deterministic placeholder phase progression.

M29a YM2151 verification is passing with the full deterministic build/test flow: `cmake -S . -B build`, `cmake --build build`, and `ctest --test-dir build --output-on-failure`. The new `super_z80_test_ym2151` coverage verifies register selection, register writes, CPU-visible `OUT` behavior on ports `0x70-0x71`, and scheduler invocation of the placeholder timing hook.

M28 documentation verification passing: `test -f docs/developer_guide.md`, `test -f docs/hardware_reference.md`, `test -f docs/programming_graphics.md`, `test -f docs/programming_audio.md`, `test -f docs/programming_input.md`, and `test -f docs/platform_timing.md`.

Most recent implementation verification remains the passing M27 run: `cmake -S . -B build`, `cmake --build build`, and `ctest --test-dir build --output-on-failure`. The full suite included `super_z80_test_platform_determinism`, `super_z80_test_cpu_dma_irq_integration`, `super_z80_test_vdp_vblank_irq`, and `super_z80_test_input_audio_integration`, all passing in the shared deterministic headless build.

## Next Step
Execute `M29e` to integrate YM2151 output into the existing PSG audio path while preserving deterministic internal mixing behavior.
