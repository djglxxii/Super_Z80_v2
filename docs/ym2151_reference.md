# YM2151 Reference

## Purpose

Authoritative YM2151 implementation and programming reference for the Super_Z80 project.

## CPU Interface

- Port `0x70` - address register
- Port `0x71` - data register

CPU writes select a YM2151 register through port `0x70` and write the selected register value through port `0x71`. Status reads are exposed through port `0x70`.

## Internal Model

The implemented YM2151 model is deterministic and scheduler-driven.

- 8 channels
- 4 operators per channel
- per-channel algorithm selection
- per-channel feedback state
- per-operator phase progression
- Timer A and Timer B state
- timer overflow status bits
- IRQ-facing pending behavior when enabled timer overflows occur

Each operator stores explicit phase, phase increment, envelope state, envelope level, and last-output state. Each channel stores operator state plus frequency, block, algorithm, feedback, and key-on state needed for deterministic FM stepping.

## Register Categories

### Algorithm / Feedback

Channel control registers select the algorithm routing and feedback amount used when combining the four operators in a channel.

### Key On/Off

Key-on control writes start operator envelope progression for the targeted channel. Key-off transitions operators into release and eventually silence.

### Frequency / Block

Frequency and block registers define the channel pitch state that feeds operator phase increment calculation.

### Operator Parameter Registers

Operator parameter registers are decoded into the per-operator state model. The current implementation preserves raw register values and uses the decoded operator state for deterministic synthesis progression.

### Timer Registers

Timer registers store the Timer A latch, Timer B latch, start bits, and IRQ-enable bits. Timer progression occurs only through explicit scheduler-owned `tick(uint32_t cycles)` calls.

### Status / Control Behavior

Status reads expose timer overflow bits. Control writes can start timers, enable timer IRQ-facing behavior, and clear latched overflow state without introducing host-time dependencies.

## FM Sample Generation

The implemented FM generator advances entirely under scheduler control.

- operator phases advance from stored phase increments
- envelope state is updated as operators key on, sustain, release, and return to off
- channel output is produced by evaluating the selected algorithm across the four operators
- feedback participates in the per-channel operator chain
- the device updates a deterministic mono signed `int16_t` current sample

FM sample production is driven by `YM2151::tick(uint32_t cycles)` and never by SDL callbacks, wall-clock timing, or background threads.

## Mixer Integration

Mixed platform audio is produced by combining:

- PSG output from the baseline APU
- YM2151 output from the FM device

The scheduler advances the APU and YM2151, then the `AudioMixer` combines their current mono samples into a deterministic mixed signed `int16_t` sample. That mixed sample is the emulator-owned audio stream consumed by headless tests and by the SDL host output path.

## IRQ-Facing Behavior

Timer A and Timer B overflow bits latch into YM2151 status state. When the corresponding timer IRQ-enable bit is set, an overflow also raises the YM2151 IRQ-facing pending signal exposed through the bus. This is currently documented as IRQ-facing behavior rather than full CPU interrupt delivery semantics.
