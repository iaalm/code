	.file	"bar.c"
	.section	.rodata
.LC0:
	.string	"a is bigger"
.LC1:
	.string	"b is bigger"
	.text
	.globl	choose
	.type	choose, @function
choose:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$16, %rsp
	movl	%edi, -4(%rbp)
	movl	%esi, -8(%rbp)
	movl	-4(%rbp), %eax
	cmpl	-8(%rbp), %eax
	jle	.L2
	movl	$11, %esi
	movl	$.LC0, %edi
	call	myprint
	jmp	.L3
.L2:
	movl	$11, %esi
	movl	$.LC1, %edi
	call	myprint
.L3:
	movl	$0, %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	choose, .-choose
	.ident	"GCC: (GNU) 4.6.3"
	.section	.note.GNU-stack,"",@progbits
