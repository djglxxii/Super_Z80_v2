# Super_Z80_v2 State Snapshot

## Current Milestone
M63

## Audio Status
Current validated audio implementation:
- PSG-style APU plus YM2151 FM audio mixed into the platform sample stream

YM2151 FM expansion is implemented on ports `0x70-0x71`.
- deterministic register interface, operator/channel state, timer/status/IRQ-facing behavior, FM sample generation, and mixed-output integration are complete
- the scheduler owns PSG/YM sample mixing before samples enter the existing headless or SDL-facing platform output path

Audio Progress

Completed:
- M26a — Audio Registers & State
- M26b — Audio Tone and Noise Generator State Progression
- M26c — Audio Mixer and Internal Sample Output State
- M26d — SDL Audio Output Integration
- M26e — Audio Deterministic Validation and Tests

YM2151 / FM Expansion Completed:
- M29a — YM2151 Register Interface
- M29b — YM2151 Operator and Channel State Model
- M29c — YM2151 Timers, Status Bits, and IRQ Behavior
- M29d — YM2151 FM Sample Generation
- M29e — PSG + YM2151 Mixer Integration
- M29f — Deterministic FM / Audio Validation
- M29g — SDL Audio Output Integration

M26d and M29g both cover SDL output milestones.
M26d established the baseline PSG host-output path, while M29g updated the host integration layer to expose the already-produced mixed PSG + YM2151 stream.
M29g is host integration only and does not change emulator hardware semantics.

PCM remains excluded from the platform design.

