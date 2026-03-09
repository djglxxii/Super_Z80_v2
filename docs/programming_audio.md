# Super_Z80 Audio Programming Guide

## Scope
This guide covers the implemented audio hardware only: the PSG-style APU on `0xD0-0xDB`. There is no YM2151 or FM programming interface in the current platform.

## Audio Block Summary
- `3` tone channels with 12-bit periods
- `1` noise channel with a 15-bit LFSR
- Mono signed `16-bit` output
- Fixed sample rate: `48000 Hz`

## Register Map
| Port | Name |
| --- | --- |
| `0xD0` | Tone A low |
| `0xD1` | Tone A high (`bits 3:0`) |
| `0xD2` | Tone B low |
| `0xD3` | Tone B high (`bits 3:0`) |
| `0xD4` | Tone C low |
| `0xD5` | Tone C high (`bits 3:0`) |
| `0xD6` | Noise control |
| `0xD7` | Volume A |
| `0xD8` | Volume B |
| `0xD9` | Volume C |
| `0xDA` | Noise volume |
| `0xDB` | Global audio control |

Masked bits:
- Tone high ports keep only bits `3:0`.
- `0xD6` keeps only bits `2:0`.
- Volume ports keep only bits `3:0`.
- `0xDB` accepts only bits `2:0`; bit `7` is read-only overrun status.

## Tone Channel Control
The effective period is:

```text
period = ((HI & 0x0F) << 8) | LO
```

Rules:
- Period `0x000` is silent.
- Non-zero periods drive a 50% duty square wave.
- Phase toggles whenever the internal divider expires.
- Register updates affect future stepping immediately because divider reloads come from the stored period.

## Noise Channel Control
`AUD_NOISE_CTRL` (`0xD6`) layout:
- Bits `1:0`: clock select
- Bit `2`: mode

Clock select:
- `00`: divider `16`
- `01`: divider `32`
- `10`: divider `64`
- `11`: clock from Tone C divider events

Mode:
- `0`: white noise using XOR feedback from LFSR bits `0` and `1`
- `1`: periodic noise by shifting toward zero and reseeding to `0x4000` when the cycle completes

## Volume Control
- Volumes are attenuation-style.
- `0x0` is loudest.
- `0xF` is silent.

Lookup table:

```text
0x0->15  0x1->14  0x2->13  0x3->12
0x4->11  0x5->10  0x6->9   0x7->8
0x8->7   0x9->6   0xA->5   0xB->4
0xC->3   0xD->2   0xE->1   0xF->0
```

## Global Control
`AUD_CTRL` (`0xDB`) bits:
- Bit `0`: enable
- Bit `1`: mute
- Bit `2`: reset runtime state
- Bit `7`: sample queue overrun status on read

Behavior:
- `enable=0` forces silence.
- `mute=1` forces silence but channel state still advances.
- Writing bit `2` resets tone phases, divider counters, noise LFSR, current sample state, and the overrun latch.
- Bit `2` self-clears and reads back as `0`.

## Recommended Programming Patterns
- Initialize all three tone periods and all four volumes explicitly after reset.
- Set `AUD_CTRL` bit `0` only after channel registers are programmed.
- Use `AUD_CTRL` bit `2` when you need a deterministic phase restart, such as starting title music or a sound test from a known state.
- Treat Tone C clocked noise as coupled behavior: changing Tone C period changes the noise cadence when `0xD6[1:0] == 3`.
- Use volume `0xF` to silence an individual channel without disabling the global mixer.

## Practical Notes
- The platform output is mono only.
- Sample timing is scheduler-owned. Software should think in scanlines and register writes, not host time.
- SDL playback in the shell is only a sink for samples already produced by the deterministic core.

## Future FM Support
YM2151-based FM audio is planned but not yet available in the current emulator build.

Do not target FM hardware until the `M29a-M29f` milestone block is completed. Until then, write audio code only against the implemented PSG-style registers on `0xD0-0xDB`.
