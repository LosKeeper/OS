	.file	"ex1.c"
	.text
	.section	.rodata
.LC0:
	.string	""
	.text
	.globl	raler
	.type	raler, @function
raler:
.LFB6:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	subq	$224, %rsp
	movl	%edi, -212(%rbp)
	movq	%rsi, -224(%rbp)
	movq	%rdx, -160(%rbp)
	movq	%rcx, -152(%rbp)
	movq	%r8, -144(%rbp)
	movq	%r9, -136(%rbp)
	testb	%al, %al
	je	.L2
	movaps	%xmm0, -128(%rbp)
	movaps	%xmm1, -112(%rbp)
	movaps	%xmm2, -96(%rbp)
	movaps	%xmm3, -80(%rbp)
	movaps	%xmm4, -64(%rbp)
	movaps	%xmm5, -48(%rbp)
	movaps	%xmm6, -32(%rbp)
	movaps	%xmm7, -16(%rbp)
.L2:
	movq	%fs:40, %rax
	movq	%rax, -184(%rbp)
	xorl	%eax, %eax
	movl	$16, -208(%rbp)
	movl	$48, -204(%rbp)
	leaq	16(%rbp), %rax
	movq	%rax, -200(%rbp)
	leaq	-176(%rbp), %rax
	movq	%rax, -192(%rbp)
	movq	stderr(%rip), %rax
	leaq	-208(%rbp), %rdx
	movq	-224(%rbp), %rcx
	movq	%rcx, %rsi
	movq	%rax, %rdi
	call	vfprintf@PLT
	movq	stderr(%rip), %rax
	movq	%rax, %rsi
	movl	$10, %edi
	call	fputc@PLT
	cmpl	$1, -212(%rbp)
	jne	.L3
	leaq	.LC0(%rip), %rdi
	call	perror@PLT
.L3:
	movl	$1, %edi
	call	exit@PLT
	.cfi_endproc
.LFE6:
	.size	raler, .-raler
	.comm	compteur,8,8
	.globl	f
	.type	f, @function
f:
.LFB7:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	movl	$0, -4(%rbp)
	jmp	.L6
.L7:
	movq	compteur(%rip), %rax
	addq	$1, %rax
	movq	%rax, compteur(%rip)
	addl	$1, -4(%rbp)
.L6:
	cmpl	$999999, -4(%rbp)
	jle	.L7
	movl	$0, %eax
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE7:
	.size	f, .-f
	.section	.rodata
	.align 8
.LC1:
	.string	"pthread_create(&t[i], NULL, f, NULL)"
.LC2:
	.string	"pthread_join(t[i], NULL)"
.LC3:
	.string	"compteur = %ld\n"
	.text
	.globl	main
	.type	main, @function
main:
.LFB8:
	.cfi_startproc
	endbr64
	pushq	%rbp
	.cfi_def_cfa_offset 16
	.cfi_offset 6, -16
	movq	%rsp, %rbp
	.cfi_def_cfa_register 6
	pushq	%r12
	pushq	%rbx
	subq	$64, %rsp
	.cfi_offset 12, -24
	.cfi_offset 3, -32
	movq	%fs:40, %rax
	movq	%rax, -24(%rbp)
	xorl	%eax, %eax
	movl	$0, -72(%rbp)
	jmp	.L10
.L12:
	leaq	-64(%rbp), %rax
	movl	-72(%rbp), %edx
	movslq	%edx, %rdx
	salq	$3, %rdx
	leaq	(%rax,%rdx), %r12
	call	__errno_location@PLT
	movq	%rax, %rbx
	movl	$0, %ecx
	leaq	f(%rip), %rdx
	movl	$0, %esi
	movq	%r12, %rdi
	call	pthread_create@PLT
	movl	%eax, (%rbx)
	movl	(%rbx), %eax
	testl	%eax, %eax
	jle	.L11
	leaq	.LC1(%rip), %rsi
	movl	$1, %edi
	movl	$0, %eax
	call	raler
.L11:
	addl	$1, -72(%rbp)
.L10:
	cmpl	$3, -72(%rbp)
	jle	.L12
	movl	$0, -68(%rbp)
	jmp	.L13
.L15:
	movl	-68(%rbp), %eax
	cltq
	movq	-64(%rbp,%rax,8), %r12
	call	__errno_location@PLT
	movq	%rax, %rbx
	movl	$0, %esi
	movq	%r12, %rdi
	call	pthread_join@PLT
	movl	%eax, (%rbx)
	movl	(%rbx), %eax
	testl	%eax, %eax
	jle	.L14
	leaq	.LC2(%rip), %rsi
	movl	$1, %edi
	movl	$0, %eax
	call	raler
.L14:
	addl	$1, -68(%rbp)
.L13:
	cmpl	$3, -68(%rbp)
	jle	.L15
	movq	compteur(%rip), %rax
	movq	%rax, %rsi
	leaq	.LC3(%rip), %rdi
	movl	$0, %eax
	call	printf@PLT
	movl	$0, %eax
	movq	-24(%rbp), %rcx
	xorq	%fs:40, %rcx
	je	.L17
	call	__stack_chk_fail@PLT
.L17:
	addq	$64, %rsp
	popq	%rbx
	popq	%r12
	popq	%rbp
	.cfi_def_cfa 7, 8
	ret
	.cfi_endproc
.LFE8:
	.size	main, .-main
	.ident	"GCC: (Ubuntu 9.4.0-1ubuntu1~20.04.1) 9.4.0"
	.section	.note.GNU-stack,"",@progbits
	.section	.note.gnu.property,"a"
	.align 8
	.long	 1f - 0f
	.long	 4f - 1f
	.long	 5
0:
	.string	 "GNU"
1:
	.align 8
	.long	 0xc0000002
	.long	 3f - 2f
2:
	.long	 0x3
3:
	.align 8
4:
