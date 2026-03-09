; Minimal splash/logo tiles in Showcase ROM 8x8x8bpp tile format.

 MACRO SHOWCASE_SPLASH_ROW bits, ink
  db (((bits >> 7) & 1) * ink), (((bits >> 6) & 1) * ink), (((bits >> 5) & 1) * ink), (((bits >> 4) & 1) * ink)
  db (((bits >> 3) & 1) * ink), (((bits >> 2) & 1) * ink), (((bits >> 1) & 1) * ink), (((bits >> 0) & 1) * ink)
 ENDM

 MACRO SHOWCASE_SPLASH_TILE ink, r0, r1, r2, r3, r4, r5, r6, r7
  SHOWCASE_SPLASH_ROW r0, ink
  SHOWCASE_SPLASH_ROW r1, ink
  SHOWCASE_SPLASH_ROW r2, ink
  SHOWCASE_SPLASH_ROW r3, ink
  SHOWCASE_SPLASH_ROW r4, ink
  SHOWCASE_SPLASH_ROW r5, ink
  SHOWCASE_SPLASH_ROW r6, ink
  SHOWCASE_SPLASH_ROW r7, ink
 ENDM

; Tile 0: blank
 defs 64, 0

 ; Tile 1: left frame cap
 SHOWCASE_SPLASH_TILE 2, %00000111, %00011111, %00111111, %01111111, %01111111, %00111111, %00011111, %00000111
 ; Tile 2: right frame cap
 SHOWCASE_SPLASH_TILE 2, %11100000, %11111000, %11111100, %11111110, %11111110, %11111100, %11111000, %11100000
 ; Tile 3: horizontal frame fill
 defs 64, 2
 ; Tile 4: logo S
 SHOWCASE_SPLASH_TILE 1, %00111110, %01100000, %01100000, %00111100, %00000110, %00000110, %01111100, %00000000
 ; Tile 5: logo Z
 SHOWCASE_SPLASH_TILE 2, %01111110, %00000110, %00001100, %00011000, %00110000, %01100000, %01111110, %00000000
 ; Tile 6: center accent bar
 SHOWCASE_SPLASH_TILE 3, %00000000, %11111111, %11111111, %00011000, %00011000, %11111111, %11111111, %00000000
 ; Tile 7: system block
 defs 64, 4
