# Super_Z80_v2 Audio Baseline v1

## Status
Authoritative baseline for milestone M25.

This document defines the baseline audio hardware contract for Super_Z80_v2. It is a specification only. No synthesis, mixing, SDL device, or runtime audio behavior is implemented by this milestone.

The baseline favors determinism, small implementation steps, and a narrow CPU-visible interface that fits the existing bus and scheduler architecture.

## 1. Audio Hardware Overview

### Baseline Topology
- 4 monaural input channels.
- 3 tone channels: `Tone A`, `Tone B`, `Tone C`.
- 1 noise channel: `Noise`.
- Final output is a single signed 16-bit PCM stream.

### Supported Waveform Types
- Tone channels generate a 50% duty-cycle square wave.
- Noise channel generates 1-bit pseudo-random noise from a deterministic linear feedback shift register (LFSR).

### Tone Generation Model
- Each tone channel is controlled by a 12-bit period value.
- The tone output toggles between `+1` and `-1` when its divider expires.
- Period value `0x000` is treated as silence to avoid undefined or host-dependent behavior.

Tone frequency is defined by:

`tone_frequency_hz = audio_master_clock_hz / (32 * tone_period)`

Baseline constants:
- `audio_master_clock_hz = 3,579,545`
- `tone_period` range: `0x001` to `0xFFF`

The exact divider implementation must match this formula in future milestones.

### Noise Generation Model
- Noise uses a 15-bit LFSR.
- The output bit is the LFSR least-significant bit before the next shift.
- The default feedback taps are bits `0` and `1` XORed into bit `14`.
- Noise clock selection is programmable.
- Noise can run from fixed divisors or from `Tone C` period events.

### Mixing Behavior
- Each channel contributes a signed normalized sample in the range `[-level, +level]`.
- Tone and noise levels are controlled independently by 4-bit volume registers.
- The mixer sums all enabled channels, normalizes by the maximum possible active amplitude, then converts to signed 16-bit PCM.
- Output is mono only in the baseline.

## 2. Audio Register Map

Audio ports occupy `0xD0` through `0xDB`. This range is intentionally contiguous and does not overlap the existing controller (`0xDC-0xDD`), VDP (`0xE0-0xED`), or IRQ/DMA (`0xF0-0xF9`) allocations.

Unless otherwise stated:
- All audio registers are readable and writable.
- Unused bits read back as `0`.
- Writes mask off unused bits.
- Reset occurs on emulator reset and initializes all registers to their reset values.

### Register Summary

| Port | Name | Bits | Reset | Behavior |
| --- | --- | --- | --- | --- |
| `0xD0` | `AUD_TONE_A_LO` | `7:0` tone A period low | `0x00` | R/W |
| `0xD1` | `AUD_TONE_A_HI` | `3:0` tone A period high, `7:4` unused | `0x00` | R/W |
| `0xD2` | `AUD_TONE_B_LO` | `7:0` tone B period low | `0x00` | R/W |
| `0xD3` | `AUD_TONE_B_HI` | `3:0` tone B period high, `7:4` unused | `0x00` | R/W |
| `0xD4` | `AUD_TONE_C_LO` | `7:0` tone C period low | `0x00` | R/W |
| `0xD5` | `AUD_TONE_C_HI` | `3:0` tone C period high, `7:4` unused | `0x00` | R/W |
| `0xD6` | `AUD_NOISE_CTRL` | `1:0` noise period select, `2` noise mode select, `7:3` unused | `0x00` | R/W |
| `0xD7` | `AUD_VOL_A` | `3:0` tone A volume, `7:4` unused | `0x0F` | R/W |
| `0xD8` | `AUD_VOL_B` | `3:0` tone B volume, `7:4` unused | `0x0F` | R/W |
| `0xD9` | `AUD_VOL_C` | `3:0` tone C volume, `7:4` unused | `0x0F` | R/W |
| `0xDA` | `AUD_VOL_N` | `3:0` noise volume, `7:4` unused | `0x0F` | R/W |
| `0xDB` | `AUD_CTRL` | `0` global enable, `1` global mute, `2` reset phases/LFSR, `6:3` reserved, `7` sample buffer overrun status | `0x00` | R/W for bits `2:0`, bit `7` read-only |

### Detailed Register Semantics

