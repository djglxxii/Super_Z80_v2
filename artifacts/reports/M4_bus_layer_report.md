# Task Completion Report

## Title
M4 - Bus Layer and Initial CPU-Visible Memory Map

## Status
Completed

## Summary
Implemented a deterministic `Bus` module that routes CPU-visible reads and writes across a fixed ROM window, an unmapped open-bus region, and a RAM window backed by the existing memory subsystem. Added standalone unit coverage for ROM loading, ROM write protection, RAM routing, unmapped behavior, and reset semantics. Updated the state snapshot to reflect milestone M4 completion and the next CPU-wrapper milestone.

## Files Created
- emulator/include/bus.h
- emulator/src/bus.cpp
- emulator/tests/unit/test_bus.cpp
- artifacts/reports/M4_bus_layer_report.md

## Files Updated
- emulator/CMakeLists.txt
- docs/state_snapshot.md

## Bus API
- `superz80::Bus::reset()` clears RAM state while preserving loaded ROM data.
- `superz80::Bus::load_rom(const void* data, std::size_t size)` copies up to `0x8000` ROM bytes and fills the rest with `0xFF`.
- `superz80::Bus::read(uint16_t address) const` returns ROM bytes, routed RAM bytes, or open-bus `0xFF` for unmapped space.
- `superz80::Bus::write(uint16_t address, uint8_t value)` writes only to the RAM window and ignores ROM or unmapped writes.

## Memory Map Implemented
- `0x0000-0x7FFF`: ROM
- `0x8000-0xBFFF`: unmapped/open bus, reads as `0xFF`, writes ignored
- `0xC000-0xFFFF`: RAM, translated to memory offsets `0x0000-0x3FFF`

## Test Cases Covered
- ROM reads return loaded bytes.
- ROM bytes beyond loaded input default to `0xFF`.
- ROM writes are ignored.
- RAM reads and writes persist through the bus.
- Distinct RAM addresses remain independent.
- Unmapped reads return `0xFF`.
- Unmapped writes do not affect ROM or RAM state.
- `reset()` clears RAM and preserves ROM contents.

## Build Output Summary
- `cmake -S . -B build` configured successfully.
- `cmake --build build` built the emulator, CRC32 test target, memory test target, and bus test target successfully.

## Test Execution Results
- `ctest --test-dir build --output-on-failure` passed (`3/3` tests passed).
- `./build/emulator/super_z80_test_bus` passed all bus routing and reset checks directly.

## Commit Hash
- Recorded after commit creation in the execution handoff because the report file itself is part of the milestone commit.

## Decisions or Notes
- Kept the existing `Memory` module unchanged and moved all address decoding into the bus to preserve a narrow deterministic boundary.
- Future CPU integration can now route all CPU-visible memory access through the bus instead of coupling directly to flat memory storage.

## Recommended Next Steps
- Implement `M5 - CPU Wrapper and Z80ex Integration` on top of this bus boundary.
