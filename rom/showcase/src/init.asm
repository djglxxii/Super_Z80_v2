showcase_init:
    call showcase_vdp_init_registers
    call showcase_init_palette
    call showcase_prepare_bg_tilemap
    call showcase_upload_scene_to_vram
    ret