## Recent Changes
- M63 complete.
- The visible SDL interactive runtime now opens the SDL audio sink and pumps `EmulatorCore` mixed samples through the existing `SdlAudioOutput` path, so `--sdl-input` no longer runs silently when a ROM generates PSG or YM2151 audio.
- SDL CLI dispatch now routes `--sdl-input`, `--sdl-audio`, and `--sdl-input --sdl-audio` through the same audio-capable interactive runtime path, preserving the legacy audio-only entry flag while removing the previous flag-order ambiguity in `main()`.
- Interactive runtime startup, ROM load/reset, and in-memory snapshot restore now clear and repopulate the SDL audio queue from the shell layer, while failed SDL audio-device initialization degrades to silent execution instead of aborting the visible runtime.
- Deterministic headless execution remains unchanged and still succeeds with `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 1`, producing `HEADLESS_ROM_RESULT rom_crc32=0x3C5D42C4 ram_crc32=0xB957D880 audio_crc32=0x8F537EF9`.
- Extended headless verification also succeeds with `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 48`, producing `HEADLESS_ROM_RESULT rom_crc32=0x3C5D42C4 ram_crc32=0x1834E477 audio_crc32=0x15D6E68A`.
- CI-safe SDL smoke checks now succeed under dummy/software drivers for `--sdl-input`, `--sdl-audio`, and `--sdl-input --sdl-audio`, confirming the unified interactive runtime stays stable until expected `timeout` termination even when dummy audio/video drivers are used.
- M60 complete.
- The runtime display pipeline now renders a complete VDP framebuffer at scheduler frame wrap and exposes a read-only `EmulatorCore` framebuffer snapshot for frontend presentation without moving display ownership into the core.
- The SDL frontend now owns an emulator framebuffer texture path that uploads RGBA pixels, preserves the existing integer display scale presets with centered aspect-correct presentation, and draws emulator video before the ImGui debug overlay.
- Headless ROM execution remains unchanged and still succeeds with `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 1`, producing `HEADLESS_ROM_RESULT rom_crc32=0xD7F53636 ram_crc32=0x9A494230 audio_crc32=0xC5117D35`.
- CI-safe SDL smoke checks still succeed under dummy/software drivers for both `--sdl-input --rom rom/showcase/build/showcase.bin` and `--sdl-audio --rom rom/showcase/build/showcase.bin`, confirming the framebuffer presentation path integrates without startup regressions.
- Added narrow automated coverage proving the scheduler renders the VDP framebuffer at frame wrap and that `EmulatorCore` can return a read-only rendered framebuffer snapshot for frontend use.
- The canonical frontend plan now sequences `M60 - Display Pipeline Bring-Up`, `M61 - Unified SDL Interactive Runtime Audio`, `M62 - ROM Browser Integration`, and `M63 - Interactive SDL Audio Runtime Parity Fix`; the previously completed ROM browser integration remains `M62`, and the newly confirmed visible-runtime audio gap is now tracked as post-M62 corrective milestone `M63`.
- M62 complete.
- The frontend `File -> Load ROM...` flow now opens an in-application ImGui ROM browser that lists the current directory, supports parent/up navigation, allows entering subdirectories, and can load a selected ROM file without manually typing its full path.
- The ROM browser keeps the existing manual path field as a narrow fallback, marks likely ROM file extensions in the browser list, and reuses the existing runtime-owned ROM load/reset path so reload behavior and emulator-core semantics remain unchanged.
- Frontend settings persistence now stores `rom_browser_directory=<path>` alongside `display_scale=<n>` in `super_z80_frontend_settings.cfg`, restoring the last browsed directory when available and otherwise defaulting to the current ROM directory or process working directory.
- Directory enumeration and invalid-path handling stay frontend-local and fail safely with visible status messaging rather than crashing, while headless execution and deterministic scheduler behavior remain unchanged.
- Headless ROM execution remains unchanged and still succeeds with `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 1`, producing `HEADLESS_ROM_RESULT rom_crc32=0xD7F53636 ram_crc32=0x9A494230 audio_crc32=0xC5117D35`.
- CI-safe SDL smoke checks still succeed under dummy/software drivers for both `--sdl-input --rom rom/showcase/build/showcase.bin` and `--sdl-audio --rom rom/showcase/build/showcase.bin`, and an additional temporary-home startup check succeeds with a seeded persisted `rom_browser_directory=rom/showcase/build` configuration.
- Automated verification confirms build, tests, headless execution, and SDL frontend startup with the new browser-capable settings path; direct manual confirmation of interactive browser opening, directory navigation, and click-driven ROM selection in a real SDL session is still recommended.
- M59 complete.
- The SDL frontend now exposes a `View` menu with integer display scale presets `2x`, `4x`, and `6x`, and changing the selected scale resizes the active SDL window immediately without touching emulator-core timing or headless execution.
- SDL frontend startup now derives its initial window size from the native `256x192` framebuffer dimensions multiplied by the persisted or default scale, so both the input and audio shells open consistently at `512x384`, `1024x768`, or `1536x1152`.
- Frontend display scale persistence now uses a narrow SDL preference-directory-backed `super_z80_frontend_settings.cfg` file storing a single `display_scale=<n>` value, with fallback to the previous working directory when a preference path is unavailable.
- Headless ROM execution remains unchanged and still succeeds with `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 1`, producing `HEADLESS_ROM_RESULT rom_crc32=0xD7F53636 ram_crc32=0x9A494230 audio_crc32=0xC5117D35`.
- CI-safe SDL smoke checks still succeed under dummy/software drivers for both `--sdl-input --rom rom/showcase/build/showcase.bin` and `--sdl-audio --rom rom/showcase/build/showcase.bin`, and an additional temporary-home startup check succeeds with a seeded persisted `display_scale=4` configuration.
- Automated verification confirms build, tests, headless execution, and persisted-scale SDL startup behavior; manual visual confirmation of actual on-screen window dimensions and menu-driven scale changes in a real desktop session is still recommended.
- M58 complete.
- Frontend stabilization now clamps first-use ImGui window placement and sizing against the active viewport work area so the default debug panels stay visible inside the smaller `640x480` SDL shell windows instead of spawning off-screen.
- Frontend layout persistence now uses Dear ImGui's built-in ini load/save flow through an SDL preference-directory-backed `super_z80_frontend_layout.ini` path, with fallback to the previous local filename when a preference path is unavailable.
- Frontend startup and shutdown continue to tolerate missing or malformed layout files through the existing Dear ImGui ini parser behavior, and explicit save-on-shutdown now preserves panel visibility and arrangement without introducing a custom persistence system.
- Headless ROM execution remains unchanged and still succeeds with `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 1`, producing `HEADLESS_ROM_RESULT rom_crc32=0xD7F53636 ram_crc32=0x9A494230 audio_crc32=0xC5117D35`.
- CI-safe SDL smoke checks still succeed under dummy/software drivers for both `--sdl-input --rom rom/showcase/build/showcase.bin` and `--sdl-audio --rom rom/showcase/build/showcase.bin`, confirming the stabilized frontend startup path without changing headless execution behavior.
- Automated verification confirms build, tests, headless execution, and frontend startup behavior; manual visual confirmation of final panel placement, menu toggling, and restored layouts in a real SDL session is still recommended.
- M57 complete.
- The frontend now presents an integrated ImGui debug overlay with a top-level `Debug` menu that controls visibility for `Emulator Control`, `CPU Debug`, `Memory Viewer`, `VRAM Viewer`, `Sprite Debug`, `DMA Debug`, `Audio Debug`, `Input Visualization`, and `Frame Timing & Scheduler`.
- Added frontend-owned explicit panel visibility state plus a new `Debug Overview` workspace window so existing developer tools can be shown selectively without changing emulator execution, scheduler timing, or subsystem ownership.
- Existing debug windows now use stable first-use placement and sizing hints, and the frontend writes ImGui layout state to `super_z80_frontend_layout.ini` so panel layout and visibility persist across SDL runs without adding a custom persistence system.
- Headless ROM execution remains unchanged and still succeeds with `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 1`, producing `HEADLESS_ROM_RESULT rom_crc32=0xD7F53636 ram_crc32=0x9A494230 audio_crc32=0xC5117D35`.
- CI-safe SDL smoke checks still succeed under dummy/software drivers for both `--sdl-input --rom rom/showcase/build/showcase.bin` and `--sdl-audio --rom rom/showcase/build/showcase.bin`, confirming the integrated overlay startup path without changing headless execution behavior.
- Automated verification confirms frontend startup and persistence-path creation, but visual confirmation of menu-driven panel toggling and final arrangement still requires manual inspection in a real SDL session.
- The next official frontend milestone is now `M58 - Frontend Stabilization`.
- M56 complete.
- The frontend control panel now exposes single-slot `Save Snapshot` and `Restore Snapshot` actions for the SDL input and SDL audio shells, with visible status messaging and graceful failure when no snapshot has been captured yet.
- Added a core-owned aggregate emulator snapshot/restore path that captures CPU, bus-owned subsystems, scheduler/mixer state, and buffered audio queue state in memory without introducing file I/O, host-timing dependencies, or UI coupling into deterministic emulator subsystems.
- Expanded subsystem snapshot coverage to support full round-trip restoration for CPU internal Z80ex state, RAM/ROM, VDP, DMA, IRQ/VBlank, controller input, PSG audio, YM2151 state, scheduler counters, and buffered core audio state.
- Core unit coverage now verifies deterministic restore behavior by capturing a snapshot, advancing execution, restoring, advancing again, and confirming identical CPU/timing/RAM/audio outcomes plus identical buffered audio sample output.
- Headless ROM execution remains unchanged and still succeeds with `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 1`, producing `HEADLESS_ROM_RESULT rom_crc32=0xD7F53636 ram_crc32=0x9A494230 audio_crc32=0xC5117D35`.
- CI-safe SDL smoke checks still succeed under dummy/software drivers for both `--sdl-input --rom rom/showcase/build/showcase.bin` and `--sdl-audio --rom rom/showcase/build/showcase.bin`, confirming snapshot-enabled frontend startup paths without changing headless execution behavior.
- The next official frontend milestone is now `M57 - Integrated Debug Overlay`.
- M55 complete.
- The frontend now exposes a read-only `Frame Timing & Scheduler` ImGui panel that refreshes every rendered frame and displays live frame and scanline counters, scheduler timing constants, VBlank state, frame-ready state, and buffered audio sample count for developer inspection.
- Added a narrow read-only timing snapshot accessor on `EmulatorCore` so the frontend can inspect scheduler-owned timing and related runtime counters without holding mutable subsystem references or altering deterministic execution flow.
- Core unit coverage now verifies timing snapshots across reset, scanline advance, frame-ready, VBlank, frame wrap, and buffered audio sample progression.
- Headless ROM execution remains unchanged and still succeeds with `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 1`, producing `HEADLESS_ROM_RESULT rom_crc32=0xD7F53636 ram_crc32=0x9A494230 audio_crc32=0xC5117D35`.
- CI-safe SDL smoke checks still succeed under dummy/software drivers for both `--sdl-input --rom rom/showcase/build/showcase.bin` and `--sdl-audio --rom rom/showcase/build/showcase.bin`, confirming the frame timing panel startup path without changing headless execution behavior.
- The next official frontend milestone is now `M56 - Emulator Snapshot Tools`.
- M54 complete.
- The frontend now exposes a read-only `Input Visualization` ImGui panel that refreshes every rendered frame and displays the currently supported controller buttons plus raw active-low `PAD1` and `PAD1_SYS` port-visible values for developer inspection.
- Added a narrow read-only input snapshot accessor on `IO`, exposed through `Bus` and `EmulatorCore`, so the frontend can inspect live controller state without holding mutable input references or altering deterministic execution flow.
- Core and input unit coverage now verify logical button snapshot state alongside the raw active-low port values surfaced to the frontend.
- Headless ROM execution remains unchanged and still succeeds with `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 1`, producing `HEADLESS_ROM_RESULT rom_crc32=0xD7F53636 ram_crc32=0x9A494230 audio_crc32=0xC5117D35`.
- CI-safe SDL smoke checks still succeed under dummy/software drivers for both `--sdl-input --rom rom/showcase/build/showcase.bin` and `--sdl-audio --rom rom/showcase/build/showcase.bin`, confirming the input visualization panel startup path without changing headless execution behavior.
- The next official frontend milestone is now `M55 - Frame Timing & Scheduler Panel`.
- M53 complete.
- The frontend now exposes a read-only `Audio Debug` ImGui panel that refreshes every rendered frame and displays PSG register/state, YM2151 register/state, per-channel activity summaries, timer/IRQ status, and current mixed-sample indicators for developer inspection.
- Added narrow read-only audio snapshot accessors on `APU` and `YM2151`, then exposed a combined audio snapshot through `EmulatorCore` so the frontend can inspect live audio subsystem state without holding mutable core references or altering deterministic execution flow.
- Core unit coverage now verifies audio snapshots for both PSG and YM2151 state, including raw register values, PSG enable/control state, and YM2151 decoded channel activity.
- Headless ROM execution remains unchanged and still succeeds with `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 1`, producing `HEADLESS_ROM_RESULT rom_crc32=0xD7F53636 ram_crc32=0x9A494230 audio_crc32=0xC5117D35`.
- CI-safe SDL smoke checks still succeed under dummy/software drivers for both `--sdl-input --rom rom/showcase/build/showcase.bin` and `--sdl-audio --rom rom/showcase/build/showcase.bin`, confirming the audio debug panel startup path without changing headless execution behavior.
- The next official frontend milestone is now `M54 - Input Visualization Panel`.
- M52 complete.
- The frontend now exposes a read-only `DMA Debug` ImGui panel that displays the live DMA source address, destination address, transfer length, CPU-visible control register value, and active/idle status refreshed every rendered frame.
- Added a narrow read-only DMA snapshot accessor on `DMA`, exposed through `EmulatorCore`, so the frontend can inspect DMA register state without holding mutable core references or altering deterministic execution flow.
- Core unit coverage now verifies DMA snapshots in both idle and active states, including source/destination register values, transfer length, and the busy-bit control value.
- Headless ROM execution remains unchanged and still succeeds with `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 1`, producing `HEADLESS_ROM_RESULT rom_crc32=0xD7F53636 ram_crc32=0x9A494230 audio_crc32=0xC5117D35`.
- CI-safe SDL smoke checks still succeed under dummy/software drivers for both `--sdl-input --rom rom/showcase/build/showcase.bin` and `--sdl-audio --rom rom/showcase/build/showcase.bin`, confirming the DMA debug panel startup path without changing headless execution behavior.
- The next official frontend milestone is now `M53 - Audio Debug Panel`.
- M51 complete.
- The frontend now exposes a read-only `Sprite Debug` ImGui panel that displays all `64` sprite attribute table entries with sprite index, `X`, `Y`, tile index, and raw attribute-byte values refreshed every rendered frame.
- Added a narrow read-only sprite table snapshot accessor on `EmulatorCore` that decodes SAT entries from VDP VRAM using the existing `Y/X/Tile/Attr` layout without exposing mutable VDP references or altering deterministic execution flow.
- Core unit coverage now verifies that sprite table snapshots reflect SAT writes for both the first and last sprite entries and remain bounded to the platform `64`-sprite limit.
- Headless ROM execution remains unchanged and still succeeds with `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 1`, producing `HEADLESS_ROM_RESULT rom_crc32=0xD7F53636 ram_crc32=0x9A494230 audio_crc32=0xC5117D35`.
- CI-safe SDL smoke checks still succeed under dummy/software drivers for both `--sdl-input --rom rom/showcase/build/showcase.bin` and `--sdl-audio --rom rom/showcase/build/showcase.bin`, confirming the sprite debug panel startup path without changing headless execution behavior.
- The next official frontend milestone is now `M52 - DMA Debug Panel`.
- M50 complete.
- The frontend now exposes a read-only `VRAM Viewer` ImGui panel with clamped hex start-address navigation across the full `0x0000-0xFFFF` VDP VRAM range and a 16-bytes-per-row raw hex display that refreshes every rendered frame.
- Added a narrow read-only VRAM snapshot accessor on `EmulatorCore`, then routed that snapshot through the existing shell-owned frontend runtime-state handoff so the UI can inspect live VDP memory without holding mutable core references or changing deterministic execution flow.
- Core unit coverage now verifies that VRAM snapshots reflect writes at the start, middle, and end of the `64 KiB` VRAM space.
- Headless ROM execution remains unchanged and still succeeds with `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 1`, producing `HEADLESS_ROM_RESULT rom_crc32=0xD7F53636 ram_crc32=0x9A494230 audio_crc32=0xC5117D35`.
- CI-safe SDL smoke checks still succeed under dummy/software drivers for both `--sdl-input --rom rom/showcase/build/showcase.bin` and `--sdl-audio --rom rom/showcase/build/showcase.bin`, confirming the VRAM viewer startup path without changing headless execution behavior.
- The next official frontend milestone is now `M51 - Sprite Debug Panel`.
- M49 complete.
- The frontend now exposes a read-only `Memory Viewer` ImGui panel with ROM/RAM region selection, hex-oriented paging, and clamped start-address navigation for the `0x0000-0x7FFF` ROM window and `0xC000-0xFFFF` RAM window.
- Added narrow read-only ROM and RAM snapshot accessors on `EmulatorCore`, then routed those snapshots through the existing shell-owned frontend runtime-state handoff so the UI can inspect memory without holding mutable core references or changing deterministic execution flow.
- Core unit coverage now verifies that ROM/RAM snapshots reflect loaded ROM bytes, open-bus-filled ROM slack, and live RAM writes at both the start and end of the mapped RAM window.
- Headless ROM execution remains unchanged and still succeeds with `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 1`, producing `HEADLESS_ROM_RESULT rom_crc32=0xD7F53636 ram_crc32=0x9A494230 audio_crc32=0xC5117D35`.
- CI-safe SDL smoke checks still succeed under dummy/software drivers for both `--sdl-input --rom rom/showcase/build/showcase.bin` and `--sdl-audio --rom rom/showcase/build/showcase.bin`, confirming the memory viewer startup path without changing headless execution behavior.
- The next official frontend milestone is now `M50 - VRAM Viewer`.
- M48 complete.
- The frontend now exposes a read-only `CPU Debug` ImGui panel that refreshes every rendered frame and displays the live Z80 register set, including `AF/BC/DE/HL`, `PC`, `SP`, `IX`, `IY`, `I`, `R`, decoded `S/Z/H/PV/N/C` flags, and interrupt/HALT state.
- Added a narrow read-only CPU snapshot path from `superz80::CPU` through `EmulatorCore` into the frontend runtime-state handoff, preserving the existing shell-owned control flow and avoiding any UI writes into deterministic core logic.
- CPU unit coverage now verifies the new register snapshot accessor against Z80ex execution state after instruction execution and HALT.
- Headless ROM execution remains unchanged and still succeeds with `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 1`, producing `HEADLESS_ROM_RESULT rom_crc32=0xD7F53636 ram_crc32=0x9A494230 audio_crc32=0xC5117D35`.
- CI-safe SDL smoke checks still succeed under dummy/software drivers for both `--sdl-input --rom rom/showcase/build/showcase.bin` and `--sdl-audio --rom rom/showcase/build/showcase.bin`, confirming the CPU panel startup path without changing headless execution behavior.
- The next official frontend milestone is now `M49 - Memory Viewer`.
- M47 complete.
- The frontend now exposes a top-level `File` menu with `Load ROM...` and `Reload ROM`, using a minimal ImGui modal text-entry flow for interactive ROM path entry without adding native file dialog dependencies.
- Frontend-issued ROM load/reload requests now flow through shell-owned runtime state and reuse the existing `initialize()` plus `load_rom()` path, preserving deterministic core boundaries while resetting execution cleanly from the loaded ROM.
- SDL input and SDL audio shells now accept optional `--rom <path>` startup ROMs, track the active ROM path for reset/reload, and surface load success or failure through both the frontend status area and shell stderr/stdout messages.
- SDL audio output now clears its host queue on ROM load/reset so interactive ROM switches do not leak stale pre-reset samples into the restarted runtime.
- Headless ROM execution remains unchanged and still succeeds with `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 1`, producing `HEADLESS_ROM_RESULT rom_crc32=0xD7F53636 ram_crc32=0x9A494230 audio_crc32=0xC5117D35`.
- CI-safe SDL smoke checks succeed under dummy/software drivers for both `--sdl-input --rom rom/showcase/build/showcase.bin` and `--sdl-audio --rom rom/showcase/build/showcase.bin`, confirming the shell-owned ROM loading path used by the new frontend workflow.
- Invalid ROM startup paths now fail clearly in SDL runtime startup, for example `./build/super_z80 --sdl-input --rom /tmp/does_not_exist.bin` reports `Unable to open ROM file: /tmp/does_not_exist.bin`.
- The next official frontend milestone is now `M48 - CPU Debug Panel`.
- M46 complete.
- The frontend now exposes an `Emulator Control` ImGui window with run/pause, reset, and single-frame stepping controls plus live execution state and frame-counter text.
- SDL shell loops now route frontend-issued runtime commands through shell-owned control state, stepping exactly `Scheduler::kScanlinesPerFrame` for both continuous execution and paused single-frame stepping without modifying deterministic core subsystem logic.
- Runtime reset now reuses the existing startup initialization path from the shell layer, and the SDL audio shell reapplies its demo tone setup after reset so host-facing behavior matches shell startup semantics.
- Headless ROM execution remains unchanged and still succeeds with `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 1`, producing `HEADLESS_ROM_RESULT rom_crc32=0xD7F53636 ram_crc32=0x9A494230 audio_crc32=0xC5117D35`.
- CI-safe SDL smoke checks still succeed under dummy/software drivers for both `--sdl-input` and `--sdl-audio`, confirming frontend startup plus the control-panel path without changing headless execution behavior.
- The next official frontend milestone is now `M47 - ROM Loader Interface`.
- M45 complete.
- Dear ImGui is now vendored under `emulator/vendor/imgui/` and wired into the SDL-capable frontend path using the SDL2 + SDLRenderer2 backends.
- The frontend lifecycle now owns SDL event forwarding, ImGui frame setup, and rendering of a minimal `Super_Z80 Frontend` diagnostic window without moving UI concerns into deterministic emulator core code.
- The SDL input shell and SDL audio shell now create an `SDL_Renderer`, clear/present each frame, and render the frontend overlay while preserving the existing headless execution path.
- Headless ROM execution still succeeds with `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 1`, producing `HEADLESS_ROM_RESULT rom_crc32=0xD7F53636 ram_crc32=0x9A494230 audio_crc32=0xC5117D35`.
- CI-safe SDL smoke checks now succeed under dummy/software drivers for both `--sdl-input` and `--sdl-audio`, confirming SDL startup plus the ImGui render path without changing hardware behavior.
- The next official frontend milestone is now `M46 - Emulator Control Panel`.
- M44 complete.
- Added the initial frontend module scaffold under `emulator/frontend/`, including a narrow `Frontend` lifecycle interface and a stub `DebugPanelHost` for future debugging panel registration.
- The SDL shell entry paths now construct the frontend scaffold and run no-op frontend begin/end frame hooks without changing headless execution or emulator core behavior.
- Build-system integration now compiles the frontend scaffold as its own library while keeping the deterministic core and shell boundaries explicit.
- The next official frontend milestone is now `M45 - ImGui Integration`.
- M43 complete.
- The Showcase ROM source is now annotated as the developer reference implementation, with comments that explain startup ordering, deterministic frame sequencing, CPU-side scene composition, VRAM upload order, metasprite SAT expansion, and the separation between update logic and hardware-visible render writes.
- Added `docs/showcase_programmers_guide.md` as the programmer-facing reference for the Showcase ROM, covering build flow, source layout, text rendering, scrolling/parallax, metasprites, controller input, PSG sound effects, YM2151 playback, the integrated scene, and deterministic verification commands.
- Repeated `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 48` runs remain byte-identical with `HEADLESS_ROM_RESULT rom_crc32=0xD7F53636 ram_crc32=0x9A494230 audio_crc32=0xD8F49994`.
- The Showcase ROM is now considered the canonical developer reference ROM for the platform.
- M42 complete.
- The Showcase ROM now uses a foreground text overlay that explicitly identifies the scene as the `SUPER Z80 PLATFORM SHOWCASE`, documents `D-PAD MOVE  A:SOUND`, and calls out that YM2151 music and PSG sound effects are active together.
- Existing Phase 5 behavior is now presented as one coherent final demo scene without changing hardware behavior: the parallax background/foreground scroll updates, controller-driven metasprite movement, sprite animation cadence, YM2151 note loop, and PSG `A`-button edge trigger all remain in the same deterministic frame order.
- Repeated `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 48` runs remain byte-identical with `HEADLESS_ROM_RESULT rom_crc32=0xD7F53636 ram_crc32=0x9A494230 audio_crc32=0xD8F49994`.
- The next official Showcase milestone is now `M43 - Developer Reference ROM Finalization`.
- M41 complete.
- Verified the Showcase ROM's existing frame update already demonstrates simultaneous audio mixing by running the YM2151 music step and PSG sound-effect step independently in the same deterministic loop without shared register state.
- Repeated `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 32` runs remain byte-identical with `HEADLESS_ROM_RESULT rom_crc32=0xFCEDF42B ram_crc32=0x9A494230 audio_crc32=0xD8F49994`.
- The next official Showcase milestone is now `M42 - Integrated Showcase Scene`.
- M40 complete.
- The Showcase ROM now initializes deterministic FM playback state during boot by disabling YM2151 timers, keying off all channels, and programming a minimal channel `0` voice through explicit ROM-local register writes before the existing scene starts.
- `showcase_update` now includes a narrow `showcase_update_music` step that advances `SHOWCASE_MUSIC_NOTE_INDEX` and `SHOWCASE_MUSIC_NOTE_TIMER` once per frame, keeping the FM behavior deterministic and easy to study without changing the existing parallax, metasprite, controller, or PSG sound-effect flows.
- The Showcase ROM now runs a continuous four-note YM2151 loop on channel `0`, rewriting frequency/block registers and retriggering all four operators every `16` frames so the scene carries simple background music without adding a generalized music engine.
- Repeated `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 32` runs remain byte-identical with `HEADLESS_ROM_RESULT rom_crc32=0xFCEDF42B ram_crc32=0x9A494230 audio_crc32=0xD8F49994`.
- The next official Showcase milestone is now `M41 - Audio Mixing Demonstration`.
- M39 complete.
- The Showcase ROM now performs an explicit ROM-local PSG initialization step during boot, leaving all channels silent, enabling PSG output deterministically, and seeding `SHOWCASE_PAD1_PREV_STATE` plus `SHOWCASE_SFX_TIMER` in RAM.
- `showcase_update` now checks `PAD1` button `A` on a press edge before the existing scene update logic, writes tone channel A period `$0060` and volume `$02` directly to PSG ports, and stops the effect after a fixed `10`-frame countdown without disturbing parallax, metasprite movement, or sprite animation.
- Headless execution still receives idle controller state, so repeated `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 24` runs remain byte-identical with `HEADLESS_ROM_RESULT rom_crc32=0xCDD4CAD9 ram_crc32=0x9A494230 audio_crc32=0xD8F49994`.
- The next official Showcase milestone is now `M40 - YM2151 Music Playback`.
- Showcase controller-input enhancement reconciled as `M38a - Controller Interaction Enhancement` after `M38`.
- The Showcase ROM now uses the existing once-per-frame PAD1 polling path to drive the metasprite origin directly from active-low directional input, storing the sampled controller state in deterministic RAM before update/render work runs.
- `showcase_update` now applies `LEFT`/`RIGHT` to `meta_x` and `UP`/`DOWN` to `meta_y` in `1`-pixel steps per frame while leaving the validated parallax background/foreground scroll updates and the fixed `8`-frame animation cadence unchanged.
- Headless execution still receives the deterministic idle controller state, so repeated `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 24` runs remain byte-identical with `HEADLESS_ROM_RESULT rom_crc32=0xA48747B5 ram_crc32=0x9A494230 audio_crc32=0xD8F49994`.
- The next official Showcase milestone remains `M39 - PSG Sound Effects`.
- M38 complete.
- The Showcase ROM sprite demo now stores a single logical metasprite origin in deterministic `meta_x` and `meta_y` RAM state, with all four hardware sprites deriving their positions from that shared base plus fixed `8`-pixel offsets.
- The local Showcase sprite asset now contains two `2x2` animation frames laid out as four `8x8` tiles per frame, so each SAT entry references its own tile while still forming one logical moving object.
- `showcase_render_sprite` now rewrites SAT entries `0` through `3` each frame, selecting `showcase_demo_sprite_tile_base + (sprite_frame * 4)` as the frame-local tile block and preserving the existing parallax scene plus `+1` horizontal movement.
- Repeated `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 20` runs remain byte-identical with `HEADLESS_ROM_RESULT rom_crc32=0xBBF8B4A9 ram_crc32=0x9A494230 audio_crc32=0xD8F49994`.
- M37 complete.
- The Showcase ROM sprite demo now includes two visually distinct local animation frames plus deterministic `sprite_frame` and `sprite_anim_counter` RAM state, both initialized during boot and advanced only from the main loop update path.
- The single Showcase sprite still moves `+1` pixel horizontally per frame above the parallax scene, and it now toggles its SAT tile index every `8` frames by rewriting entry `0` with `showcase_demo_sprite_tile_base + sprite_frame`.
- The M36 SAT update flow remains unchanged apart from tile selection, preserving the validated VRAM pointer-load helper and explicit per-frame SAT rewrite contract.
- Repeated `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 16` runs remain byte-identical with `HEADLESS_ROM_RESULT rom_crc32=0xCAEAE31F ram_crc32=0x9A494230 audio_crc32=0xD8F49994`.
- M36 complete.
- The Showcase ROM now includes a local `8x8` sprite tile asset, deterministic `sprite_x` / `sprite_y` RAM state, and a render path that rewrites sprite attribute table entry `0` each frame.
- The parallax Showcase scene now demonstrates a high-priority sprite moving `+1` pixel horizontally per frame while the background and foreground layers continue their existing independent scroll rates.
- `showcase_upload_scene_to_vram` now uploads the sprite pattern tile and explicitly targets the BG tilemap, FG tilemap, and SAT VRAM bases through the VDP address helper instead of relying on reset-only sequential streaming.
- The VDP control port now supports low-byte then high-byte VRAM pointer loads, resolving the previous ROM-side defect that blocked deterministic SAT updates after boot while preserving existing control-port readback behavior.
- Repeated `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 12` runs remain byte-identical with `HEADLESS_ROM_RESULT rom_crc32=0xDBDAA92D ram_crc32=0x9A494230 audio_crc32=0xD8F49994`.
- M35 complete.
- The Showcase ROM now allocates distinct background and foreground tilemap buffers plus explicit `background_scroll_x`, `background_scroll_y`, `foreground_scroll_x`, and `foreground_scroll_y` state bytes in RAM, keeping the parallax logic obvious and deterministic.
- The background plane now combines the existing splash tilemap with repeated pattern bands, while the foreground plane uses transparent text banners and labels so both layers remain visible as separate moving surfaces.
- `showcase_upload_scene_to_vram` now uploads both tilemaps into the existing BG and FG VDP regions, and `showcase_render` writes both sets of layer scroll registers each frame.
- The parallax demo advances the background layer by `+1/+1` pixels per frame and the foreground layer by `+2/+0`, producing a clearly visible depth effect without adding scene management, sprites, or new assets.
- Repeated `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 10` runs remain byte-identical with the verified `HEADLESS_ROM_RESULT` recorded in `artifacts/reports/m35_parallax_scrolling_demo.md`.
- M34 complete.
- The Showcase ROM now keeps deterministic `scroll_x` / `scroll_y` state in RAM, increments both axes once per frame inside `showcase_update`, and writes the current values to the VDP background scroll registers during `showcase_render`.
- The existing splash tilemap and reusable M31 text composition path now serve as a combined X/Y scrolling Showcase scene, visibly demonstrating vertical scroll register behavior without introducing scene management or changing the asset pipeline.
- Repeated `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 8` runs remain byte-identical with the verified `HEADLESS_ROM_RESULT` recorded in `artifacts/reports/m34_vertical_scrolling_demo.md`.
- M33 complete.
- The Showcase ROM now keeps deterministic `scroll_x` / `scroll_y` state in RAM, increments horizontal scroll once per frame inside `showcase_update`, and writes the current values to the VDP background scroll registers during `showcase_render`.
- The existing splash tilemap and reusable M31 text composition path now serve as the first dynamic Showcase scene, visibly moving across frames without introducing scene management or changing the asset pipeline.
- Repeated `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 8` runs remain byte-identical with `HEADLESS_ROM_RESULT rom_crc32=0xA0F7C5B ram_crc32=0x9A494230 audio_crc32=0xD8F49994`.
- M32 complete.
- The Showcase ROM now boots into a deterministic system splash that combines the local splash tile/tilemap assets with the reusable M31 text path to render `SUPER Z80 SHOWCASE` and `SYSTEM SPLASH OK`.
- Fixed a real Showcase integration defect where the splash tilemap referenced raw low tile indices even though the splash assets are uploaded after the font block; the tilemap now targets the correct splash tile base in VRAM.
- Updated the local splash asset set and layout so the ROM presents a clearer centered startup graphic while preserving the existing simple init path and static deterministic behavior.
- M31 complete.
- Replaced the Showcase ROM's hardcoded offset-based line writer with a reusable tile-text layer in `rom/showcase/src/text.asm` that writes zero-terminated ASCII strings by tile X/Y position into the background tilemap buffer.
- The Showcase text path now clears lines deterministically, maps printable ASCII directly onto the local `rom/showcase/assets/font_8x8.asm` tile set, and treats unsupported characters as blank tiles.
- The Showcase validation scene now renders `SUPER Z80 SHOWCASE` and `FONT AND TEXT OK` through the reusable text API, and repeated `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 4` runs remain byte-identical.
- M30 complete.
- Restructured `rom/showcase/` into the canonical ROM project scaffold with modular `src/`, `assets/`, `inc/`, and `build/` directories plus a local `Makefile`.
- Replaced the single-file Showcase validation ROM with a modular entry/init/main-loop layout that boots through the SDK runtime, initializes palette and VDP state, prepares a background tilemap buffer, uploads local font/splash assets through shared VRAM helpers, and enters a deterministic `wait_vblank` / `poll_input` / `update` / `render` loop.
- Moved the Showcase ROM asset contract into `rom/showcase/assets/` with local font, splash tile, and splash tilemap assembly data so future Showcase milestones no longer depend on SDK asset file includes.
- `make -C rom/showcase` now emits `rom/showcase/build/showcase.bin`, and repeated `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 4` runs now produce stable identical `HEADLESS_ROM_RESULT` output for the scaffolded ROM.
- M29j complete.
- Added `rom/showcase/src/main.asm` as a minimal SDK-backed Showcase validation ROM that boots through the repository runtime, initializes palette state, loads the checked-in font and splash tiles, clears a background tilemap buffer, and prints `SUPER Z80 SDK` / `BOOT VALIDATED`.
- Added `rom/showcase/Makefile` so the Showcase validation ROM can be assembled into `build/showcase_validation.bin` through a repository-local target.
- Repeated headless external-ROM execution now validates the first Showcase-oriented ROM path with deterministic `HEADLESS_ROM_RESULT` output from `./build/super_z80 --rom build/showcase_validation.bin --headless --frames 4`.
- M29i complete.
- Replaced the placeholder-only SDK directories with real checked-in include files, runtime startup code, and deterministic assembly assets under `sdk/`.
- Added a minimal reset/runtime path that installs reset and `RST 38h` vectors, initializes stack and baseline hardware state, acknowledges IRQs, and transfers control to ROM-defined `sdk_main`.
- Added deterministic font and splash asset assembly files plus a starter ROM source under `rom/starter/src/main.asm` to prove that ROM code can assemble against the repository SDK surface.
- Developer-facing documentation now points ROM authors at the new SDK/runtime include contract and the external `--rom` headless execution flow.
- M29g complete.
- Added a dedicated SDL-facing `SdlAudioOutput` sink with a bounded FIFO/ring buffer that accepts scheduler-owned mixed `int16_t` samples from `EmulatorCore` and feeds them to SDL through a callback-only consumption path.
- The SDL callback now consumes already-buffered mono `48000 Hz` signed-16 PCM and emits local silence on underrun without advancing emulator state, preserving scheduler ownership of synthesis and sample timing.
- The SDL runtime shell now pumps `EmulatorCore` audio into the SDL sink instead of calling `SDL_QueueAudio` directly from the main loop; when the SDL-facing buffer fills, new host-bound samples are dropped while headless core behavior remains unchanged.
- New SDL sink unit coverage verifies FIFO ordering, silence-on-underflow behavior, the bounded "drop new samples when full" policy, dummy-driver initialization, and core-to-SDL buffer handoff ordering.
- M29f complete.
- Added reusable test-only deterministic audio sequence helpers for programming APU/YM2151 state and collecting exact mixed sample vectors from scheduler- and EmulatorCore-owned stepping.
- Expanded audio validation coverage to prove exact repeatability for silent baseline, APU-only, YM2151-only, and combined PSG+YM paths across the audio mixer, scheduler-owned advancement, direct YM2151 sample generation, and EmulatorCore output consumption.
- New regression coverage now explicitly proves `EmulatorCore` emits scheduler-owned mixed samples rather than an APU-only stream by validating audible YM2151-only output while the APU source remains silent.
- The current internal mixed audio pipeline is now locked down with deterministic exact-equality tests so future SDL/output work can rely on the scheduler-owned path without redesigning synthesis or timing ownership.
- M29e complete.
- Added a deterministic integer `AudioMixer` module that reads the existing PSG-style `APU` sample state plus the YM2151 internal sample and clamps the summed result into a scheduler-owned mixed `int16_t` output.
- The scheduler now owns platform-audio sample composition through explicit audio-sample stepping plus scanline YM2151 advancement, and `EmulatorCore` now consumes scheduler-owned mixed samples instead of pushing raw PSG output directly.
- New unit coverage verifies simple sums, negative mixing, clamp boundaries, and repeated identical mixer inputs producing identical outputs.
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
- SDL dummy-driver validation now checks the runtime mono `48000 Hz` signed-16 little-endian contract plus the SDL sink initialization path without requiring a real audio device.
- SDL runtime shell now supports `--sdl-audio`, opening a mono `48000 Hz` signed-16 little-endian SDL audio device whose callback consumes deterministic PCM buffered ahead of time by the emulator core.
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
M39 PSG sound effects verification is passing with the deterministic flow: `cmake -S . -B build`, `cmake --build build`, `ctest --test-dir build --output-on-failure`, `make -C rom/showcase clean`, `make -C rom/showcase`, and repeated `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 24`. The Showcase ROM now verifies successful repository build/test execution, explicit ROM-local PSG initialization, edge-triggered tone-A sound-effect writes on button `A`, successful ROM assembly, and stable repeated headless execution with no input-triggered audio events.

