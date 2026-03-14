#include "emulator_core.h"
#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <vector>

namespace {

bool expect_equal_u8(const char* name, uint8_t actual, uint8_t expected) {
    if (actual != expected) {
        std::cerr << "[FAIL] " << name << " expected=0x" << std::hex << std::uppercase
                  << static_cast<unsigned int>(expected) << " actual=0x"
                  << static_cast<unsigned int>(actual) << std::dec << std::nouppercase
                  << std::endl;
        return false;
    }

    std::cout << "[PASS] " << name << " value=0x" << std::hex << std::uppercase
              << static_cast<unsigned int>(actual) << std::dec << std::nouppercase
              << std::endl;
    return true;
}

bool expect_equal_u16(const char* name, uint16_t actual, uint16_t expected) {
    if (actual != expected) {
        std::cerr << "[FAIL] " << name << " expected=0x" << std::hex << std::uppercase
                  << expected << " actual=0x" << actual << std::dec << std::nouppercase
                  << std::endl;
        return false;
    }

    std::cout << "[PASS] " << name << " value=0x" << std::hex << std::uppercase << actual
              << std::dec << std::nouppercase << std::endl;
    return true;
}

bool expect_equal_u32(const char* name, uint32_t actual, uint32_t expected) {
    if (actual != expected) {
        std::cerr << "[FAIL] " << name << " expected=0x" << std::hex << std::uppercase
                  << expected << " actual=0x" << actual << std::dec << std::nouppercase
                  << std::endl;
        return false;
    }

    std::cout << "[PASS] " << name << " value=0x" << std::hex << std::uppercase << actual
              << std::dec << std::nouppercase << std::endl;
    return true;
}

std::vector<EmulatorCore::AudioSample> collect_audio_samples(EmulatorCore& core) {
    std::vector<EmulatorCore::AudioSample> samples(core.available_audio_samples(), 0);
    if (!samples.empty()) {
        core.consume_audio_samples(samples.data(), samples.size());
    }
    return samples;
}

template <typename T>
uint32_t checksum_bytes(const T* data, std::size_t count) {
    const auto* bytes = reinterpret_cast<const uint8_t*>(data);
    const std::size_t byte_count = sizeof(T) * count;
    uint32_t checksum = 2166136261U;
    for (std::size_t index = 0U; index < byte_count; ++index) {
        checksum ^= static_cast<uint32_t>(bytes[index]);
        checksum *= 16777619U;
    }
    return checksum;
}

} // namespace

