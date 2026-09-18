	.file	"prog.c"
	.text
	.section	.rodata
.LC5:
	.string	"i: %f\n"
.LC6:
	.string	"j: %f\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB0:
	.cfi_startproc
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$48, %rsp
	movl	%edi, -36(%rbp)
	movq	%rsi, -48(%rbp)
	movss	.LC0(%rip), %xmm0
	movss	%xmm0, -4(%rbp)
	movss	.LC1(%rip), %xmm0
	movss	%xmm0, -8(%rbp)
	movss	-4(%rbp), %xmm0
	addss	-8(%rbp), %xmm0
	movss	.LC2(%rip), %xmm1
	xorps	%xmm1, %xmm0
	movss	%xmm0, -12(%rbp)
	movss	.LC3(%rip), %xmm0
	movss	%xmm0, -16(%rbp)
	movsd	.LC4(%rip), %xmm0
	movsd	%xmm0, -24(%rbp)
	pxor	%xmm2, %xmm2
	cvtss2sd	-16(%rbp), %xmm2
	movq	%xmm2, %rax
	movq	%rax, %xmm0
	movl	$.LC5, %edi
	movl	$1, %eax
	call	printf
	movq	-24(%rbp), %rax
	movq	%rax, %xmm0
	movl	$.LC6, %edi
	movl	$1, %eax
	call	printf
	movq	-48(%rbp), %rax
	movq	(%rax), %rax
	movzbl	(%rax), %eax
	movsbl	%al, %eax
	movl	%eax, -28(%rbp)
	movl	-28(%rbp), %eax
	leave
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE0:
	.size	main, .-main
	.section	.rodata
	.align 4
.LC0:
	.long	1073741824
	.align 4
.LC1:
	.long	1076887552
	.align 16
.LC2:
	.long	-2147483648
	.long	0
	.long	0
	.long	0
	.align 4
.LC3:
	.long	1045220557
	.align 8
.LC4:
	.long	-1717986918
	.long	1070176665
	.ident	"GCC: (GNU) 15.2.0"
	.section	.note.GNU-stack,"",@progbits
