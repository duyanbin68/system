	.file	"vsprintf.c"
	.text
	.globl	strlen
	.type	strlen, @function
strlen:
.LFB0:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$16, %esp
	movl	$0, -4(%ebp)
	jmp	.L2
.L3:
	addl	$1, -4(%ebp)
.L2:
	movl	8(%ebp), %eax
	leal	1(%eax), %edx
	movl	%edx, 8(%ebp)
	movzbl	(%eax), %eax
	testb	%al, %al
	jne	.L3
	movl	-4(%ebp), %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE0:
	.size	strlen, .-strlen
	.type	skip_atoi, @function
skip_atoi:
.LFB1:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	pushl	%ebx
	subl	$16, %esp
	.cfi_offset 3, -12
	movl	$0, -8(%ebp)
	jmp	.L6
.L8:
	movl	-8(%ebp), %edx
	movl	%edx, %eax
	sall	$2, %eax
	addl	%edx, %eax
	addl	%eax, %eax
	movl	%eax, %ebx
	movl	8(%ebp), %eax
	movl	(%eax), %eax
	leal	1(%eax), %ecx
	movl	8(%ebp), %edx
	movl	%ecx, (%edx)
	movzbl	(%eax), %eax
	movsbl	%al, %eax
	addl	%ebx, %eax
	subl	$48, %eax
	movl	%eax, -8(%ebp)
.L6:
	movl	8(%ebp), %eax
	movl	(%eax), %eax
	movzbl	(%eax), %eax
	cmpb	$47, %al
	jle	.L7
	movl	8(%ebp), %eax
	movl	(%eax), %eax
	movzbl	(%eax), %eax
	cmpb	$57, %al
	jle	.L8
.L7:
	movl	-8(%ebp), %eax
	addl	$16, %esp
	popl	%ebx
	.cfi_restore 3
	popl	%ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE1:
	.size	skip_atoi, .-skip_atoi
	.section	.rodata
	.align 4
.LC0:
	.string	"0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ"
	.align 4
.LC1:
	.string	"0123456789abcdefghijklmnopqrstuvwxyz"
	.text
	.type	number, @function
number:
.LFB2:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$64, %esp
	movl	$.LC0, -8(%ebp)
	movl	28(%ebp), %eax
	andl	$64, %eax
	testl	%eax, %eax
	je	.L11
	movl	$.LC1, -8(%ebp)
.L11:
	movl	28(%ebp), %eax
	andl	$16, %eax
	testl	%eax, %eax
	je	.L12
	andl	$-2, 28(%ebp)
.L12:
	cmpl	$1, 16(%ebp)
	jle	.L13
	cmpl	$36, 16(%ebp)
	jle	.L14
.L13:
	movl	$0, %eax
	jmp	.L47
.L14:
	movl	28(%ebp), %eax
	andl	$1, %eax
	testl	%eax, %eax
	je	.L16
	movl	$48, %eax
	jmp	.L17
.L16:
	movl	$32, %eax
.L17:
	movb	%al, -13(%ebp)
	movl	28(%ebp), %eax
	andl	$2, %eax
	testl	%eax, %eax
	je	.L18
	cmpl	$0, 12(%ebp)
	jns	.L18
	movb	$45, -1(%ebp)
	negl	12(%ebp)
	jmp	.L19
.L18:
	movl	28(%ebp), %eax
	andl	$4, %eax
	testl	%eax, %eax
	jne	.L20
	movl	28(%ebp), %eax
	andl	$8, %eax
	testl	%eax, %eax
	je	.L21
	movl	$32, %eax
	jmp	.L23
.L21:
	movl	$0, %eax
	jmp	.L23
.L20:
	movl	$43, %eax
.L23:
	movb	%al, -1(%ebp)
.L19:
	cmpb	$0, -1(%ebp)
	je	.L24
	subl	$1, 20(%ebp)
.L24:
	movl	28(%ebp), %eax
	andl	$32, %eax
	testl	%eax, %eax
	je	.L25
	cmpl	$16, 16(%ebp)
	jne	.L26
	subl	$2, 20(%ebp)
	jmp	.L25
.L26:
	cmpl	$8, 16(%ebp)
	jne	.L25
	subl	$1, 20(%ebp)
.L25:
	movl	$0, -12(%ebp)
	cmpl	$0, 12(%ebp)
	jne	.L27
	movl	-12(%ebp), %eax
	leal	1(%eax), %edx
	movl	%edx, -12(%ebp)
	movb	$48, -56(%ebp,%eax)
	jmp	.L28
