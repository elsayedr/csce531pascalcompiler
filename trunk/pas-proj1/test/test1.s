	.file	"test.pas"
gcc2_compiled.:
	.local	Xyz
	.common	Xyz,4,4
.section	".text"
	.align 4
	.global pascal_main_program
	.type	 pascal_main_program,#function
	.proc	020
pascal_main_program:
	!#PROLOGUE# 0
	save	%sp, -112, %sp
	!#PROLOGUE# 1
	sethi	%hi(Xyz), %o1
	or	%o1, %lo(Xyz), %o0
	mov	3, %o1
	st	%o1, [%o0]
	sethi	%hi(Xyz), %o1
	or	%o1, %lo(Xyz), %o0
	mov	4, %o1
	st	%o1, [%o0]
	sethi	%hi(Xyz), %o1
	or	%o1, %lo(Xyz), %o0
	mov	5, %o1
	st	%o1, [%o0]
.LL2:
	ret
	restore
.LLfe1:
	.size	 pascal_main_program,.LLfe1-pascal_main_program
.section	".data"
	.type	 ctor_run_condition_0.4,#object
	.size	 ctor_run_condition_0.4,1
ctor_run_condition_0.4:
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
	sethi	%hi(ctor_run_condition_0.4), %o1
	or	%o1, %lo(ctor_run_condition_0.4), %o0
	ldub	[%o0], %o1
	and	%o1, 0xff, %o0
	cmp	%o0, 0
	be	.LL4
	nop
	sethi	%hi(ctor_run_condition_0.4), %o1
	or	%o1, %lo(ctor_run_condition_0.4), %o0
	stb	%g0, [%o0]
	sethi	%hi(fini_pascal_main_program), %o1
	or	%o1, %lo(fini_pascal_main_program), %o0
	call	_p_atexit, 0
	 nop
	call	_p_doinitproc, 0
	 nop
.LL4:
.LL3:
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
.LL5:
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
	b	.LL6
	 nop
.LL6:
	ret
	restore
.LLfe4:
	.size	 main,.LLfe4-main
	.ident	"GCC: (GNU) 2.95.2 19991024 (release)"
