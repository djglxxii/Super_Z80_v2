# Super_Z80_v2 Architecture Baseline

Super_Z80_v2 targets a modular emulator architecture where subsystem boundaries remain explicit and testable.

Core emulation logic will be separated from frontend or shell concerns so headless validation can remain stable across environments.

A bus-centric design is preferred for subsystem communication to keep component interactions inspectable and deterministic.

Deterministic headless verification is a first-class requirement and must be preserved as the implementation grows.

Initial subsystem candidates include memory, CPU, VDP, DMA, audio, input, trace, and an emulator coordinator.

## Library Strategy
The emulator will integrate established third-party components when they reduce risk or avoid reimplementing mature solutions.

CPU emulation is expected to rely on Z80ex.

The platform shell layer will likely use SDL2 for windowing, input, and audio.

The emulator core remains modular and independent of UI layers.

Dependency use must remain minimal and purposeful, with each dependency decision recorded in the decision log.

This baseline captures current intent only and will be refined through implementation milestones.
