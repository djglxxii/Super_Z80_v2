; Super_Z80 controller include surface.

SZ_PORT_PAD1                 EQU $DC
SZ_PORT_PAD1_SYS             EQU $DD

SZ_PAD1_UP                   EQU $01
SZ_PAD1_DOWN                 EQU $02
SZ_PAD1_LEFT                 EQU $04
SZ_PAD1_RIGHT                EQU $08
SZ_PAD1_A                    EQU $10
SZ_PAD1_B                    EQU $20
SZ_PAD1_UNUSED_MASK          EQU $C0

SZ_PAD1_SYS_START            EQU $01

SZ_PAD_IDLE                  EQU $FF
SZ_PAD_SYS_IDLE              EQU $FF

 MACRO SZ_INPUT_READ_PAD1 target_reg
  in target_reg, (SZ_PORT_PAD1)
 ENDM

 MACRO SZ_INPUT_READ_PAD1_SYS target_reg
  in target_reg, (SZ_PORT_PAD1_SYS)
 ENDM
