.globl _start
.globl _hlt
.extern
.intel_syntax noprefix

_start:
	call kernel_main

_hlt:
	hlt
	jmp _hlt
