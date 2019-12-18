	.file	"thread.c"
	.comm	task,256,32
	.comm	current,4,4
	.comm	gdt_opcode,6,2
	.comm	idt_opcode,6,2
	.comm	gdt_table,1024,32
	.comm	idt_table,1024,32
	.comm	krn_stack,512,32
	.comm	task0,120,32
	.globl	ThreadIdArray
	.bss
	.align 32
	.type	ThreadIdArray, @object
	.size	ThreadIdArray, 120
ThreadIdArray:
	.zero	120
	.text
	.globl	init_thread
	.type	init_thread, @function
init_thread:
.LFB0:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$16, %esp
	movw	$8192, gdt_opcode
	movl	$gdt_table, %eax
	movl	%eax, gdt_opcode+2
	movw	$2048, idt_opcode
	movl	$idt_table, %eax
	movl	%eax, idt_opcode+2
	movl	$task0, task
	movw	$-1, gdt_table+8
	movw	$0, gdt_table+10
	movw	$-26112, gdt_table+12
	movw	$207, gdt_table+14
	movw	$-1, gdt_table+16
	movw	$0, gdt_table+18
	movw	$-28160, gdt_table+20
	movw	$207, gdt_table+22
	movw	$-1, gdt_table+24
	movw	$0, gdt_table+26
	movw	$-1536, gdt_table+28
	movw	$207, gdt_table+30
	movw	$-1, gdt_table+32
	movw	$0, gdt_table+34
	movw	$-3584, gdt_table+36
	movw	$207, gdt_table+38
	movl	$task0+16, -4(%ebp)
	movl	-4(%ebp), %eax
	sarl	$16, %eax
	andw	$255, %ax
	movw	%ax, -8(%ebp)
	movl	-4(%ebp), %eax
	sarl	$16, %eax
	movb	$0, %al
	movw	%ax, -6(%ebp)
	movw	$104, gdt_table+40
	movl	-4(%ebp), %eax
	movw	%ax, gdt_table+42
	movzwl	-8(%ebp), %eax
	orw	$-5888, %ax
	movw	%ax, gdt_table+44
	movzwl	-6(%ebp), %eax
	orb	$-128, %al
	movw	%ax, gdt_table+46
	movl	$krn_stack, %eax
	addl	$264, %eax
	movl	%eax, task0+20
	movl	$16, task0+24
	movl	$524288, task0+44
	movl	$task0, current
#APP
# 62 "thread.c" 1
	lgdt gdt_opcode
	
# 0 "" 2
# 64 "thread.c" 1
	lidt idt_opcode
	
# 0 "" 2
#NO_APP
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE0:
	.size	init_thread, .-init_thread
	.globl	TSS
	.type	TSS, @function
TSS:
.LFB1:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$16, %esp
	movw	$0, -2(%ebp)
	movl	8(%ebp), %eax
	addl	$5, %eax
	sall	$3, %eax
	movw	%ax, -2(%ebp)
	movzwl	-2(%ebp), %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE1:
	.size	TSS, .-TSS
	.globl	switch_to
	.type	switch_to, @function
switch_to:
.LFB2:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$20, %esp
	movl	8(%ebp), %eax
	movl	%eax, (%esp)
	call	TSS
	movzwl	%ax, %eax
	movl	%eax, -4(%ebp)
	movl	8(%ebp), %eax
	movl	task(,%eax,4), %edx
	movl	current, %eax
	cmpl	%eax, %edx
	je	.L4
	movl	8(%ebp), %eax
	movl	task(,%eax,4), %eax
	movl	%eax, current
#APP
# 85 "thread.c" 1
	ljmp -8(%ebp);
# 0 "" 2
#NO_APP
.L4:
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE2:
	.size	switch_to, .-switch_to
	.globl	thread_schedule
	.type	thread_schedule, @function
thread_schedule:
.LFB3:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$20, %esp
.L16:
	movl	$-1, -8(%ebp)
	movl	$0, -4(%ebp)
	movl	$0, -12(%ebp)
	movl	$task, -16(%ebp)
	jmp	.L8
.L10:
	addl	$4, -16(%ebp)
	movl	-16(%ebp), %eax
	movl	(%eax), %eax
	testl	%eax, %eax
	jne	.L9
	jmp	.L8
.L9:
	movl	-16(%ebp), %eax
	movl	(%eax), %eax
	movl	(%eax), %eax
	cmpl	$1, %eax
	jne	.L8
	movl	-16(%ebp), %eax
	movl	(%eax), %eax
	movl	4(%eax), %eax
	cmpl	-8(%ebp), %eax
	jle	.L8
	movl	-16(%ebp), %eax
	movl	(%eax), %eax
	movl	4(%eax), %eax
	movl	%eax, -8(%ebp)
	movl	-12(%ebp), %eax
	movl	%eax, -4(%ebp)
.L8:
	addl	$1, -12(%ebp)
	cmpl	$63, -12(%ebp)
	jle	.L10
	cmpl	$0, -8(%ebp)
	je	.L11
	jmp	.L17
.L11:
	movl	$task, -16(%ebp)
	jmp	.L13
.L15:
	movl	-16(%ebp), %eax
	movl	(%eax), %eax
	testl	%eax, %eax
	je	.L14
	movl	-16(%ebp), %eax
	movl	(%eax), %eax
	movl	-16(%ebp), %edx
	movl	(%edx), %edx
	movl	8(%edx), %edx
	addl	$15, %edx
	movl	%edx, 4(%eax)
.L14:
	addl	$4, -16(%ebp)
