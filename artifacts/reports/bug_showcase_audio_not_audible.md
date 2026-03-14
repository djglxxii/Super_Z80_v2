# Bug Report: Showcase ROM Audio Not Audible In Visible SDL Runtime

## Status

Confirmed

## Summary

When the showcase ROM is loaded in the visible SDL runtime, audio can be absent even though the ROM is actively generating PSG and YM2151 output.

The immediate cause is that the emulator exposes audio playback only through the `--sdl-audio` shell, while the `--sdl-input` shell provides the visible interactive runtime without opening an SDL audio device or pumping audio samples.

Because `main()` dispatches `--sdl-input` before `--sdl-audio`, launching with both flags still selects the input shell and remains silent.

## Repository Evidence

### 1. The ROM does generate audio

- [`rom/showcase/src/vdp_init.asm`](/home/djglxxii/src/Super_Z80_v2/rom/showcase/src/vdp_init.asm#L54) initializes audio state, enables the PSG block, and calls `showcase_init_music`.
- [`rom/showcase/src/main_loop.asm`](/home/djglxxii/src/Super_Z80_v2/rom/showcase/src/main_loop.asm#L31) runs `showcase_update_sfx` and `showcase_update_music` every frame.
- [`rom/showcase/src/main_loop.asm`](/home/djglxxii/src/Super_Z80_v2/rom/showcase/src/main_loop.asm#L141) programs YM2151 channel 0.
- [`rom/showcase/src/main_loop.asm`](/home/djglxxii/src/Super_Z80_v2/rom/showcase/src/main_loop.asm#L194) advances the YM2151 note loop continuously.

### 2. The emulator core produces mixed audio samples

- [`emulator/src/scheduler.cpp`](/home/djglxxii/src/Super_Z80_v2/emulator/src/scheduler.cpp#L24) ticks YM2151 and the audio mixer once per scanline.
- [`emulator/src/emulator_core.cpp`](/home/djglxxii/src/Super_Z80_v2/emulator/src/emulator_core.cpp#L180) converts scanline progression into audio sample generation and pushes samples into the core audio buffer.

### 3. Headless verification confirms nonzero audio output

Command run:

```bash
./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 48
```

Observed result:

```text
Emulator core initialized (audio output ready).
HEADLESS_ROM_RESULT rom_crc32=0x3C5D42C4 ram_crc32=0x1834E477 audio_crc32=0x15D6E68A frames=48 frame_counter=48 scanline_counter=0 audio_samples=4096
```

Interpretation:

- `audio_samples=4096` shows the core produced a full audio buffer.
- `audio_crc32=0x15D6E68A` is nonzero and confirms deterministic non-silent sample output.

## Root Cause

### Input shell has no audio playback path

- [`emulator/src/main.cpp`](/home/djglxxii/src/Super_Z80_v2/emulator/src/main.cpp#L786) implements `run_sdl_input_shell`.
- That path creates a window, renderer, frontend, and controller handling.
- It does **not** create `SdlAudioOutput`.
- It does **not** call `pump_audio_output`.
- It therefore cannot produce audible output even if the loaded ROM generates audio internally.

### Audio shell does have playback, but it is a separate mode

- [`emulator/src/main.cpp`](/home/djglxxii/src/Super_Z80_v2/emulator/src/main.cpp#L609) implements `run_sdl_audio_shell`.
- [`emulator/src/main.cpp`](/home/djglxxii/src/Super_Z80_v2/emulator/src/main.cpp#L639) opens the SDL audio device through `SdlAudioOutput`.
- [`emulator/src/main.cpp`](/home/djglxxii/src/Super_Z80_v2/emulator/src/main.cpp#L771) pumps mixed core audio into the SDL device every loop iteration.

### Flag precedence makes combined launch still silent

- [`emulator/src/main.cpp`](/home/djglxxii/src/Super_Z80_v2/emulator/src/main.cpp#L1028) checks `options.use_sdl_input` before `options.use_sdl_audio`.
- If both flags are present, the input shell is selected first.
- Result: the visible interactive mode wins, but audio playback is still absent.

## User-Visible Impact

- A user can load the showcase ROM, see a scene that documents active PSG/YM playback, and still hear nothing.
- A user may reasonably assume audio is broken in the ROM or in the mixer, when the real issue is runtime shell selection and missing audio integration in the input shell.
- Passing both `--sdl-input` and `--sdl-audio` does not recover audio because of current dispatch order.

## Reproduction

### Silent visible runtime

```bash
./build/super_z80 --sdl-input --rom rom/showcase/build/showcase.bin
```

Expected by user:

- visible showcase scene
- interactive input
- audible audio

Actual:

- visible scene
- interactive input
- no SDL audio playback path

### Also silent if both flags are passed

```bash
./build/super_z80 --sdl-input --sdl-audio --rom rom/showcase/build/showcase.bin
```

Actual:

- `main()` selects the input shell first
- no SDL audio device is opened

### Audible mode is separate

```bash
./build/super_z80 --sdl-audio --rom rom/showcase/build/showcase.bin
```

This is the only SDL runtime path in the current code that opens audio playback.

## Recommended Patch Scope

Any milestone generated from this bug should address both behavior and UX clarity.

Minimum acceptable fix options:

1. Integrate SDL audio playback into the visible/input shell so interactive runtime can also be audible.
2. Or unify the SDL input and SDL audio shells into one runtime path that supports window, input, and audio together.
3. Also resolve CLI ambiguity so `--sdl-input --sdl-audio` does not silently pick the non-audio path.

## Commands Run During Investigation

```bash
rg -n "showbase|showcase|YM2151|sdl-audio|audio device|SDL_Audio|audio callback|enqueue|queue audio|PSG" -S .
rg --files emulator src rom tests | sort
sed -n '1,220p' emulator/src/main.cpp
sed -n '220,420p' emulator/src/main.cpp
sed -n '420,760p' emulator/src/main.cpp
sed -n '760,980p' emulator/src/main.cpp
sed -n '1,220p' emulator/src/sdl_audio_output.cpp
sed -n '1,220p' emulator/src/sdl_audio_output.h
sed -n '1,220p' emulator/src/scheduler.cpp
sed -n '1,260p' emulator/src/emulator_core.cpp
sed -n '1,320p' rom/showcase/src/main_loop.asm
sed -n '1,180p' rom/showcase/src/vdp_init.asm
./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 48
ls -l rom/showcase/build
file rom/showcase/build/showcase.bin
```

## Conclusion

The bug is not in showcase ROM audio generation.

The defect is in SDL runtime integration and CLI mode selection:

- the visible interactive shell does not play audio
- the audio-playing shell is separate
- combined flags still select the silent shell
