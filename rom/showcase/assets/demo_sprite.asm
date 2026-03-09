; Simple 8x8 diamond sprite for the Showcase sprite demo.

 MACRO SHOWCASE_SPRITE_ROW bits, ink
  db (((bits >> 7) & 1) * ink), (((bits >> 6) & 1) * ink), (((bits >> 5) & 1) * ink), (((bits >> 4) & 1) * ink)
  db (((bits >> 3) & 1) * ink), (((bits >> 2) & 1) * ink), (((bits >> 1) & 1) * ink), (((bits >> 0) & 1) * ink)
 ENDM

 SHOWCASE_SPRITE_ROW %00011000, 5
 SHOWCASE_SPRITE_ROW %00111100, 5
 SHOWCASE_SPRITE_ROW %01111110, 5
 SHOWCASE_SPRITE_ROW %11111111, 5
 SHOWCASE_SPRITE_ROW %11111111, 5
 SHOWCASE_SPRITE_ROW %01111110, 5
 SHOWCASE_SPRITE_ROW %00111100, 5
 SHOWCASE_SPRITE_ROW %00011000, 5
