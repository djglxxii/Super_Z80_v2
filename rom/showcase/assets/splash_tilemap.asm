; Minimal 32x24 splash tilemap for the Showcase ROM scaffold.

 MACRO SHOWCASE_SPLASH_MAP_ROW left_fill, center0, center1, center2, center3, center4, center5, right_fill
  defs left_fill, 0
  db center0, center1, center2, center3, center4, center5
  defs right_fill, 0
 ENDM

defs (5 * 32), 0
SHOWCASE_SPLASH_MAP_ROW 13, showcase_splash_tile_base + 1, showcase_splash_tile_base + 3, showcase_splash_tile_base + 3, showcase_splash_tile_base + 3, showcase_splash_tile_base + 3, showcase_splash_tile_base + 2, 13
SHOWCASE_SPLASH_MAP_ROW 13, showcase_splash_tile_base + 1, showcase_splash_tile_base + 4, showcase_splash_tile_base + 6, showcase_splash_tile_base + 5, showcase_splash_tile_base + 7, showcase_splash_tile_base + 2, 13
SHOWCASE_SPLASH_MAP_ROW 13, showcase_splash_tile_base + 1, showcase_splash_tile_base + 7, showcase_splash_tile_base + 7, showcase_splash_tile_base + 7, showcase_splash_tile_base + 7, showcase_splash_tile_base + 2, 13
SHOWCASE_SPLASH_MAP_ROW 13, showcase_splash_tile_base + 1, showcase_splash_tile_base + 4, showcase_splash_tile_base + 6, showcase_splash_tile_base + 5, showcase_splash_tile_base + 7, showcase_splash_tile_base + 2, 13
SHOWCASE_SPLASH_MAP_ROW 13, showcase_splash_tile_base + 1, showcase_splash_tile_base + 3, showcase_splash_tile_base + 3, showcase_splash_tile_base + 3, showcase_splash_tile_base + 3, showcase_splash_tile_base + 2, 13
defs (14 * 32), 0
