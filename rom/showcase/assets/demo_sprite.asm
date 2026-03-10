; Two 8x8 animation frames for the Showcase sprite demo.
; Frame 0 is a filled diamond. Frame 1 is a pinched diamond with a
; visible center split so tile switching is obvious.

 MACRO SHOWCASE_SPRITE_ROW bits, ink
  db (((bits >> 7) & 1) * ink), (((bits >> 6) & 1) * ink), (((bits >> 5) & 1) * ink), (((bits >> 4) & 1) * ink)
  db (((bits >> 3) & 1) * ink), (((bits >> 2) & 1) * ink), (((bits >> 1) & 1) * ink), (((bits >> 0) & 1) * ink)
 ENDM

 ; Frame 0
 SHOWCASE_SPRITE_ROW %00011000, 5
 SHOWCASE_SPRITE_ROW %00111100, 5
 SHOWCASE_SPRITE_ROW %01111110, 5
 SHOWCASE_SPRITE_ROW %11111111, 5
 SHOWCASE_SPRITE_ROW %11111111, 5
 SHOWCASE_SPRITE_ROW %01111110, 5
 SHOWCASE_SPRITE_ROW %00111100, 5
 SHOWCASE_SPRITE_ROW %00011000, 5

 ; Frame 1
 SHOWCASE_SPRITE_ROW %00011000, 5
 SHOWCASE_SPRITE_ROW %00111100, 5
 SHOWCASE_SPRITE_ROW %01100110, 5
 SHOWCASE_SPRITE_ROW %11000011, 5
 SHOWCASE_SPRITE_ROW %11000011, 5
 SHOWCASE_SPRITE_ROW %01100110, 5
 SHOWCASE_SPRITE_ROW %00111100, 5
 SHOWCASE_SPRITE_ROW %00011000, 5
