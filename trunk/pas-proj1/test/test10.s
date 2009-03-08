	.file	"test10.c"
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
	sethi	%hi(x), %o1
	or	%o1, %lo(x), %o0
	ld	[%o0], %o1
	cmp	%o1, 6
	ble	.LL3
	nop
	sethi	%hi(x), %o1
	or	%o1, %lo(x), %o0
	mov	7, %o1
	st	%o1, [%o0]
.LL3:
	sethi	%hi(y), %o1
	or	%o1, %lo(y), %o0
	ld	[%o0], %o1
	cmp	%o1, 6
	bleu	.LL4
	nop
	sethi	%hi(y), %o1
	or	%o1, %lo(y), %o0
	mov	7, %o1
	st	%o1, [%o0]
.LL4:
.LL2:
	ret
	restore
.LLfe1:
	.size	 main,.LLfe1-main
	.common	x,4,4
	.common	y,4,4
	.ident	"GCC: (GNU) 2.95.2 19991024 (release)"