#### `0xD0-0xD5` Tone Period Registers
- Each tone period is little-endian across `LO` then `HI`.
- Effective 12-bit value: `((HI & 0x0F) << 8) | LO`.
- Read returns the masked stored value.
- A period value of `0x000` silences the channel.

#### `0xD6` Noise Control
- Bits `1:0` select the noise clock:
- `00`: `audio_master_clock_hz / 32 / 16`
- `01`: `audio_master_clock_hz / 32 / 32`
- `10`: `audio_master_clock_hz / 32 / 64`
- `11`: clock from `Tone C` divider events
- Bit `2` selects periodicity mode:
- `0`: white-noise LFSR stepping
- `1`: periodic-noise mode using the same LFSR with reset-on-cycle semantics defined by the implementation milestone

The exact periodic-noise stepping rules are deferred to M26b, but the externally visible contract is that mode selection must remain deterministic and readable from this register.

#### `0xD7-0xDA` Volume Registers
- 4-bit attenuation-style scale.
- `0x0` is maximum audible level.
- `0xF` is silent.
- Future implementation must map the 16 steps through a fixed lookup table so amplitude is deterministic and host-independent.

Baseline lookup table:

| Register Value | Linear Level |
| --- | --- |
| `0x0` | `15` |
| `0x1` | `14` |
| `0x2` | `13` |
| `0x3` | `12` |
| `0x4` | `11` |
| `0x5` | `10` |
| `0x6` | `9` |
| `0x7` | `8` |
| `0x8` | `7` |
| `0x9` | `6` |
| `0xA` | `5` |
| `0xB` | `4` |
| `0xC` | `3` |
| `0xD` | `2` |
| `0xE` | `1` |
| `0xF` | `0` |

#### `0xDB` Global Audio Control
- Bit `0` `ENABLE`:
- `0`: audio subsystem disabled; generated samples are forced to silence.
- `1`: audio subsystem active.
- Bit `1` `MUTE`:
- `0`: normal output.
- `1`: output forced to silence while channel state still advances.
- Bit `2` `RESET_STATE`:
- Writing `1` resets tone phases, divider countdowns, noise LFSR, sample accumulator state, and overrun status.
- This bit is self-clearing and reads back as `0`.
- Bit `7` `OVERRUN`:
- Set if the future producer-side sample queue exceeds its defined capacity before consumption.
- Cleared by writing `1` to bit `2` or on emulator reset.
- Read-only from the CPU perspective.

## 3. Channel Architecture

### Tone Channels

Each tone channel has:
- 12-bit period register.
- 4-bit volume register.
- Internal divider counter.
- 1-bit phase state.

Frequency control:
- Driven only by the programmed period.
- No sweep, envelope, or modulation in the baseline.

Volume control:
- The volume register selects a fixed linear amplitude from the baseline lookup table.

Phase generation:
- The phase toggles each time the divider expires.
- Reset initializes the phase to low output.

Output sample behavior:
- If disabled, muted, or volume level is silent, the channel contributes `0`.
- Otherwise it contributes either `+level` or `-level` depending on current phase.

### Noise Channel

The noise channel has:
- Noise control register.
- 4-bit volume register.
- Internal clock divider.
- 15-bit LFSR state.

Frequency control:
- Selected by `AUD_NOISE_CTRL[1:0]`.
- Source may be fixed divisors or `Tone C` divider events.

Volume control:
- Same 4-bit attenuation mapping as tone channels.

Phase generation:
- There is no square-wave phase bit.
- The sample sign is derived from the current noise output bit.

Output sample behavior:
- If disabled, muted, or volume level is silent, the channel contributes `0`.
- Otherwise output is `+level` when the current noise bit is `1`, and `-level` when it is `0`.

## 4. Timing Model

### Sample Rate
- Baseline output sample rate: `48000 Hz`.
- This rate is part of the hardware contract for deterministic tests and SDL integration.

### Scheduler Ownership
- The scheduler owns audio time.
- The audio subsystem must not run from host wall-clock time.
- Audio generation advances only when the emulator advances.

### Update Cadence
- Audio state advances during scheduler stepping.
- The scheduler produces a deterministic number of audio samples per scanline using a rational accumulator.

Baseline cadence constants:
- video scanlines per frame: `262`
- video frames per second: `60`
- scanlines per second: `15720`
- samples per second: `48000`

This yields:
- exactly `48000` samples per emulated second
- a repeating deterministic pattern of per-scanline sample counts whose sum over one second is `48000`

