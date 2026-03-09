; Minimal 32x24 splash tilemap contract.

 MACRO SZ_SPLASH_MAP_ROW left_fill, center0, center1, center2, center3, right_fill
  defs left_fill, 0
  db center0, center1, center2, center3
  defs right_fill, 0
 ENDM

sz_splash_tilemap:
 defs (11 * 32), 0
 SZ_SPLASH_MAP_ROW 14, 3, 1, 2, 4, 14
 SZ_SPLASH_MAP_ROW 14, 3, 1, 2, 4, 14
 defs (11 * 32), 0

sz_splash_tilemap_end:
sz_splash_tilemap_bytes EQU sz_splash_tilemap_end - sz_splash_tilemap