Showcase milestone reconciliation verification is passing with the required repository-truth checks: `git status --short` and `rg -n "M40|M39|M38a|controller input|PSG Sound Effects|Metasprite" docs artifacts/reports rom/showcase -S`. The controller-driven metasprite work remains documented as `M38a - Controller Interaction Enhancement`, `M39 - PSG Sound Effects` is now complete, and the next official Showcase milestone is `M40 - YM2151 Music Playback`.

M36 basic sprite rendering verification is passing with the deterministic flow: `cmake -S . -B build`, `cmake --build build`, `ctest --test-dir build --output-on-failure`, `make -C rom/showcase clean`, `make -C rom/showcase`, and repeated `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 12`. The Showcase ROM now verifies successful repository build/test execution, explicit VRAM base uploads, stable per-frame SAT updates, visible sprite motion over the parallax scene, successful ROM assembly, and stable repeated headless execution for the sprite demo.

M35 parallax scrolling verification is passing with the deterministic flow: `cmake -S . -B build`, `cmake --build build`, `ctest --test-dir build --output-on-failure`, `make -C rom/showcase clean`, `make -C rom/showcase`, and repeated `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 10`. The Showcase ROM now verifies successful repository build/test execution, two-layer BG/FG scene upload, independent per-frame layer scroll updates, successful ROM assembly, and stable repeated headless execution for the parallax demo.

