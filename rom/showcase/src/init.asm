showcase_init:
    call showcase_vdp_init_registers
    call showcase_reset_scroll_state
    call showcase_reset_sprite_state
    call showcase_init_audio_state
    call showcase_init_palette
    call showcase_compose_parallax_scene
    call showcase_upload_scene_to_vram
    ret
