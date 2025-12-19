    .module blink
	.optsdcc -mpdk14

    .area DATA
	.area OSEG (OVR,DATA)

    .globl _main_asm

    .area RSEG (ABS)
    .org 0x0000
    __flag	=	0x0000
    __sp	=	0x0002
    __clkmd	=	0x0003
    __ihrcr	=	0x000b
    __ilrcr	=	0x0039
    __eoscr	=	0x000a
    __inten	=	0x0004
    __intrq	=	0x0005
    __integs	=	0x000c
    __padier	=	0x000d
    __pa	=	0x0010
    __pac	=	0x0011
    __paph	=	0x0012
    __pbdier	=	0x000e
    __pb	=	0x0014
    __pbc	=	0x0015
    __pbph	=	0x0016
    __t16m	=	0x0006
    __t16c::
        .ds 2
    __tm2c	=	0x001c
    __tm2ct	=	0x001d
    __tm2s	=	0x0017
    __tm2b	=	0x0009
    __tm3c	=	0x0032
    __tm3ct	=	0x0033
    __tm3s	=	0x0034
    __tm3b	=	0x0035
    __bgtr	=	0x001a
    __gpcc	=	0x0018
    __gpcs	=	0x0019
    __rfcc	=	0x0036
    __rfccrh	=	0x0037
    __rfccrl	=	0x0038
    __pwmg0c	=	0x0020
    __pwmg0s	=	0x0021
    __pwmg0dth	=	0x0022
    __pwmg0dtl	=	0x0023
    __pwmg0cubh	=	0x0024
    __pwmg0cubl	=	0x0025
    __pwmg1c	=	0x0026
    __pwmg1s	=	0x0027
    __pwmg1dth	=	0x0028
    __pwmg1dtl	=	0x0029
    __pwmg1cubh	=	0x002a
    __pwmg1cubl	=	0x002b
    __pwmg2c	=	0x002c
    __pwmg2s	=	0x002d
    __pwmg2dth	=	0x002e
    __pwmg2dtl	=	0x002f
    __pwmg2cubh	=	0x0030
    __pwmg2cubl	=	0x0031
    __misc	=	0x0008
    __misc2	=	0x000f
    __misclvr	=	0x001b

	.area DATA
    _delay_loop_32_PARM:
	.ds 4
	.area SSEG
	.area HOME
	.area HEADER (ABS)
	.area HOME
	.area GSINIT
	.area GSFINAL
	.area GSINIT
	.area PREG (ABS)
	.area GSINIT
	.area GSFINAL
	.area HOME
	.area HOME
	.area CODE
_main_asm:
    ; Set 4 pin to output
    set1.io __pac, #4

    ; Main loop
_loop:
    ; Turn led off
    set0.io __pa, #4

    ; Delay 100000 (0x0186a0)
    mov	a, #0xa0
	mov	_delay_loop_32_PARM+0, a
	mov	a, #0x86
	mov	_delay_loop_32_PARM+1, a
	mov	a, #0x01
	mov	_delay_loop_32_PARM+2, a
	clear _delay_loop_32_PARM+3
	call _delay_loop_32

    ; Turn led on
    set1.io __pa, #4

     ; Delay 100000 (0x0186a0)                                                                                                                 
    mov	a, #0xa0
	mov	_delay_loop_32_PARM+0, a
	mov	a, #0x86
	mov	_delay_loop_32_PARM+1, a
	mov	a, #0x01
	mov	_delay_loop_32_PARM+2, a
	clear _delay_loop_32_PARM+3
	call _delay_loop_32
    goto _loop

_delay_loop_32:
	_delay_loop_32_1:
	dec	_delay_loop_32_PARM+0
	subc _delay_loop_32_PARM+1
	subc _delay_loop_32_PARM+2
	subc _delay_loop_32_PARM+3
	mov a, _delay_loop_32_PARM+0
	or a, _delay_loop_32_PARM+1
	or a, _delay_loop_32_PARM+2
	or a, _delay_loop_32_PARM+3
	t1sn.io	f, z
	goto _delay_loop_32_1
	ret

	.area CODE
	.area CONST
	.area CABS (ABS)