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

 ; Tile 1: S
 SHOWCASE_SPLASH_TILE 1, %00111110, %01100000, %01100000, %00111100, %00000110, %00000110, %01111100, %00000000
 ; Tile 2: Z
 SHOWCASE_SPLASH_TILE 2, %01111110, %00000110, %00001100, %00011000, %00110000, %01100000, %01111110, %00000000
 ; Tile 3: frame corner accent
 SHOWCASE_SPLASH_TILE 3, %11111111, %11000000, %10111111, %10100001, %10100001, %10111101, %10000001, %11111111
; Tile 4: solid fill accent
 defs 64, 4
