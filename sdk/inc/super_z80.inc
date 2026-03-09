; Super_Z80 canonical SDK include root.
; Include this file first from ROM sources. It pulls in the small stable
; platform definitions for VDP, input, IRQ, DMA, audio, and basic runtime use.

 INCLUDE "vdp.inc"
 INCLUDE "input.inc"

SZ_ROM_BASE                  EQU $0000
SZ_ROM_LIMIT                 EQU $8000
SZ_IRQ_VECTOR                EQU $0038
SZ_RUNTIME_MAIN_ORG          EQU $0100

SZ_RAM_BASE                  EQU $C000
SZ_RAM_END                   EQU $FFFF
SZ_STACK_TOP                 EQU $FFFE

SZ_PORT_IRQ_STATUS           EQU $F0
SZ_PORT_IRQ_ENABLE           EQU $F1
SZ_PORT_IRQ_ACK              EQU $F2
SZ_IRQ_VBLANK_BIT            EQU $01

SZ_PORT_DMA_SRC_LOW          EQU $F4
SZ_PORT_DMA_SRC_HIGH         EQU $F5
SZ_PORT_DMA_DST_LOW          EQU $F6
SZ_PORT_DMA_DST_HIGH         EQU $F7
SZ_PORT_DMA_LENGTH           EQU $F8
SZ_PORT_DMA_CONTROL          EQU $F9

SZ_PORT_AUD_TONE_A_LOW       EQU $D0
SZ_PORT_AUD_TONE_A_HIGH      EQU $D1
SZ_PORT_AUD_TONE_B_LOW       EQU $D2
SZ_PORT_AUD_TONE_B_HIGH      EQU $D3
SZ_PORT_AUD_TONE_C_LOW       EQU $D4
SZ_PORT_AUD_TONE_C_HIGH      EQU $D5
SZ_PORT_AUD_NOISE_CTRL       EQU $D6
SZ_PORT_AUD_VOL_A            EQU $D7
SZ_PORT_AUD_VOL_B            EQU $D8
SZ_PORT_AUD_VOL_C            EQU $D9
SZ_PORT_AUD_VOL_N            EQU $DA
SZ_PORT_AUD_CTRL             EQU $DB
SZ_AUD_VOL_SILENT            EQU $0F
SZ_AUD_CTRL_ENABLE           EQU $01
SZ_AUD_CTRL_MUTE             EQU $02
SZ_AUD_CTRL_RESET_STATE      EQU $04

SZ_PORT_YM2151_ADDR          EQU $70
SZ_PORT_YM2151_DATA          EQU $71

 MACRO SZ_PORT_WRITE_IMM port, value
  ld a, value
  out (port), a
 ENDM

 MACRO SZ_PORT_READ target_reg, port
  in target_reg, (port)
 ENDM

 MACRO SZ_DISABLE_IRQS
  di
 ENDM

 MACRO SZ_ENABLE_IRQS
  ei
 ENDM