M32 system splash screen verification is passing with the deterministic flow: `cmake -S . -B build`, `cmake --build build`, `ctest --test-dir build --output-on-failure`, `make -C rom/showcase clean`, `make -C rom/showcase`, and repeated `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 4`. The Showcase ROM now verifies boot-to-splash presentation, correct local splash asset indexing, required reusable-text rendering, successful ROM assembly, and stable repeated headless execution.

M31 font and text rendering verification is passing with the deterministic flow: `cmake -S . -B build`, `cmake --build build`, `ctest --test-dir build --output-on-failure`, `make -C rom/showcase clean`, `make -C rom/showcase`, and repeated `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 4`. The Showcase ROM now verifies the reusable text API, local ASCII font mapping, required on-screen validation strings, successful ROM assembly, and stable repeated headless execution.

M30 Showcase ROM scaffold verification is passing with the deterministic flow: `cmake -S . -B build`, `cmake --build build`, `ctest --test-dir build --output-on-failure`, `make -C rom/showcase`, and repeated `./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 4`. The new scaffold now verifies the canonical Showcase project layout, local asset ownership, successful ROM assembly into `rom/showcase/build/showcase.bin`, deterministic SDK-runtime boot, and stable repeated headless execution of the scaffolded main loop.

M29j SDK integration validation is passing with the deterministic flow: `cmake -S . -B build`, `cmake --build build`, `ctest --test-dir build --output-on-failure`, `sjasmplus --nologo -I . --raw=build/showcase_validation.bin rom/showcase/src/main.asm`, and repeated `./build/super_z80 --rom build/showcase_validation.bin --headless --frames 4`. The new Showcase validation ROM now proves that the checked-in SDK runtime, font assets, and splash assets are sufficient for a minimal ROM to boot, initialize VRAM state through the current sequential VDP contract, and produce stable repeated headless execution summaries.

