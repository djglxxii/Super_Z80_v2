# Super_Z80_v2 State Snapshot

## Current Milestone
M4

## Recent Changes
- CRC32 utility module added.
- First unit test executable added for deterministic known-answer checks.
- CMake test wiring added for CRC32 validation.
- Deterministic 64KB memory subsystem added with reset, read, and write primitives.
- Memory unit tests added and passing.
- Bus layer added with deterministic ROM, open-bus, and RAM routing behavior.
- Initial CPU-visible memory map implemented: ROM at `0x0000-0x7FFF`, open bus at `0x8000-0xBFFF`, RAM at `0xC000-0xFFFF`.
- Bus unit tests added and passing.

## Known Issues
None yet.

## Verification Status
M4 build and test verification passing.

## Next Step
M5 - CPU Wrapper and Z80ex Integration
