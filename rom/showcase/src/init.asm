showcase_init:
    call showcase_vdp_init_registers
    call showcase_init_palette
    call showcase_compose_system_splash
    call showcase_upload_scene_to_vram
    ret
