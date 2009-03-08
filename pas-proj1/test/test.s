	.file	"test.c"
gcc2_compiled.:
.section	".text"
	.align 4
	.global main
	.type	 main,#function
	.proc	04
main:
	!#PROLOGUE# 0
	save	%sp, -112, %sp
	!#PROLOGUE# 1
	sethi	%hi(p), %o0
	or	%o0, %lo(p), %l0
	mov	17, %o0
	call	malloc, 0
	 nop
	st	%o0, [%l0]
	sethi	%hi(p), %o0
	or	%o0, %lo(p), %o1
	ld	[%o1], %o0
	call	free, 0
	 nop
.LL2:
	ret
	restore
.LLfe1:
	.size	 main,.LLfe1-main
	.common	p,4,4
	.ident	"GCC: (GNU) 2.95.2 19991024 (release)"
