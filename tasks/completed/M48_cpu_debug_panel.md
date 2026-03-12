# Task Packet

## Title

M48 — CPU Debug Panel

## Goal

Implement the first **hardware inspection panel** in the Super_Z80 frontend: a CPU Debug Panel that displays the current Z80 CPU register state during emulator execution.

This panel establishes the basic pattern for all future hardware debug panels.

The panel must be **read-only** in this milestone and must not modify CPU state.

The deterministic emulator core must remain unchanged.

## Repository Context

Recent milestones:

M45 — ImGui Integration  
M46 — Emulator Control Panel  
M47 — ROM Loader Interface

The emulator now supports:

• SDL frontend window  
• Dear ImGui debug interface  
• Emulator run/pause/reset/step controls  
• Interactive ROM loading  

The next step is exposing **hardware state to developers**, starting with the CPU.

## Required Read Order

Before making any changes, read in this order:

1. `docs/plan.md`
2. `docs/state_snapshot.md`
3. `docs/architecture.md`
4. `docs/decisions.md`
5. `AGENTS.md`
6. `artifacts/reports/M47_rom_loader_interface_report.md`

Then inspect the current frontend implementation:

- `emulator/frontend/frontend.h`
- `emulator/frontend/frontend.cpp`
- `emulator/frontend/debug_panels/debug_panel_host.h`
- `emulator/frontend/debug_panels/debug_panel_host.cpp`

And the CPU implementation:

- `emulator/include/emulator_core.h`
- `emulator/src/emulator_core.cpp`

If necessary, inspect the CPU class or register structure used internally by the Z80 implementation.

## Scope

Included:

• CPU debug window  
• Z80 register display  
• flag display  
• program counter visibility  
• stack pointer visibility  
• refresh every frame  

Excluded:

• memory editing
• breakpoints
• disassembly
• instruction tracing
• stepping individual instructions
• modifying registers
• advanced debugger features

This milestone is **display-only**.

## Milestone Definition

The CPU Debug Panel must show the following Z80 registers:

### Primary Registers

A  
F  

B  
C  

D  
E  

H  
L  

### Register Pairs

AF  
BC  
DE  
HL  

### Special Registers

PC (Program Counter)  
SP (Stack Pointer)  
IX  
IY  

### Additional CPU State (if available)

I register  
R register  

### Flags

Display Z80 flags clearly:

S (Sign)  
Z (Zero)  
H (Half Carry)  
P/V (Parity/Overflow)  
N (Subtract)  
C (Carry)

Flags should be visually readable (example: ON/OFF or highlighted).

## Panel Layout

Create a window titled:

```

CPU Debug

```

Example layout:

```

CPU Debug

Registers
A:  3F     F:  92
B:  10     C:  00
D:  22     E:  18
H:  40     L:  12

AF: 3F92
BC: 1000
DE: 2218
HL: 4012

PC: 8000
SP: FF20
IX: 0000
IY: 0000

Flags
S Z H P N C
0 1 0 1 0 1

```

Exact formatting may vary but must remain clear and compact.

## Implementation Requirements

### 1. Create CPU Debug Panel

Extend the existing debug panel host system.

Do NOT bypass the panel host architecture.

The CPU panel must be rendered through the existing frontend debug panel infrastructure.

### 2. Access CPU State

Retrieve CPU register state from the emulator runtime.

The frontend must not directly manipulate emulator internals.

Use existing public accessors if available.

If minimal getters are required, they may be added **only as read-only accessors**.

Do not expose mutable CPU references.

### 3. Update Frequency

The CPU panel must update every rendered frame.

It must reflect the **current CPU state after the last executed frame**.

### 4. Determinism

The CPU panel must not:

• alter timing  
• trigger emulator execution  
• introduce side effects  

The panel must be **purely observational**.

### 5. UI Integration

The panel should appear automatically alongside the existing frontend window.

No menu toggling is required in this milestone.

Future milestones may add panel visibility toggles.

## Files Expected to Change

Likely:

- `emulator/frontend/debug_panels/debug_panel_host.cpp`
- `emulator/frontend/debug_panels/debug_panel_host.h`
- `emulator/frontend/frontend.cpp`
- `emulator/frontend/frontend.h`

Possibly:

- `emulator/include/emulator_core.h`
- `emulator/src/emulator_core.cpp`

Only if small read-only accessors are required.

## Files to Create

Create milestone report:

```

artifacts/reports/M48_cpu_debug_panel_report.md

```

## Constraints

Do not:

• refactor the emulator core  
• add debugger logic  
• implement breakpoints  
• implement instruction stepping  
• add third-party dependencies  
• modify deterministic scheduling  
• modify ROM loading  

This milestone must remain small and focused.

## Verification Commands

Run:

```

cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure

```

Then run frontend smoke tests:

```

./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 1

```
```

env SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software timeout 2s ./build/super_z80 --sdl-input --rom rom/showcase/build/showcase.bin

```
```

env SDL_VIDEODRIVER=dummy SDL_RENDER_DRIVER=software SDL_AUDIODRIVER=dummy timeout 2s ./build/super_z80 --sdl-audio --rom rom/showcase/build/showcase.bin

```

Confirm:

• build succeeds  
• tests pass  
• frontend launches without crash  

Automated tests cannot verify the ImGui panel visually; document this in the report.

## Acceptance Criteria

- Emulator builds successfully
- Tests pass
- CPU Debug window appears
- Z80 registers are displayed
- Flags are displayed
- PC and SP visible
- Panel updates during execution
- Headless mode unchanged
- `docs/state_snapshot.md` updated
- `M48_cpu_debug_panel_report.md` created

## Suggested Branch

```

m48_cpu_debug_panel

```

## Suggested Commit Message

```

M48: add cpu debug panel

```

## Completion Report Format

Use:

Summary  
Files Changed  
Files Created  
Commands Run  
Verification Results  
Issues Encountered  
Recommended Next Step

## Recommended Next Step

M49 — Memory Viewer Panel
```