M29i SDK runtime surface verification is passing with the deterministic flow: `cmake -S . -B build`, `cmake --build build`, `ctest --test-dir build --output-on-failure`, `sjasmplus --nologo -I . --raw=build/test_sdk_rom.bin rom/starter/src/main.asm`, and repeated `./build/super_z80 --rom build/test_sdk_rom.bin --headless --frames 2`. The new SDK-backed ROM assembly path now verifies include resolution, startup/runtime linkage, asset inclusion, successful ROM execution, and deterministic repeated headless output.

M29g SDL audio output integration is passing with the deterministic build/test flow: `cmake -S . -B build`, `cmake --build build --target super_z80_test_sdl_audio_output super_z80_test_audio_output_integration super_z80_test_audio_determinism super_z80_test_scheduler`, `ctest --test-dir build --output-on-failure --tests-regex "super_z80_test_(sdl_audio_output|audio_output_integration|audio_determinism|scheduler)"`, `cmake --build build`, and `ctest --test-dir build --output-on-failure`. The focused coverage now verifies SDL sink FIFO order, silence-on-underflow, bounded-buffer drop policy, dummy-driver initialization, preserved core-to-sink sample ordering, and unchanged deterministic mixed-output behavior.

M29f deterministic audio validation is passing with the deterministic build/test flow: `cmake -S . -B build`, `cmake --build build --target super_z80_test_audio_mixer super_z80_test_scheduler super_z80_test_audio_output_integration super_z80_test_audio_determinism super_z80_test_ym2151`, `ctest --test-dir build --output-on-failure --tests-regex "super_z80_test_(audio_mixer|scheduler|audio_output_integration|audio_determinism|ym2151)"`, `cmake --build build`, and `ctest --test-dir build --output-on-failure`. The expanded coverage now verifies exact mixed-sample repeatability for silent baseline, APU-only, YM2151-only, combined output, fixed scripted scheduler stepping, direct YM2151 sample sequences, and EmulatorCore mixed-output consumption.

