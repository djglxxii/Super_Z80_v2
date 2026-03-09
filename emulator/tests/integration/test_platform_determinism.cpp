#include "emulator_core.h"
#include "io.h"
#include "vdp.h"

#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

bool expect_true(const char* name, bool value) {
    if (!value) {
        std::cerr << "[FAIL] " << name << " expected=true actual=false" << std::endl;
        return false;
    }

    std::cout << "[PASS] " << name << " value=true" << std::endl;
    return true;
}

std::array<uint8_t, 0x48U> make_rom() {
    std::array<uint8_t, 0x48U> rom{};

    rom[0x0000U] = 0x31U;
    rom[0x0001U] = 0xFEU;
    rom[0x0002U] = 0xD0U;
    rom[0x0003U] = 0x3EU;
    rom[0x0004U] = 0x01U;
    rom[0x0005U] = 0xD3U;
    rom[0x0006U] = 0xF1U;
    rom[0x0007U] = 0xFBU;
    rom[0x0008U] = 0xDBU;
    rom[0x0009U] = 0xDCU;
    rom[0x000AU] = 0x32U;
    rom[0x000BU] = 0x00U;
    rom[0x000CU] = 0xC3U;
    rom[0x000DU] = 0xDBU;
    rom[0x000EU] = 0xDDU;
    rom[0x000FU] = 0x32U;
    rom[0x0010U] = 0x01U;
    rom[0x0011U] = 0xC3U;
    rom[0x0012U] = 0x76U;
    rom[0x0013U] = 0x3AU;
    rom[0x0014U] = 0x02U;
    rom[0x0015U] = 0xC3U;
    rom[0x0016U] = 0x3CU;
    rom[0x0017U] = 0x32U;
    rom[0x0018U] = 0x02U;
    rom[0x0019U] = 0xC3U;
    rom[0x001AU] = 0xC3U;
    rom[0x001BU] = 0x08U;
    rom[0x001CU] = 0x00U;

    rom[0x0038U] = 0xDBU;
    rom[0x0039U] = 0xF0U;
    rom[0x003AU] = 0x32U;
    rom[0x003BU] = 0x03U;
    rom[0x003CU] = 0xC3U;
    rom[0x003DU] = 0x3EU;
    rom[0x003EU] = 0x01U;
    rom[0x003FU] = 0xD3U;
    rom[0x0040U] = 0xF2U;
    rom[0x0041U] = 0xEDU;
    rom[0x0042U] = 0x4DU;

    return rom;
}

void seed_video(superz80::VDP& vdp) {
    vdp.write_vram(superz80::VDP::kBgTilemapBase + 0U, 0x01U);
    for (std::size_t index = 0U; index < superz80::VDP::kTileSizeBytes; ++index) {
        vdp.write_vram(static_cast<uint16_t>(superz80::VDP::kPatternBank0Base +
                                             superz80::VDP::kTileSizeBytes + index),
                       static_cast<uint8_t>((index % 8U) + 1U));
    }
    for (uint8_t index = 1U; index <= 8U; ++index) {
        vdp.set_palette_entry(index, static_cast<uint32_t>(index) << 24U);
    }
}

void program_audio(EmulatorCore& core) {
    core.bus().write_port(superz80::Bus::kAudioToneALowPort, 0x03U);
    core.bus().write_port(superz80::Bus::kAudioToneAHighPort, 0x00U);
    core.bus().write_port(superz80::Bus::kAudioVolumeAPort, 0x01U);
    core.bus().write_port(superz80::Bus::kAudioNoiseControlPort, 0x00U);
    core.bus().write_port(superz80::Bus::kAudioVolumeNoisePort, 0x04U);
    core.bus().write_port(superz80::Bus::kAudioControlPort, 0x01U);
}

void set_controller_state(EmulatorCore& core) {
    core.bus().set_controller_button(superz80::IO::Button::Left, true);
    core.bus().set_controller_button(superz80::IO::Button::A, true);
    core.bus().set_controller_button(superz80::IO::Button::Start, true);
}

struct Snapshot {
    std::array<uint8_t, 8U> ram_window;
    std::array<uint8_t, 3U> dma_window;
    std::array<uint8_t, 8U> framebuffer;
    std::vector<EmulatorCore::AudioSample> audio;
    uint8_t irq_status;
    uint8_t irq_enable;
    uint8_t dma_length;
    uint8_t dma_control;
    uint8_t pad1;
    uint8_t pad1_sys;
    uint8_t apu_control;
    uint16_t tone_a_period;
    uint16_t noise_lfsr;
    bool apu_overrun;