.L13:
	cmpl	$task+252, -16(%ebp)
	jb	.L15
	jmp	.L16
.L17:
	movl	-4(%ebp), %eax
	movl	%eax, (%esp)
	call	switch_to
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE3:
	.size	thread_schedule, .-thread_schedule
	.globl	do_timer
	.type	do_timer, @function
do_timer:
.LFB4:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	call	thread_schedule
	popl	%ebp
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE4:
	.size	do_timer, .-do_timer
	.globl	alloc_threadid
	.type	alloc_threadid, @function
alloc_threadid:
.LFB5:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$16, %esp
	movl	$1, -4(%ebp)
	jmp	.L20
.L23:
	movl	-4(%ebp), %eax
	addl	$ThreadIdArray, %eax
	movzbl	(%eax), %eax
	testb	%al, %al
	jne	.L21
	movl	-4(%ebp), %eax
	addl	$ThreadIdArray, %eax
	movb	$1, (%eax)
	movl	-4(%ebp), %eax
	jmp	.L22
.L21:
	addl	$1, -4(%ebp)
.L20:
	cmpl	$119, -4(%ebp)
	jle	.L23
	movl	$-1, %eax
.L22:
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE5:
	.size	alloc_threadid, .-alloc_threadid
	.globl	set_tss_desc
	.type	set_tss_desc, @function
set_tss_desc:
.LFB6:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$20, %esp
	movl	12(%ebp), %eax
	movb	%al, -20(%ebp)
	movl	8(%ebp), %eax
	movl	task(,%eax,4), %eax
	addl	$16, %eax
	movl	%eax, -12(%ebp)
	movl	-12(%ebp), %eax
	sarl	$16, %eax
	cwtl
	andl	$255, %eax
	movl	%eax, -8(%ebp)
	movl	-12(%ebp), %eax
	sarl	$16, %eax
	movb	$0, %al
	cwtl
	movl	%eax, -4(%ebp)
	movl	8(%ebp), %eax
	addl	$5, %eax
	movw	$104, gdt_table(,%eax,8)
	movl	8(%ebp), %eax
	leal	5(%eax), %edx
	movl	-12(%ebp), %eax
	movw	%ax, gdt_table+2(,%edx,8)
	movl	8(%ebp), %eax
	leal	5(%eax), %edx
	movl	-8(%ebp), %eax
	orw	$-5888, %ax
	movw	%ax, gdt_table+4(,%edx,8)
	movl	8(%ebp), %eax
	leal	5(%eax), %edx
	movl	-4(%ebp), %eax
	orb	$-128, %al
	movw	%ax, gdt_table+6(,%edx,8)
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE6:
	.size	set_tss_desc, .-set_tss_desc
	.globl	set_thread
	.type	set_thread, @function
set_thread:
.LFB7:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$40, %esp
	call	alloc_threadid
	movl	%eax, -16(%ebp)
	cmpl	$-1, -16(%ebp)
	jne	.L26
	movl	$-1, %eax
	jmp	.L27
.L26:
	movl	$12288, (%esp)
	call	alloc_mem
	movl	%eax, -12(%ebp)
	movl	-16(%ebp), %eax
	movl	-12(%ebp), %edx
	movl	%edx, task(,%eax,4)
	cmpl	$0, -12(%ebp)
	jne	.L28
	movl	$-1, %eax
	jmp	.L27
.L28:
	movl	$0, 4(%esp)
	movl	-16(%ebp), %eax
	movl	%eax, (%esp)
	call	set_tss_desc
	movl	-12(%ebp), %eax
	movl	-16(%ebp), %edx
	movl	%edx, 12(%eax)
	movl	-12(%ebp), %eax
	movl	$25, 4(%eax)
	movl	-12(%ebp), %eax
	movl	$10, 8(%eax)
	movl	-12(%ebp), %eax
	movl	$582, 52(%eax)
	movl	-12(%ebp), %eax
	movl	$27, 92(%eax)
	movl	-12(%ebp), %eax
	movl	$35, 100(%eax)
	movl	-12(%ebp), %eax
	movl	$35, 88(%eax)
	movl	-12(%ebp), %eax
	movl	$35, 96(%eax)
	movl	-12(%ebp), %eax
	movl	$4194304, 72(%eax)
	movl	-12(%ebp), %eax
	movl	$16, 24(%eax)
	movl	-12(%ebp), %eax
	leal	12288(%eax), %edx
	movl	-12(%ebp), %eax
	movl	%edx, 20(%eax)
	movl	-12(%ebp), %eax
	movl	$1, (%eax)
	movl	$0, %eax
.L27:
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE7:
	.size	set_thread, .-set_thread
	.globl	CreateThread
	.type	CreateThread, @function
CreateThread:
.LFB8:
	.cfi_startproc
	pushl	%ebp
	.cfi_def_cfa_offset 8
	.cfi_offset 5, -8
	movl	%esp, %ebp
	.cfi_def_cfa_register 5
	subl	$16, %esp
	movl	24(%ebp), %eax
	movl	%eax, -8(%ebp)
	movl	8(%ebp), %eax
	movl	%eax, -4(%ebp)
#APP
# 195 "thread.c" 1
	mov eax,$1
	int $0x2e
# 0 "" 2
#NO_APP
	movl	$0, %eax
	leave
	.cfi_restore 5
	.cfi_def_cfa 4, 4
	ret
	.cfi_endproc
.LFE8:
	.size	CreateThread, .-CreateThread
	.ident	"GCC: (Ubuntu 4.8.4-2ubuntu1~14.04.3) 4.8.4"
	.section	.note.GNU-stack,"",@progbits
