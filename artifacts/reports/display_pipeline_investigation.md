# Display Pipeline Investigation Report

## Problem

After loading a ROM (e.g., `rom/showcase/build/showcase.bin`), the emulator shows no visual output. Debug panels confirm the ROM loads successfully and the CPU is executing, but the window displays only a dark background with ImGui debug overlays.

## Architecture Overview

The emulator is a desktop C++ application using SDL2 for windowing/rendering and ImGui for debug UI. The display pipeline involves:

- **VDP** (`emulator/include/vdp.h`, `emulator/src/vdp.cpp`) — Video Display Processor emulating a tile-based graphics chip with two planes (BG/FG), 64 sprites, 256-color palette, and a 256x192 indexed framebuffer.
- **Scheduler** (`emulator/src/scheduler.cpp`) — Drives the emulation loop per-scanline: steps the CPU, DMA, YM2151, audio mixer, and VDP timing.
- **EmulatorCore** (`emulator/src/emulator_core.cpp`) — Wraps the bus, CPU, and scheduler; provides snapshot and audio APIs.
- **Frontend** (`emulator/frontend/frontend.cpp`) — Manages the SDL2 window, ImGui context, and debug panel rendering.
- **Debug Panel Host** (`emulator/frontend/debug_panels/debug_panel_host.cpp`) — Renders ImGui debug panels (CPU state, memory viewer, VRAM hex dump, sprite table, audio, timing, etc.).

## Components Verified Working

| Component | Status | Evidence |
|---|---|---|
| ROM loading | Working | `bus_.load_rom()` copies ROM into 0x0000–0x7FFF; headless mode produces correct CRC outputs |
| CPU execution | Working | Scheduler steps 512 instructions per scanline; PC advances through ROM code |
| VRAM access | Working | ROM writes tile data, tilemaps, and sprite attributes to VRAM via VDP ports 0xE1/0xE2 |
| Palette writes | Working | ROM programs palette entries via ports 0xE3–0xE7 |
| VBLANK timing | Working | `VDP::step_scanline()` sets `vblank_active_` at scanline 192 and `frame_ready_` at scanline 191; ROM's `showcase_wait_vblank()` polls status port 0xE0 correctly |
| DMA | Working | DMA steps each scanline in the scheduler |
| Audio | Working | APU and YM2151 tick each scanline; audio samples are produced and buffered |
| Display scaling | Working | Infrastructure exists (2x, 4x, 6x presets) with window resizing via `SDL_SetWindowSize` |

## Root Cause

Two critical pieces of the display pipeline are missing:

### 1. `VDP::render_frame()` is never called during emulation

The VDP has a complete `render_frame()` method (`vdp.cpp:197–207`) that:

1. Clears the framebuffer and sprite mask
2. Renders the background plane
3. Renders low-priority sprites
4. Renders the foreground plane
5. Renders high-priority sprites

This method is called extensively in unit tests (47+ invocations across 18 test files) but **never in the main emulation path**. The scheduler's `step_scanline()` only calls `vdp_.step_scanline(scanline_)`, which updates timing flags (`vblank_active_`, `frame_ready_`) but does not render any pixels.

**Affected file:** `emulator/src/scheduler.cpp`

```cpp
// Current code at the frame boundary (lines 34–37):
if (scanline_ == kScanlinesPerFrame) {
    scanline_ = 0U;
    ++frame_;
}
// render_frame() is never called anywhere in this path
```

The natural place to invoke rendering is at the frame boundary, just before the scanline counter resets.

### 2. No frontend display panel renders the framebuffer

The frontend only renders ImGui debug panels via `debug_panel_host_.render()`. There is no code anywhere in the frontend that:

- Reads the VDP framebuffer pixels (`VDP::framebuffer_pixel()` or `VDP::framebuffer_color()`)
- Converts palette-indexed pixels to RGBA using `VDP::resolve_pixel()` or `VDP::palette_entry()`
- Creates or updates an SDL texture with the rendered pixel data
- Draws that texture to the SDL renderer

The main loop in both `run_sdl_audio_shell` and `run_sdl_input_shell` follows this pattern:

```cpp
SDL_SetRenderDrawColor(renderer, 12, 16, 20, 255);  // dark background
SDL_RenderClear(renderer);
frontend.render();        // only renders ImGui debug panels
SDL_RenderPresent(renderer);
```

No game display is ever drawn between `SDL_RenderClear` and `SDL_RenderPresent`.

## VDP Framebuffer API

The VDP already exposes everything needed to read the rendered output:

| Method | Purpose |
|---|---|
| `render_frame()` | Rasterizes all planes and sprites into the internal framebuffer |
| `framebuffer_pixel(x, y)` | Returns the palette index at pixel (x, y) |
| `framebuffer_color(pixel_index)` | Returns the RGBA color for a linear framebuffer index |
| `palette_entry(index)` | Returns the RGBA value for a palette index |
| `resolve_pixel(palette_index)` | Converts a palette index to its RGBA color |
| `frame_ready()` | Returns true when scanline 191 has been reached (frame complete) |

Palette RGBA format is `0xRRGGBBAA` (red in bits 31–24, green in bits 23–16, blue in bits 15–8, alpha in bits 7–0).

## What Needs to Be Built

### Fix 1: Invoke `render_frame()` at the frame boundary

In `Scheduler::step_scanline()`, call `vdp_.render_frame()` when the frame completes. This could be done either:

- **At the frame boundary** (when `scanline_ == kScanlinesPerFrame`), rendering the entire frame at once — simplest approach, matches current `render_frame()` design.
- **Per-scanline** — would require refactoring `render_frame()` into incremental scanline rendering, which is more accurate to real hardware but significantly more work.

### Fix 2: Create a framebuffer display panel/texture

The frontend needs a component that:

1. Creates an `SDL_Texture` of size 256x192 with `SDL_TEXTUREACCESS_STREAMING` and `SDL_PIXELFORMAT_RGBA8888`
2. Each frame, locks the texture and copies the VDP framebuffer (converting palette indices to RGBA via `VDP::framebuffer_color()` or `VDP::resolve_pixel()`)
3. Renders the texture to the SDL renderer, scaled to fill the window (or a designated region), behind the ImGui debug overlay

This texture should be drawn between `SDL_RenderClear` and `frontend.render()` so the game display appears behind the debug panels.

## File References

| File | Relevant Lines | Purpose |
|---|---|---|
| `emulator/src/scheduler.cpp` | 24–41 | `step_scanline()` — frame boundary at line 34 |
| `emulator/src/emulator_core.cpp` | 36–39 | `step_scanline()` wrapper |
| `emulator/src/vdp.cpp` | 197–207 | `render_frame()` implementation |
| `emulator/src/vdp.cpp` | 323–333 | `step_scanline()` — timing flags only |
| `emulator/include/vdp.h` | 86–98 | Framebuffer access API |
| `emulator/frontend/frontend.cpp` | 230–244 | `render()` — only renders debug panels |
| `emulator/src/main.cpp` | 744–764 | SDL audio shell main loop |
| `emulator/src/main.cpp` | 906–973 | SDL input shell main loop |
