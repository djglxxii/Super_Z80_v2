# Super_Z80 Input Programming Guide

## Ports
- `PAD1` at `0xDC`
- `PAD1_SYS` at `0xDD`

Both ports are active-low.

## Button Bit Mapping
### `PAD1` (`0xDC`)
- Bit `0`: Up
- Bit `1`: Down
- Bit `2`: Left
- Bit `3`: Right
- Bit `4`: A
- Bit `5`: B
- Bits `6-7`: unused, read high

### `PAD1_SYS` (`0xDD`)
- Bit `0`: Start
- Bits `1-7`: unused, read high

## Idle Values
- `PAD1 = 0xFF`
- `PAD1_SYS = 0xFF`

Pressed buttons clear their assigned bits to `0`.

Examples:
- Up pressed only: `PAD1 = 0xFE`
- Start pressed only: `PAD1_SYS = 0xFE`
- Up + Right + A pressed: `PAD1 = 0xE6`

## CPU Access
Games read the ports with standard Z80 `IN` instructions.

Example:

```asm
    in   a,($DC)    ; read directional/A/B buttons
    in   b,($DD)    ; read Start
```

The port low byte is authoritative because the bus masks the CPU port address to `8` bits.

## Recommended Polling Pattern
- Poll once per gameplay update or once per frame.
- Read both `0xDC` and `0xDD` back-to-back so button state stays coherent for that update.
- Invert or mask the active-low bits in game logic rather than rewriting port values.

Example bit test flow:

```text
raw = IN 0xDC
pressed = ~raw
if (pressed & 0x10) != 0 then A is down
```

## Write Behavior
- Direct writes to `0xDC` and `0xDD` are ignored.
- Input state is owned by the shell layer or test harness, not by CPU-written latch registers.

## SDL Shell Mapping
The optional SDL input shell maps:
- Arrow keys or d-pad to `Up/Down/Left/Right`
- `Z` or controller `A` to button `A`
- `X` or controller `B` to button `B`
- `Enter` or controller `Start` to `Start`

This mapping is a development convenience, not extra hardware.