.L27:
	jmp	.L29
.L30:
	movl	-12(%ebp), %ecx
	leal	1(%ecx), %eax
	movl	%eax, -12(%ebp)
	movl	$0, %edx
	movl	16(%ebp), %eax
#APP
# 112 "vsprintf.c" 1
	divl %eax
# 0 "" 2
#NO_APP
	movl	%eax, 12(%ebp)
	movl	%edx, -20(%ebp)
	movl	-20(%ebp), %eax
	movl	%eax, %edx
	movl	-8(%ebp), %eax
	addl	%edx, %eax
	movzbl	(%eax), %eax
	movb	%al, -56(%ebp,%ecx)
.L29:
	cmpl	$0, 12(%ebp)
	jne	.L30
.L28:
	movl	-12(%ebp), %eax
	cmpl	24(%ebp), %eax
	jle	.L31
	movl	-12(%ebp), %eax
	movl	%eax, 24(%ebp)
.L31:
	movl	24(%ebp), %eax
	subl	%eax, 20(%ebp)
	movl	28(%ebp), %eax
	andl	$17, %eax
	testl	%eax, %eax
	jne	.L32
	jmp	.L33
.L34:
	movl	8(%ebp), %eax
	leal	1(%eax), %edx
	movl	%edx, 8(%ebp)
	movb	$32, (%eax)
.L33:
	movl	20(%ebp), %eax
	leal	-1(%eax), %edx
	movl	%edx, 20(%ebp)
	testl	%eax, %eax
	jg	.L34
.L32:
	cmpb	$0, -1(%ebp)
	je	.L35
	movl	8(%ebp), %eax
	leal	1(%eax), %edx
	movl	%edx, 8(%ebp)
	movzbl	-1(%ebp), %edx
	movb	%dl, (%eax)
.L35:
	movl	28(%ebp), %eax
	andl	$32, %eax
	testl	%eax, %eax
	je	.L36
	cmpl	$8, 16(%ebp)
	jne	.L37
	movl	8(%ebp), %eax
	leal	1(%eax), %edx
	movl	%edx, 8(%ebp)
	movb	$48, (%eax)
	jmp	.L36
.L37:
	cmpl	$16, 16(%ebp)
	jne	.L36
	movl	8(%ebp), %eax
	leal	1(%eax), %edx
	movl	%edx, 8(%ebp)
	movb	$48, (%eax)
	movl	8(%ebp), %eax
	leal	1(%eax), %edx
	movl	%edx, 8(%ebp)
	movl	-8(%ebp), %edx
	movzbl	33(%edx), %edx
	movb	%dl, (%eax)
.L36:
	movl	28(%ebp), %eax
	andl	$16, %eax
	testl	%eax, %eax
	jne	.L38
	jmp	.L39
.L40:
	movl	8(%ebp), %eax
	leal	1(%eax), %edx
	movl	%edx, 8(%ebp)
	movzbl	-13(%ebp), %edx
	movb	%dl, (%eax)
.L39:
	movl	20(%ebp), %eax
	leal	-1(%eax), %edx
	movl	%edx, 20(%ebp)
	testl	%eax, %eax
	jg	.L40
.L38:
	jmp	.L41
.L42:
	movl	8(%ebp), %eax
	leal	1(%eax), %edx
	movl	%edx, 8(%ebp)
	movb	$48, (%eax)
.L41:
	movl	24(%ebp), %eax
	leal	-1(%eax), %edx
	movl	%edx, 24(%ebp)
	cmpl	-12(%ebp), %eax
	jg	.L42
	jmp	.L43
.L44:
	movl	8(%ebp), %eax
	leal	1(%eax), %edx
	movl	%edx, 8(%ebp)
	leal	-56(%ebp), %ecx
	movl	-12(%ebp), %edx
	addl	%ecx, %edx
	movzbl	(%edx), %edx
	movb	%dl, (%eax)
.L43:
	movl	-12(%ebp), %eax
	leal	-1(%eax), %edx
	movl	%edx, -12(%ebp)
	testl	%eax, %eax
	jg	.L44
	jmp	.L45
.L46:
	movl	8(%ebp), %eax
	leal	1(%eax), %edx
	movl	%edx, 8(%ebp)
	movb	$32, (%eax)
.L45:
	movl	20(%ebp), %eax
	leal	-1(%eax), %edx
	movl	%edx, 20(%ebp)
	testl	%eax, %eax
	jg	.L46
	movl	8(%ebp), %eax