M29e audio mixer verification is passing with the deterministic build/test flow: `cmake -S . -B build`, `cmake --build build`, `ctest --test-dir build --output-on-failure --tests-regex "super_z80_test_(audio_mixer|scheduler|audio_output_integration|audio_determinism|ym2151)"`, and `ctest --test-dir build --output-on-failure`. The new `super_z80_test_audio_mixer` coverage verifies scheduler-facing PSG/YM additive mixing, clamp behavior, and deterministic repeatability, while the focused regression run confirms the mixed output path preserves scheduler, YM2151, and core audio behavior.

M29d YM2151 verification is passing with the deterministic build/test flow: `cmake -S . -B build`, `cmake --build build`, `ctest --test-dir build --output-on-failure --tests-regex "super_z80_test_(ym2151|scheduler|bus)"`, and `ctest --test-dir build --output-on-failure`. The expanded `super_z80_test_ym2151` coverage verifies reset sample state, stored phase increments, deterministic FM sample generation, sample repeatability, release-to-silence behavior, timer/status/IRQ behavior, bus-visible pending state, and the scheduler-owned fixed YM2151 cycle budget.

M29c YM2151 verification is passing with the deterministic build/test flow: `cmake -S . -B build`, `cmake --build build`, `ctest --test-dir build --output-on-failure --tests-regex "super_z80_test_(ym2151|scheduler|bus)"`, and `ctest --test-dir build --output-on-failure`. The expanded `super_z80_test_ym2151` coverage verifies timer latch storage, disabled/enabled deterministic progression, Timer A and Timer B overflow/status bits, IRQ-pending gating, control-path clears, repeatability, bus-visible pending state, and the scheduler-owned fixed YM2151 cycle budget.

