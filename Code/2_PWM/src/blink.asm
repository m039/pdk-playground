    .module blink
	.optsdcc -mpdk14

    .area DATA
	.area OSEG (OVR,DATA)
    .globl _main

    .area HOME
_main:
    nop
    goto _main