.L47:
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE2:
	.size	number, .-number
	.globl	vsprintf
	.type	vsprintf, @function
vsprintf:
.LFB3:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$56, %esp
	movl	8(%ebp), %eax
	movl	%eax, -4(%ebp)
	jmp	.L49
.L76:
	movl	12(%ebp), %eax
	movzbl	(%eax), %eax
	cmpb	$37, %al
	je	.L50
	movl	-4(%ebp), %eax
	leal	1(%eax), %edx
	movl	%edx, -4(%ebp)
	movl	12(%ebp), %edx
	movzbl	(%edx), %edx
	movb	%dl, (%eax)
	jmp	.L51
.L50:
	movl	$0, -8(%ebp)
.L52:
	movl	12(%ebp), %eax
	addl	$1, %eax
	movl	%eax, 12(%ebp)
	movl	12(%ebp), %eax
	movzbl	(%eax), %eax
	movsbl	%al, %eax
	subl	$32, %eax
	cmpl	$16, %eax
	ja	.L53
	movl	.L55(,%eax,4), %eax
	jmp	*%eax
	.section	.rodata
	.align 4
	.align 4
.L55:
	.long	.L54
	.long	.L53
	.long	.L53
	.long	.L56
	.long	.L53
	.long	.L53
	.long	.L53
	.long	.L53
	.long	.L53
	.long	.L53
	.long	.L53
	.long	.L57
	.long	.L53
	.long	.L58
	.long	.L53
	.long	.L53
	.long	.L59
	.text
.L58:
	orl	$16, -8(%ebp)
	jmp	.L52
.L57:
	orl	$4, -8(%ebp)
	jmp	.L52
.L54:
	orl	$8, -8(%ebp)
	jmp	.L52
.L56:
	orl	$32, -8(%ebp)
	jmp	.L52
.L59:
	orl	$1, -8(%ebp)
	jmp	.L52
.L53:
	movl	$-1, -12(%ebp)
	movl	12(%ebp), %eax
	movzbl	(%eax), %eax
	cmpb	$47, %al
	jle	.L60
	movl	12(%ebp), %eax
	movzbl	(%eax), %eax
	cmpb	$57, %al
	jg	.L60
	leal	12(%ebp), %eax
	movl	%eax, (%esp)
	call	skip_atoi
	movl	%eax, -12(%ebp)
	jmp	.L61
.L60:
	movl	12(%ebp), %eax
	movzbl	(%eax), %eax
	cmpb	$42, %al
	jne	.L61
	addl	$4, 16(%ebp)
	movl	16(%ebp), %eax
	movl	-4(%eax), %eax
	movl	%eax, -12(%ebp)
	cmpl	$0, -12(%ebp)
	jns	.L61
	negl	-12(%ebp)
	orl	$16, -8(%ebp)
.L61:
	movl	$-1, -16(%ebp)
	movl	12(%ebp), %eax
	movzbl	(%eax), %eax
	cmpb	$46, %al
	jne	.L62
	movl	12(%ebp), %eax
	addl	$1, %eax
	movl	%eax, 12(%ebp)
	movl	12(%ebp), %eax
	movzbl	(%eax), %eax
	cmpb	$47, %al
	jle	.L63
	movl	12(%ebp), %eax
	movzbl	(%eax), %eax
	cmpb	$57, %al
	jg	.L63
	leal	12(%ebp), %eax
	movl	%eax, (%esp)
	call	skip_atoi
	movl	%eax, -16(%ebp)
	jmp	.L64
.L63:
	movl	12(%ebp), %eax
	movzbl	(%eax), %eax
	cmpb	$42, %al
	jne	.L64
	addl	$4, 16(%ebp)
	movl	16(%ebp), %eax
	movl	-4(%eax), %eax
	movl	%eax, -16(%ebp)
.L64:
	cmpl	$0, -16(%ebp)
	jns	.L62
	movl	$0, -16(%ebp)
.L62:
	movl	$-1, -20(%ebp)
	movl	12(%ebp), %eax
	movzbl	(%eax), %eax
	cmpb	$104, %al
	je	.L65
	movl	12(%ebp), %eax
	movzbl	(%eax), %eax
	cmpb	$108, %al
	je	.L65
	movl	12(%ebp), %eax
	movzbl	(%eax), %eax
	cmpb	$76, %al
	jne	.L66
.L65:
	movl	12(%ebp), %eax
	movzbl	(%eax), %eax
	movsbl	%al, %eax
	movl	%eax, -20(%ebp)
	movl	12(%ebp), %eax
	addl	$1, %eax
	movl	%eax, 12(%ebp)
