; Minimal 32x24 splash tilemap for the Showcase ROM scaffold.

 MACRO SHOWCASE_SPLASH_MAP_ROW left_fill, center0, center1, center2, center3, right_fill
  defs left_fill, 0
  db center0, center1, center2, center3
  defs right_fill, 0
 ENDM

defs (11 * 32), 0
 SHOWCASE_SPLASH_MAP_ROW 14, 3, 1, 2, 4, 14
 SHOWCASE_SPLASH_MAP_ROW 14, 3, 1, 2, 4, 14
 defs (11 * 32), 0