M29b YM2151 verification is passing with the deterministic build/test flow: `cmake -S . -B build`, `cmake --build build`, `ctest --test-dir build --output-on-failure --tests-regex "super_z80_test_(ym2151|scheduler|bus)"`, and `ctest --test-dir build --output-on-failure`. The expanded `super_z80_test_ym2151` coverage verifies channel/operator state decode, key-on writes, frequency/block updates, operator parameter storage, CPU-visible `OUT` behavior on ports `0x70-0x71`, and deterministic placeholder phase progression.

M29a YM2151 verification is passing with the full deterministic build/test flow: `cmake -S . -B build`, `cmake --build build`, and `ctest --test-dir build --output-on-failure`. The new `super_z80_test_ym2151` coverage verifies register selection, register writes, CPU-visible `OUT` behavior on ports `0x70-0x71`, and scheduler invocation of the placeholder timing hook.

M28 documentation verification passing: `test -f docs/developer_guide.md`, `test -f docs/hardware_reference.md`, `test -f docs/programming_graphics.md`, `test -f docs/programming_audio.md`, `test -f docs/programming_input.md`, and `test -f docs/platform_timing.md`.

Most recent implementation verification remains the passing M27 run: `cmake -S . -B build`, `cmake --build build`, and `ctest --test-dir build --output-on-failure`. The full suite included `super_z80_test_platform_determinism`, `super_z80_test_cpu_dma_irq_integration`, `super_z80_test_vdp_vblank_irq`, and `super_z80_test_input_audio_integration`, all passing in the shared deterministic headless build.

## Next Step
Choose the next explicit task packet. The next planned follow-up is `M36 - Basic Sprite Rendering` to layer deterministic sprite uploads and motion on top of the newly validated parallax Showcase scene.