.L66:
	movl	12(%ebp), %eax
	movzbl	(%eax), %eax
	movsbl	%al, %eax
	subl	$88, %eax
	cmpl	$32, %eax
	ja	.L67
	movl	.L69(,%eax,4), %eax
	jmp	*%eax
	.section	.rodata
	.align 4
	.align 4
.L69:
	.long	.L68
	.long	.L67
	.long	.L67
	.long	.L67
	.long	.L67
	.long	.L67
	.long	.L67
	.long	.L67
	.long	.L67
	.long	.L67
	.long	.L67
	.long	.L67
	.long	.L70
	.long	.L67
	.long	.L67
	.long	.L67
	.long	.L67
	.long	.L70
	.long	.L67
	.long	.L67
	.long	.L67
	.long	.L67
	.long	.L67
	.long	.L67
	.long	.L67
	.long	.L67
	.long	.L67
	.long	.L67
	.long	.L67
	.long	.L71
	.long	.L67
	.long	.L67
	.long	.L72
	.text
.L72:
	orl	$64, -8(%ebp)
.L68:
	addl	$4, 16(%ebp)
	movl	16(%ebp), %eax
	subl	$4, %eax
	movl	(%eax), %eax
	movl	-8(%ebp), %edx
	movl	%edx, 20(%esp)
	movl	-16(%ebp), %edx
	movl	%edx, 16(%esp)
	movl	-12(%ebp), %edx
	movl	%edx, 12(%esp)
	movl	$16, 8(%esp)
	movl	%eax, 4(%esp)
	movl	-4(%ebp), %eax
	movl	%eax, (%esp)
	call	number
	movl	%eax, -4(%ebp)
	jmp	.L51
.L70:
	orl	$2, -8(%ebp)
.L71:
	addl	$4, 16(%ebp)
	movl	16(%ebp), %eax
	subl	$4, %eax
	movl	(%eax), %eax
	movl	-8(%ebp), %edx
	movl	%edx, 20(%esp)
	movl	-16(%ebp), %edx
	movl	%edx, 16(%esp)
	movl	-12(%ebp), %edx
	movl	%edx, 12(%esp)
	movl	$10, 8(%esp)
	movl	%eax, 4(%esp)
	movl	-4(%ebp), %eax
	movl	%eax, (%esp)
	call	number
	movl	%eax, -4(%ebp)
	jmp	.L51
.L67:
	movl	12(%ebp), %eax
	movzbl	(%eax), %eax
	cmpb	$37, %al
	je	.L73
	movl	-4(%ebp), %eax
	leal	1(%eax), %edx
	movl	%edx, -4(%ebp)
	movb	$37, (%eax)
.L73:
	movl	12(%ebp), %eax
	movzbl	(%eax), %eax
	testb	%al, %al
	je	.L74
	movl	-4(%ebp), %eax
	leal	1(%eax), %edx
	movl	%edx, -4(%ebp)
	movl	12(%ebp), %edx
	movzbl	(%edx), %edx
	movb	%dl, (%eax)
	jmp	.L78
.L74:
	movl	12(%ebp), %eax
	subl	$1, %eax
	movl	%eax, 12(%ebp)
.L78:
	nop
.L51:
	movl	12(%ebp), %eax
	addl	$1, %eax
	movl	%eax, 12(%ebp)
.L49:
	movl	12(%ebp), %eax
	movzbl	(%eax), %eax
	testb	%al, %al
	jne	.L76
	movl	-4(%ebp), %eax
	movb	$0, (%eax)
	movl	-4(%ebp), %edx
	movl	8(%ebp), %eax
	subl	%eax, %edx
	movl	%edx, %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE3:
	.size	vsprintf, .-vsprintf
	.globl	sprintf
	.type	sprintf, @function
sprintf:
.LFB4:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$28, %esp
	leal	12(%ebp), %eax
	addl	$4, %eax
	movl	%eax, -4(%ebp)
	movl	12(%ebp), %eax
	movl	-4(%ebp), %edx
	movl	%edx, 8(%esp)
	movl	%eax, 4(%esp)
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	vsprintf
	movl	%eax, -8(%ebp)
	movl	-8(%ebp), %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE4:
	.size	sprintf, .-sprintf
	.ident	"GCC: (Ubuntu 4.8.4-2ubuntu1~14.04.3) 4.8.4"
	.section	.note.GNU-stack,"",@progbits
