# Super_Z80 Timing Model

## Deterministic Contract
The platform advances from scanline stepping. CPU, DMA, VBlank, VDP scanline state, and audio sample production all derive from that single scheduler-owned timeline.

## Scanline Execution
- Scanlines per frame: `262`
- Frames per second: `60`
- Scanlines per second: `15720`
- CPU instruction steps per scanline: `10`

For each scanline step the emulator performs:
1. `10` CPU instruction steps
2. one DMA byte transfer if DMA is active
3. scanline increment and frame wrap
4. VBlank update
5. VDP scanline update
6. audio sample production for that scanline

## VBlank and Frame Timing
- `frame_ready` latches at scanline `191`
- VBlank becomes active at scanline `192`
- VBlank clears when scanline wraps to `0`

Implications:
- Software can treat scanline `191` as the last visible-line completion point.
- The IRQ request for VBlank appears at the `192` boundary, not earlier.

## IRQ Behavior
- IRQ request source currently documented here: VBlank only.
- VBlank requests IRQ bit `0x01`.
- CPU-visible IRQ ports:
  - `0xF0`: status
  - `0xF1`: enable mask
  - `0xF2`: acknowledge
- The CPU wrapper presents interrupt data `0xFF`, so enabled maskable interrupts land on the standard `RST 38h` vector.

Observed execution rule:
- The scheduler raises the VBlank IRQ on the scanline where VBlank begins.
- The CPU observes and services that interrupt on the following CPU step sequence, so software should not expect the handler to run before scanline `192` has completed.

## DMA Restrictions
- DMA is not cycle-stealable or burst-based in the current platform.
- DMA moves exactly one byte per scanline while active.
- There is no separate DMA IRQ.
- DMA timing is deterministic because it is scheduler-owned and independent of SDL or host pacing.

## Audio Progression Timing
- Output rate: `48000` samples per emulated second.
- Audio samples are emitted from a rational accumulator driven once per scanline.
- Over one frame, the scanline cadence sums to `800` samples.
- Over one second, the cadence sums to exactly `48000` samples.
- The current deterministic pattern contains `14` scanlines per frame that emit `4` samples; the remaining scanlines emit `3`.

Per-sample synthesis:
- Each emitted sample advances the APU by an integer tick count derived from the `3579545 Hz` audio master clock and the fixed `48000 Hz` sample rate.
- No floating-point accumulation is used.

## Host and SDL Restrictions
- Host wall-clock time is not authoritative.
- SDL audio queue timing does not alter synthesis state.
- SDL input events update controller state, but the emulator only observes that state when software reads the ports.

## Software Guidance
- Poll status and IRQ state from emulated ports, not from host timing assumptions.
- Build game loops around deterministic frame or scanline progression.
- Do not assume DMA completes within a frame unless the programmed length fits one byte per scanline.
