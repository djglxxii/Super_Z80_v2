# M41 – Audio Mixing Demonstration

Agent Task Packet

## Goal

Demonstrate simultaneous playback of YM2151 music and PSG sound effects in the Showcase ROM.

The existing YM music loop from **M40** must continue playing while PSG sound effects from **M39** are triggered by input.

---

# Scope

Modify only the Showcase ROM.

Do **not** modify emulator audio code unless a bug is discovered.

The objective is to prove that:

```
YM2151 music + PSG sound effect
play at the same time
without interrupting each other
```

---

# Required Behavior

The ROM must demonstrate the following:

### Background Music

* YM2151 music loop continues indefinitely (already implemented in M40).

### Sound Effect Trigger

* Pressing **PAD1 A** triggers the PSG tone burst (from M39).

### Mixing

When the sound effect triggers:

```
YM music continues playing
PSG sound plays on top of music
```

Neither audio source should reset or interrupt the other.

---

# Files Expected to Change

```
rom/showcase/src/main_loop.asm
rom/showcase/inc/constants.inc
rom/showcase/inc/memory_map.inc
```

Documentation updates:

```
docs/showcase_state_snapshot.md
docs/state_snapshot.md
artifacts/reports/m41_audio_mixing_demo.md
```

---

# Implementation Guidance

The ROM should simply allow both systems to operate simultaneously.

Typical structure:

```
frame update loop

    update music timing
    write YM registers when needed

    read controller

    if A pressed:
        trigger PSG sound

    update graphics
```

Important rule:

```
PSG writes must not alter YM registers
YM writes must not silence PSG
```

Each subsystem must maintain independent state.

---

# Verification Commands

Run standard deterministic verification.

```
cmake -S . -B build
cmake --build build
ctest --test-dir build --output-on-failure
```

Rebuild Showcase ROM:

```
make -C rom/showcase clean
make -C rom/showcase
```

Run deterministic ROM execution:

```
./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 32
./build/super_z80 --rom rom/showcase/build/showcase.bin --headless --frames 32
```

CRC outputs must match.

---

# Deliverables

The agent must produce:

```
artifacts/reports/m41_audio_mixing_demo.md
```

The report must include:

* milestone preflight block
* summary of mixing behavior
* files changed
* commands run
* verification results
* deterministic CRC output

---

# Expected Result

The Showcase ROM now demonstrates:

```
graphics
controller input
sprite animation
parallax scrolling
PSG sound effects
YM2151 music
simultaneous audio mixing
```

This milestone verifies that the **complete Super_Z80 audio subsystem works under real ROM conditions**.

---

# Next Milestone

After completion:

```
M42 – Integrated Showcase Scene
```

This milestone merges all subsystems into a final interactive demo scene.