int main() {
    bool ok = true;

    EmulatorCore core;
    core.initialize();

    constexpr std::array<uint8_t, 4> kRom = {0x3EU, 0x01U, 0xD3U, 0xE0U};
    core.load_rom(kRom.data(), kRom.size());
    core.bus().write(0xC000U, 0x12U);
    core.bus().write(0xC001U, 0x34U);
    core.bus().write(0xFFFFU, 0xABU);
    core.bus().vdp().write_vram(0x0000U, 0x56U);
    core.bus().vdp().write_vram(0x1234U, 0x78U);
    core.bus().vdp().write_vram(0xFFFFU, 0x9AU);
    core.bus().vdp().write_vram(superz80::VDP::kSpriteAttributeTableBase, 0x10U);
    core.bus().vdp().write_vram(static_cast<uint16_t>(superz80::VDP::kSpriteAttributeTableBase + 1U), 0x20U);
    core.bus().vdp().write_vram(static_cast<uint16_t>(superz80::VDP::kSpriteAttributeTableBase + 2U), 0x30U);
    core.bus().vdp().write_vram(static_cast<uint16_t>(superz80::VDP::kSpriteAttributeTableBase + 3U), 0x40U);
    const uint16_t last_sprite_base = static_cast<uint16_t>(
        superz80::VDP::kSpriteAttributeTableBase +
        ((superz80::VDP::kMaxSprites - 1U) * superz80::VDP::kSpriteSizeBytes));
    core.bus().vdp().write_vram(last_sprite_base, 0xAAU);
    core.bus().vdp().write_vram(static_cast<uint16_t>(last_sprite_base + 1U), 0xBBU);
    core.bus().vdp().write_vram(static_cast<uint16_t>(last_sprite_base + 2U), 0xCCU);
    core.bus().vdp().write_vram(static_cast<uint16_t>(last_sprite_base + 3U), 0xDDU);
    core.bus().write_port(superz80::Bus::kDmaSrcLowPort, 0x00U);
    core.bus().write_port(superz80::Bus::kDmaSrcHighPort, 0xC0U);
    core.bus().write_port(superz80::Bus::kDmaDstLowPort, 0x20U);
    core.bus().write_port(superz80::Bus::kDmaDstHighPort, 0xC0U);
    core.bus().write_port(superz80::Bus::kDmaLengthPort, 0x02U);
    core.bus().set_controller_button(superz80::IO::Button::Up, true);
    core.bus().set_controller_button(superz80::IO::Button::A, true);
    core.bus().set_controller_button(superz80::IO::Button::Start, true);

    const EmulatorCore::DmaSnapshot dma_idle_snapshot = core.dma_snapshot();
    const EmulatorCore::InputSnapshot input_snapshot = core.input_snapshot();
    const EmulatorCore::TimingSnapshot timing_reset_snapshot = core.timing_snapshot();
    core.bus().write_port(superz80::Bus::kDmaControlPort, superz80::DMA::kControlStartBit);
    const EmulatorCore::DmaSnapshot dma_active_snapshot = core.dma_snapshot();
    core.bus().write_port(superz80::Bus::kAudioToneALowPort, 0x34U);
    core.bus().write_port(superz80::Bus::kAudioToneAHighPort, 0x02U);
    core.bus().write_port(superz80::Bus::kAudioVolumeAPort, 0x05U);
    core.bus().write_port(superz80::Bus::kAudioNoiseControlPort, 0x03U);
    core.bus().write_port(superz80::Bus::kAudioControlPort, 0x01U);
    core.bus().write_port(superz80::Bus::kYm2151RegisterSelectPort, 0x20U);
    core.bus().write_port(superz80::Bus::kYm2151RegisterDataPort, 0x2DU);
    core.bus().write_port(superz80::Bus::kYm2151RegisterSelectPort, 0x28U);
    core.bus().write_port(superz80::Bus::kYm2151RegisterDataPort, 0x30U);
    core.step_scanline();
    const EmulatorCore::AudioSnapshot audio_snapshot = core.audio_snapshot();
    const EmulatorCore::TimingSnapshot timing_after_one_scanline_snapshot = core.timing_snapshot();
    core.step_scanlines(190U);
    const EmulatorCore::TimingSnapshot timing_frame_ready_snapshot = core.timing_snapshot();
    core.step_scanlines(1U);
    const EmulatorCore::TimingSnapshot timing_vblank_snapshot = core.timing_snapshot();
    core.step_scanlines(superz80::Scheduler::kScanlinesPerFrame - 192U);
    const EmulatorCore::TimingSnapshot timing_frame_wrap_snapshot = core.timing_snapshot();
    const EmulatorCore::RomSnapshot rom_snapshot = core.rom_snapshot();
    const EmulatorCore::RamSnapshot ram_snapshot = core.ram_snapshot();
    const EmulatorCore::VramSnapshot vram_snapshot = core.vram_snapshot();
    const EmulatorCore::SpriteTableSnapshot sprite_table_snapshot = core.sprite_table_snapshot();

    ok = expect_equal_u8("rom-snapshot-byte-0", rom_snapshot[0x0000U], 0x3EU) && ok;
    ok = expect_equal_u8("rom-snapshot-byte-1", rom_snapshot[0x0001U], 0x01U) && ok;
    ok = expect_equal_u8("rom-snapshot-open-bus-fill", rom_snapshot[0x0010U], 0xFFU) && ok;
    ok = expect_equal_u8("ram-snapshot-byte-0", ram_snapshot[0x0000U], 0x12U) && ok;
    ok = expect_equal_u8("ram-snapshot-byte-1", ram_snapshot[0x0001U], 0x34U) && ok;
    ok = expect_equal_u8("ram-snapshot-last-byte", ram_snapshot[0x3FFFU], 0xABU) && ok;
    ok = expect_equal_u8("vram-snapshot-byte-0", vram_snapshot[0x0000U], 0x56U) && ok;
    ok = expect_equal_u8("vram-snapshot-byte-1234", vram_snapshot[0x1234U], 0x78U) && ok;
    ok = expect_equal_u8("vram-snapshot-last-byte", vram_snapshot[0xFFFFU], 0x9AU) && ok;
    ok = expect_equal_u8("dma-snapshot-idle-src-low",
                         static_cast<uint8_t>(dma_idle_snapshot.source_address & 0x00FFU),
                         0x00U) && ok;
    ok = expect_equal_u8("dma-snapshot-idle-src-high",
                         static_cast<uint8_t>((dma_idle_snapshot.source_address >> 8U) & 0x00FFU),
                         0xC0U) && ok;
    ok = expect_equal_u8("dma-snapshot-idle-dst-low",
                         static_cast<uint8_t>(dma_idle_snapshot.destination_address & 0x00FFU),
                         0x20U) && ok;
    ok = expect_equal_u8("dma-snapshot-idle-dst-high",
                         static_cast<uint8_t>((dma_idle_snapshot.destination_address >> 8U) & 0x00FFU),
                         0xC0U) && ok;
    ok = expect_equal_u8("dma-snapshot-idle-length", dma_idle_snapshot.transfer_length, 0x02U) && ok;
    ok = expect_equal_u8("dma-snapshot-idle-control", dma_idle_snapshot.control, 0x00U) && ok;
    ok = expect_equal_u8("dma-snapshot-active-control",
                         dma_active_snapshot.control,
                         superz80::DMA::kControlBusyBit) && ok;
    ok = expect_equal_u8("dma-snapshot-active-flag",
                         static_cast<uint8_t>(dma_active_snapshot.active ? 1U : 0U),
                         0x01U) && ok;
    ok = expect_equal_u8("input-snapshot-up",
                         static_cast<uint8_t>(input_snapshot.up ? 1U : 0U),
                         0x01U) && ok;
    ok = expect_equal_u8("input-snapshot-a",
                         static_cast<uint8_t>(input_snapshot.a ? 1U : 0U),
                         0x01U) && ok;
    ok = expect_equal_u8("input-snapshot-start",
                         static_cast<uint8_t>(input_snapshot.start ? 1U : 0U),
                         0x01U) && ok;
    ok = expect_equal_u8("input-snapshot-pad1", input_snapshot.pad1, 0xEEU) && ok;
    ok = expect_equal_u8("input-snapshot-pad1-sys", input_snapshot.pad1_sys, 0xFEU) && ok;
    ok = expect_equal_u8("timing-reset-frame",
                         static_cast<uint8_t>(timing_reset_snapshot.frame_counter),
                         0x00U) && ok;
    ok = expect_equal_u8("timing-reset-scanline",
                         static_cast<uint8_t>(timing_reset_snapshot.scanline_counter),
                         0x00U) && ok;
    ok = expect_equal_u8("timing-reset-vblank",
                         static_cast<uint8_t>(timing_reset_snapshot.vblank_active ? 1U : 0U),
                         0x00U) && ok;
    ok = expect_equal_u8("timing-reset-frame-ready",
                         static_cast<uint8_t>(timing_reset_snapshot.frame_ready ? 1U : 0U),
                         0x00U) && ok;
    ok = expect_equal_u8("timing-one-scanline-frame",
                         static_cast<uint8_t>(timing_after_one_scanline_snapshot.frame_counter),
                         0x00U) && ok;
    ok = expect_equal_u8("timing-one-scanline-scanline",
                         static_cast<uint8_t>(timing_after_one_scanline_snapshot.scanline_counter),
                         0x01U) && ok;
    ok = expect_equal_u8("timing-scanlines-per-frame",
                         static_cast<uint8_t>(timing_after_one_scanline_snapshot.scanlines_per_frame),
                         static_cast<uint8_t>(superz80::Scheduler::kScanlinesPerFrame)) && ok;
    ok = expect_equal_u8("timing-instructions-per-scanline",
                         static_cast<uint8_t>(timing_after_one_scanline_snapshot.instructions_per_scanline),
                         static_cast<uint8_t>(superz80::Scheduler::kInstructionsPerScanline)) && ok;
    ok = expect_equal_u8("timing-ym-cycles-per-scanline",
                         static_cast<uint8_t>(timing_after_one_scanline_snapshot.ym2151_cycles_per_scanline),
                         static_cast<uint8_t>(superz80::Scheduler::kYm2151CyclesPerScanline)) && ok;
    ok = expect_equal_u8("timing-buffered-audio-samples",
                         static_cast<uint8_t>(timing_after_one_scanline_snapshot.buffered_audio_samples),
                         0x03U) && ok;
    ok = expect_equal_u8("timing-frame-ready-scanline",
                         static_cast<uint8_t>(timing_frame_ready_snapshot.scanline_counter),
                         0xBFU) && ok;
    ok = expect_equal_u8("timing-frame-ready-flag",
                         static_cast<uint8_t>(timing_frame_ready_snapshot.frame_ready ? 1U : 0U),
                         0x01U) && ok;
    ok = expect_equal_u8("timing-vblank-scanline",
                         static_cast<uint8_t>(timing_vblank_snapshot.scanline_counter),
                         0xC0U) && ok;
    ok = expect_equal_u8("timing-vblank-flag",
                         static_cast<uint8_t>(timing_vblank_snapshot.vblank_active ? 1U : 0U),
                         0x01U) && ok;
    ok = expect_equal_u8("timing-frame-wrap-frame",
                         static_cast<uint8_t>(timing_frame_wrap_snapshot.frame_counter),
                         0x01U) && ok;
    ok = expect_equal_u8("timing-frame-wrap-scanline",
                         static_cast<uint8_t>(timing_frame_wrap_snapshot.scanline_counter),
                         0x00U) && ok;
    ok = expect_equal_u8("audio-snapshot-psg-tone-a-low",
                         audio_snapshot.apu.registers[0U],
                         0x34U) && ok;
    ok = expect_equal_u8("audio-snapshot-psg-tone-a-high",
                         audio_snapshot.apu.registers[1U],
                         0x02U) && ok;
    ok = expect_equal_u8("audio-snapshot-psg-volume-a",
                         audio_snapshot.apu.registers[7U],
                         0x05U) && ok;
    ok = expect_equal_u8("audio-snapshot-psg-control",
                         audio_snapshot.apu.control,
                         0x01U) && ok;
    ok = expect_equal_u8("audio-snapshot-psg-enabled",
                         static_cast<uint8_t>(audio_snapshot.apu.enabled ? 1U : 0U),
                         0x01U) && ok;
    ok = expect_equal_u8("audio-snapshot-psg-noise-control",
                         audio_snapshot.apu.noise.control,
                         0x03U) && ok;
    ok = expect_equal_u8("audio-snapshot-ym-register-20",
                         audio_snapshot.ym2151.registers[0x20U],
                         0x2DU) && ok;
    ok = expect_equal_u8("audio-snapshot-ym-selected-register",
                         audio_snapshot.ym2151.selected_register,
                         0x28U) && ok;
    ok = expect_equal_u8("audio-snapshot-ym-channel-0-algorithm",
                         audio_snapshot.ym2151.channels[0U].algorithm,
                         0x05U) && ok;
    ok = expect_equal_u8("audio-snapshot-ym-channel-0-feedback",
                         audio_snapshot.ym2151.channels[0U].feedback,
                         0x05U) && ok;
    ok = expect_equal_u8("audio-snapshot-ym-channel-0-key-on-mask",
                         audio_snapshot.ym2151.channels[0U].key_on_mask,
                         0x03U) && ok;
    ok = expect_equal_u8("sprite-snapshot-first-y", sprite_table_snapshot[0U].y, 0x10U) && ok;
    ok = expect_equal_u8("sprite-snapshot-first-x", sprite_table_snapshot[0U].x, 0x20U) && ok;
    ok = expect_equal_u8("sprite-snapshot-first-tile", sprite_table_snapshot[0U].tile_index, 0x30U) && ok;
    ok = expect_equal_u8("sprite-snapshot-first-attr", sprite_table_snapshot[0U].attributes, 0x40U) && ok;
    ok = expect_equal_u8("sprite-snapshot-last-y",
                         sprite_table_snapshot[superz80::VDP::kMaxSprites - 1U].y,
                         0xAAU) && ok;
    ok = expect_equal_u8("sprite-snapshot-last-x",
                         sprite_table_snapshot[superz80::VDP::kMaxSprites - 1U].x,
                         0xBBU) && ok;
    ok = expect_equal_u8("sprite-snapshot-last-tile",
                         sprite_table_snapshot[superz80::VDP::kMaxSprites - 1U].tile_index,
                         0xCCU) && ok;
    ok = expect_equal_u8("sprite-snapshot-last-attr",
                         sprite_table_snapshot[superz80::VDP::kMaxSprites - 1U].attributes,
                         0xDDU) && ok;
    ok = expect_equal_u8("sprite-snapshot-entry-count",
                         static_cast<uint8_t>(sprite_table_snapshot.size()),
                         static_cast<uint8_t>(superz80::VDP::kMaxSprites)) && ok;

    EmulatorCore framebuffer_core;
    framebuffer_core.initialize();
    framebuffer_core.bus().vdp().write_vram(superz80::VDP::kBgTilemapBase, 0x01U);
    for (std::size_t index = 0U; index < superz80::VDP::kTileSizeBytes; ++index) {
        framebuffer_core.bus().vdp().write_vram(
            static_cast<uint16_t>(superz80::VDP::kPatternBank0Base +
                                  superz80::VDP::kTileSizeBytes + index),
            0x01U);
    }
    framebuffer_core.bus().vdp().set_palette_entry(0x01U, 0x11223344U);
    framebuffer_core.bus().vdp().render_frame();
    const EmulatorCore::FramebufferSnapshot framebuffer_snapshot =
        framebuffer_core.framebuffer_snapshot();
    ok = expect_equal_u32("framebuffer-snapshot-first-pixel",
                          framebuffer_snapshot[0U],
                          0x11223344U) && ok;

    EmulatorCore restore_core;
    restore_core.initialize();
    constexpr std::array<uint8_t, 8> kDeterminismRom = {
        0x00U, // NOP
        0x00U, // NOP
        0x00U, // NOP
        0xC3U, 0x00U, 0x00U, // JP 0000h
        0x00U,
        0x00U,
    };
    restore_core.load_rom(kDeterminismRom.data(), kDeterminismRom.size());
    restore_core.bus().write(0xC000U, 0x5AU);
    restore_core.bus().write(0xC001U, 0xA5U);
    restore_core.bus().write_port(superz80::Bus::kDmaSrcLowPort, 0x00U);
    restore_core.bus().write_port(superz80::Bus::kDmaSrcHighPort, 0xC0U);
    restore_core.bus().write_port(superz80::Bus::kDmaDstLowPort, 0x10U);
    restore_core.bus().write_port(superz80::Bus::kDmaDstHighPort, 0xC0U);
    restore_core.bus().write_port(superz80::Bus::kDmaLengthPort, 0x02U);
    restore_core.bus().write_port(superz80::Bus::kDmaControlPort, superz80::DMA::kControlStartBit);
    restore_core.bus().write_port(superz80::Bus::kAudioToneALowPort, 0x40U);
    restore_core.bus().write_port(superz80::Bus::kAudioToneAHighPort, 0x01U);
    restore_core.bus().write_port(superz80::Bus::kAudioVolumeAPort, 0x04U);
    restore_core.bus().write_port(superz80::Bus::kAudioControlPort, 0x01U);
    restore_core.bus().write_port(superz80::Bus::kYm2151RegisterSelectPort, 0x20U);
    restore_core.bus().write_port(superz80::Bus::kYm2151RegisterDataPort, 0x15U);
    restore_core.bus().write_port(superz80::Bus::kYm2151RegisterSelectPort, 0x28U);
    restore_core.bus().write_port(superz80::Bus::kYm2151RegisterDataPort, 0x10U);
    restore_core.bus().set_controller_button(superz80::IO::Button::Right, true);
    restore_core.step_scanlines(8U);
    const EmulatorCore::Snapshot restore_point = restore_core.capture_snapshot();
    restore_core.step_scanlines(24U);

    const EmulatorCore::CpuSnapshot cpu_after_restore_run = restore_core.cpu_snapshot();
    const EmulatorCore::TimingSnapshot timing_after_restore_run = restore_core.timing_snapshot();
    const EmulatorCore::AudioSnapshot audio_after_restore_run = restore_core.audio_snapshot();
    const EmulatorCore::InputSnapshot input_after_restore_run = restore_core.input_snapshot();
    const EmulatorCore::RamSnapshot ram_after_restore_run = restore_core.ram_snapshot();
    const std::vector<EmulatorCore::AudioSample> samples_after_restore_run =
        collect_audio_samples(restore_core);
    const uint32_t ram_crc_after_restore_run =
        checksum_bytes(ram_after_restore_run.data(), ram_after_restore_run.size());
    const uint32_t audio_crc_after_restore_run =
        checksum_bytes(samples_after_restore_run.data(), samples_after_restore_run.size());

    restore_core.restore_snapshot(restore_point);
    restore_core.step_scanlines(24U);

    const EmulatorCore::CpuSnapshot cpu_after_replay = restore_core.cpu_snapshot();
    const EmulatorCore::TimingSnapshot timing_after_replay = restore_core.timing_snapshot();
    const EmulatorCore::AudioSnapshot audio_after_replay = restore_core.audio_snapshot();
    const EmulatorCore::InputSnapshot input_after_replay = restore_core.input_snapshot();
    const EmulatorCore::RamSnapshot ram_after_replay = restore_core.ram_snapshot();
    const std::vector<EmulatorCore::AudioSample> samples_after_replay =
        collect_audio_samples(restore_core);
    const uint32_t ram_crc_after_replay =
        checksum_bytes(ram_after_replay.data(), ram_after_replay.size());
    const uint32_t audio_crc_after_replay =
        checksum_bytes(samples_after_replay.data(), samples_after_replay.size());

    ok = expect_equal_u16("snapshot-restore-cpu-pc", cpu_after_replay.pc, cpu_after_restore_run.pc) && ok;
    ok = expect_equal_u16("snapshot-restore-cpu-af", cpu_after_replay.af, cpu_after_restore_run.af) && ok;
    ok = expect_equal_u16("snapshot-restore-cpu-bc", cpu_after_replay.bc, cpu_after_restore_run.bc) && ok;
    ok = expect_equal_u16("snapshot-restore-cpu-de", cpu_after_replay.de, cpu_after_restore_run.de) && ok;
    ok = expect_equal_u16("snapshot-restore-cpu-hl", cpu_after_replay.hl, cpu_after_restore_run.hl) && ok;
    ok = expect_equal_u32("snapshot-restore-frame",
                          timing_after_replay.frame_counter,
                          timing_after_restore_run.frame_counter) && ok;
    ok = expect_equal_u32("snapshot-restore-scanline",
                          timing_after_replay.scanline_counter,
                          timing_after_restore_run.scanline_counter) && ok;
    ok = expect_equal_u32("snapshot-restore-buffered-audio",
                          static_cast<uint32_t>(timing_after_replay.buffered_audio_samples),
                          static_cast<uint32_t>(timing_after_restore_run.buffered_audio_samples)) && ok;
    ok = expect_equal_u8("snapshot-restore-vblank",
                         static_cast<uint8_t>(timing_after_replay.vblank_active ? 1U : 0U),
                         static_cast<uint8_t>(timing_after_restore_run.vblank_active ? 1U : 0U)) && ok;
    ok = expect_equal_u8("snapshot-restore-input-right",
                         static_cast<uint8_t>(input_after_replay.right ? 1U : 0U),
                         static_cast<uint8_t>(input_after_restore_run.right ? 1U : 0U)) && ok;
    ok = expect_equal_u8("snapshot-restore-audio-enabled",
                         static_cast<uint8_t>(audio_after_replay.apu.enabled ? 1U : 0U),
                         static_cast<uint8_t>(audio_after_restore_run.apu.enabled ? 1U : 0U)) && ok;
    ok = expect_equal_u16("snapshot-restore-ym-frequency",
                          audio_after_replay.ym2151.channels[0U].frequency,
                          audio_after_restore_run.ym2151.channels[0U].frequency) && ok;
    ok = expect_equal_u32("snapshot-restore-ram-crc",
                          ram_crc_after_replay,
                          ram_crc_after_restore_run) && ok;
    ok = expect_equal_u32("snapshot-restore-audio-crc",
                          audio_crc_after_replay,
                          audio_crc_after_restore_run) && ok;
    ok = expect_equal_u32("snapshot-restore-audio-sample-count",
                          static_cast<uint32_t>(samples_after_replay.size()),
                          static_cast<uint32_t>(samples_after_restore_run.size())) && ok;

    return ok ? 0 : 1;
}