Future implementation must avoid floating-point drift. Integer or fixed-point accumulation is required.

### Frame Boundaries
- Audio sample generation is continuous across frame boundaries.
- VBlank does not reset phase, mixer state, or sample accumulation.
- Frame boundaries matter only for scheduler bookkeeping, not for audio waveform resets.

### Determinism Rules
- No host-time pacing may change generated sample values.
- No SDL callback timing may influence synthesis state.
- The same ROM inputs and emulator stepping must produce byte-identical audio samples.
- Save-state and replay work may later require audio state serialization; the baseline design must keep that feasible by avoiding hidden host-owned state.

## 5. Mixer Design

### Inputs
- 4 mono channel inputs.

### Formula
1. Convert each active channel into a signed integer contribution.
2. Sum all four contributions.
3. Normalize against the maximum absolute sum of four channels at level `15`.
4. Clamp to signed 16-bit range.

Reference formula:

`pcm16 = clamp_s16((sum * 32767) / 60)`

Where:
- `sum` is the signed channel sum in `[-60, +60]`
- `60` is `4 channels * max level 15`

### Normalization
- All channels are equally weighted.
- There is no panning, per-channel bias, or analog-model variation in the baseline.

### Clipping Behavior
- After normalization, clamp to `[-32768, 32767]`.
- Because of the normalization rule above, clipping should only occur due to implementation error or future non-baseline extensions.

### Output Format
- Signed 16-bit little-endian PCM.
- Mono.
- `48000 Hz`.

## 6. Emulator Integration

### Bus
- The bus exposes ports `0xD0-0xDB`.
- Bus reads and writes are the only CPU-visible control surface for audio state.
- The bus owns register storage and reset-visible values in M26a.

### Scheduler
- The scheduler is responsible for calling audio advancement at deterministic cadence.
- The scheduler provides the authoritative timing source for divider stepping and sample production.

### Emulator Core
- The emulator core owns audio buffer lifetime and makes produced samples available to frontends and headless tests.
- The core must be able to run with audio generation enabled even if no SDL device is active.

### SDL Shell
- SDL is an output sink only.
- SDL audio callbacks must consume already-generated PCM from an emulator-owned buffer or queue.
- SDL callback timing must never directly step synthesis.
- If the SDL device underruns, the shell outputs silence locally without mutating emulator timing or channel state.

### Headless Operation
- Headless runs must be able to inspect or dump generated PCM deterministically.
- Audio verification must not require speakers, real-time playback, or SDL.

## 7. Deterministic Testing Strategy

The baseline testing strategy for later milestones is:

- Register tests:
- verify reset values, masking behavior, readback behavior, and self-clearing `RESET_STATE`
- Tone tests:
- program known periods and validate generated edge timing and sample sequences
- Noise tests:
- validate exact LFSR bit sequence from a known seed and clock source
- Mixer tests:
- validate summed PCM output for fixed channel states and volumes
- Scheduler integration tests:
- validate sample counts per scanline, per frame, and over longer deterministic runs
- Buffer tests:
- validate queue overrun and underrun handling without requiring SDL
- Golden-buffer tests:
- run short ROM-free hardware sequences and compare emitted PCM bytes to checked-in expected output

Deterministic test rules:
- expected outputs must be byte-for-byte comparable
- tests must not read host audio devices
- tests must avoid floating-point comparisons when integer math can define the contract exactly

## 8. Future Extension Notes

The following are explicitly non-baseline and must not be implemented under M25 or assumed by ROMs:

- stereo mixing
- envelope generators
- frequency sweep
- PCM sample playback
- FM synthesis
- per-channel panning
- analog filter emulation
- separate DAC bias or nonlinear mixer models

Any future extension must either preserve backward compatibility with this baseline or revise the hardware contract through a new milestone and documentation update.

## Planned Future Expansion: YM2151 FM Audio

This section records architectural intent only. YM2151 is planned and approved for a future milestone block, but it is not implemented in the current emulator and is not part of the validated audio platform today.

Current emulator behavior remains PSG-only:
- CPU-visible audio hardware is limited to the PSG-style APU on `0xD0-0xDB`
- no YM2151 register block is exposed in the current hardware model
- no FM synthesis behavior is available to ROM code in the current build

Any YM2151 design, port allocation, timing contract, and validation rules will be defined and verified during the `M29a-M29f` milestone block. Until that work is completed, this document remains authoritative only for the implemented PSG subsystem above.