    bool operator==(const Snapshot& other) const {
        return ram_window == other.ram_window &&
               dma_window == other.dma_window &&
               framebuffer == other.framebuffer &&
               audio == other.audio &&
               irq_status == other.irq_status &&
               irq_enable == other.irq_enable &&
               dma_length == other.dma_length &&
               dma_control == other.dma_control &&
               pad1 == other.pad1 &&
               pad1_sys == other.pad1_sys &&
               apu_control == other.apu_control &&
               tone_a_period == other.tone_a_period &&
               noise_lfsr == other.noise_lfsr &&
               apu_overrun == other.apu_overrun;
    }
};

Snapshot run_platform_sequence() {
    EmulatorCore core;
    const auto rom = make_rom();

    core.initialize();
    core.bus().load_rom(rom.data(), rom.size());
    seed_video(core.bus().vdp());
    program_audio(core);
    set_controller_state(core);

    core.bus().write(0xC100U, 0x44U);
    core.bus().write(0xC101U, 0x55U);
    core.bus().write(0xC102U, 0x66U);
    core.bus().write_port(superz80::Bus::kDmaSrcLowPort, 0x00U);
    core.bus().write_port(superz80::Bus::kDmaSrcHighPort, 0xC1U);
    core.bus().write_port(superz80::Bus::kDmaDstLowPort, 0x20U);
    core.bus().write_port(superz80::Bus::kDmaDstHighPort, 0xC1U);
    core.bus().write_port(superz80::Bus::kDmaLengthPort, 0x03U);
    core.bus().write_port(superz80::Bus::kDmaControlPort, superz80::DMA::kControlStartBit);

    for (std::size_t scanline = 0U; scanline < 256U; ++scanline) {
        core.step_scanline();
    }

    core.bus().vdp().render_frame();

    Snapshot snapshot{};
    for (std::size_t index = 0U; index < snapshot.ram_window.size(); ++index) {
        snapshot.ram_window[index] = core.bus().read(static_cast<uint16_t>(0xC300U + index));
    }
    for (std::size_t index = 0U; index < snapshot.dma_window.size(); ++index) {
        snapshot.dma_window[index] = core.bus().read(static_cast<uint16_t>(0xC120U + index));
    }
    for (std::size_t index = 0U; index < snapshot.framebuffer.size(); ++index) {
        snapshot.framebuffer[index] = core.bus().vdp().framebuffer_pixel(index, 0U);
    }

    snapshot.audio.resize(core.available_audio_samples(), 0);
    const std::size_t copied = core.consume_audio_samples(snapshot.audio.data(), snapshot.audio.size());
    snapshot.audio.resize(copied);
    snapshot.irq_status = core.bus().read_port(superz80::Bus::kIrqStatusPort);
    snapshot.irq_enable = core.bus().read_port(superz80::Bus::kIrqEnablePort);
    snapshot.dma_length = core.bus().read_port(superz80::Bus::kDmaLengthPort);
    snapshot.dma_control = core.bus().read_port(superz80::Bus::kDmaControlPort);
    snapshot.pad1 = core.bus().read_port(superz80::Bus::kPad1Port);
    snapshot.pad1_sys = core.bus().read_port(superz80::Bus::kPad1SysPort);
    snapshot.apu_control = core.bus().apu().control();
    snapshot.tone_a_period = core.bus().apu().tone_period(0U);
    snapshot.noise_lfsr = core.bus().apu().noise_lfsr();
    snapshot.apu_overrun = core.bus().apu().overrun();
    return snapshot;
}

} // namespace

int main() {
    bool ok = true;

    const Snapshot first = run_platform_sequence();
    const Snapshot second = run_platform_sequence();

    ok = expect_true("full-platform-snapshots-match-across-identical-runs", first == second) && ok;
    ok = expect_true("cpu-recorded-controller-state", first.ram_window[0] == 0xEBU && first.ram_window[1] == 0xFEU) && ok;
    ok = expect_true("dma-destination-window-copied-expected-bytes",
                     first.dma_window[0] == 0x44U && first.dma_window[1] == 0x55U &&
                         first.dma_window[2] == 0x66U) && ok;
    ok = expect_true("irq-handler-ran-during-sequence", first.ram_window[2] != 0x00U && first.ram_window[3] == 0x01U) && ok;
    ok = expect_true("video-framebuffer-contains-seeded-pattern", first.framebuffer[0] == 0x01U && first.framebuffer[7] == 0x08U) && ok;
    ok = expect_true("audio-buffer-is-non-empty-and-deterministic", !first.audio.empty()) && ok;
    ok = expect_true("dma-is-finished-at-capture-time", first.dma_length == 0x00U && first.dma_control == 0x00U) && ok;
    ok = expect_true("irq-enable-latch-remains-programmed", first.irq_enable == 0x01U && first.irq_status == 0x00U) && ok;
    ok = expect_true("apu-state-remains-active-and-non-overrun",
                     first.apu_control == 0x01U && first.tone_a_period == 0x0003U && !first.apu_overrun) && ok;

    return ok ? 0 : 1;
}
