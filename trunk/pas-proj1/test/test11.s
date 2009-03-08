	.file	"test11.pas"
gcc2_compiled.:
	.local	I
	.common	I,4,4
	.local	J
	.common	J,4,4
	.global _p_write
	.global _p_check_inoutres
.section	".rodata"
	.align 8
.LLC0:
	.asciz	"i = "
.section	".text"
	.align 4
	.global pascal_main_program
	.type	 pascal_main_program,#function
	.proc	020
pascal_main_program:
	!#PROLOGUE# 0
	save	%sp, -136, %sp
	!#PROLOGUE# 1
	sethi	%hi(J), %o1
	or	%o1, %lo(J), %o0
	mov	20, %o1
	st	%o1, [%o0]
	sethi	%hi(J), %o1
	or	%o1, %lo(J), %o0
	ld	[%o0], %o1
	st	%o1, [%fp-20]
	ld	[%fp-20], %o0
	cmp	%o0, 0
	ble	.LL3
	nop
	sethi	%hi(I), %o1
	or	%o1, %lo(I), %o0
	mov	1, %o1
	st	%o1, [%o0]
	stb	%g0, [%fp-21]
.LL4:
	ldub	[%fp-21], %o1
	and	%o1, 0xff, %o0
	cmp	%o0, 0
	be	.LL6
	nop
	sethi	%hi(I), %o1
	or	%o1, %lo(I), %o0
	ld	[%o0], %o1
	ld	[%fp-20], %o0
	cmp	%o1, %o0
	bne	.LL7
	nop
	b	.LL3
	 nop
.LL7:
	sethi	%hi(I), %o1
	or	%o1, %lo(I), %o0
	sethi	%hi(I), %o2
	or	%o2, %lo(I), %o1
	ld	[%o1], %o2
	add	%o2, 1, %o1
	st	%o1, [%o0]
.LL6:
	mov	1, %o0
	stb	%o0, [%fp-21]
	mov	3, %o0
	st	%o0, [%sp+92]
	sethi	%hi(I), %o1
	or	%o1, %lo(I), %o0
	ld	[%o0], %o1
	st	%o1, [%sp+96]
	mov	22, %o0
	st	%o0, [%sp+100]
	sethi	%hi(_p_stdout), %o1
	or	%o1, %lo(_p_stdout), %o0
	mov	384, %o1
	mov	3, %o2
	mov	17, %o3
	sethi	%hi(.LLC0), %o5
	or	%o5, %lo(.LLC0), %o4
	mov	4, %o5
	call	_p_write, 0
	 nop
	sethi	%hi(_p_InOutRes), %o1
	or	%o1, %lo(_p_InOutRes), %o0
	ld	[%o0], %o1
	cmp	%o1, 0
	be	.LL9
	nop
	call	_p_check_inoutres, 0
	 nop
	b	.LL9
	 nop
.LL8:
.LL9:
	sethi	%hi(J), %o1
	or	%o1, %lo(J), %o0
	sethi	%hi(J), %o2
	or	%o2, %lo(J), %o1
	ld	[%o1], %o2
	add	%o2, -1, %o1
	st	%o1, [%o0]
	b	.LL4
	 nop
.LL5:
.LL3:
.LL2:
	ret
	restore
.LLfe1:
	.size	 pascal_main_program,.LLfe1-pascal_main_program
.section	".data"
	.type	 ctor_run_condition_2.4,#object
	.size	 ctor_run_condition_2.4,1
ctor_run_condition_2.4:
	.byte	1
	.global _p_atexit
	.global _p_doinitproc
.section	".text"
	.align 4
	.global init_pascal_main_program
	.type	 init_pascal_main_program,#function
	.proc	020
init_pascal_main_program:
	!#PROLOGUE# 0
	save	%sp, -112, %sp
	!#PROLOGUE# 1
	sethi	%hi(ctor_run_condition_2.4), %o1
	or	%o1, %lo(ctor_run_condition_2.4), %o0
	ldub	[%o0], %o1
	and	%o1, 0xff, %o0
	cmp	%o0, 0
	be	.LL11
	nop
	sethi	%hi(ctor_run_condition_2.4), %o1
	or	%o1, %lo(ctor_run_condition_2.4), %o0
	stb	%g0, [%o0]
	sethi	%hi(fini_pascal_main_program), %o1
	or	%o1, %lo(fini_pascal_main_program), %o0
	call	_p_atexit, 0
	 nop
	call	_p_doinitproc, 0
	 nop
.LL11:
.LL10:
	ret
	restore
.LLfe2:
	.size	 init_pascal_main_program,.LLfe2-init_pascal_main_program
	.align 4
	.global fini_pascal_main_program
	.type	 fini_pascal_main_program,#function
	.proc	020
fini_pascal_main_program:
	!#PROLOGUE# 0
	save	%sp, -112, %sp
	!#PROLOGUE# 1
.LL12:
	ret
	restore
.LLfe3:
	.size	 fini_pascal_main_program,.LLfe3-fini_pascal_main_program
	.global init_pascal_main_program
	.global pascal_main_program
	.global _p_finalize
	.align 4
	.global main
	.type	 main,#function
	.proc	04
main:
	!#PROLOGUE# 0
	save	%sp, -112, %sp
	!#PROLOGUE# 1
	st	%i0, [%fp+68]
	st	%i1, [%fp+72]
	st	%i2, [%fp+76]
	ld	[%fp+68], %o0
	ld	[%fp+72], %o1
	ld	[%fp+76], %o2
	call	_p_initialize, 0
	 nop
	call	init_pascal_main_program, 0
	 nop
	call	pascal_main_program, 0
	 nop
	call	_p_finalize, 0
	 nop
	mov	0, %i0
	b	.LL13
	 nop
.LL13:
	ret
	restore
.LLfe4:
	.size	 main,.LLfe4-main
	.ident	"GCC: (GNU) 2.95.2 19991024 (release)"
