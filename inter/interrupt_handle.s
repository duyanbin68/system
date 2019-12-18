/*#中断处理程序汇编码
comment !
此文件存放了系统中断处理函数，不包含8259A的16个中断
作者： Jason_Du
修改日期：2013-10-18
email: duyanbin68@163.com
!
.386p
.model flat
assume gs:nothing
*/
.global handle_divide_error,handle_single_step_exception,handle_nmi
.global handle_breakpoint_exception,handle_overflow,handle_bounds_check
.global handle_inval_opcode,handle_copr_not_available,handle_double_fault
.global handle_copr_seg_overrun,handle_inval_tss,handle_segment_not_present
.global handle_stack_exception,handle_general_protection
.global handle_page_fault,handle_copr_error,int2e_interrupt,clock_interrupt
.global hard_handle,keyboard_handle,mouse_handle,floppy_handle
.global handle_mf_error,handle_ac_error,handle_mc_error,handle_xm_error

.global x_position,y_position

.extern HandleDivideError,HandleSingleStepException,HandleNmi
.extern HandleBreakpointException,HandleOverflow,HandleBoundsCheck
.extern HandleInvalOpcode,HandleCoprNotAvailable,HandleDoubleFault
.extern HandleCoprSegOverrun,HandleInvalTss,HandleSegmentNotPresent
.extern HandleStackException,HandleGeneralProtection,HandlePageFault
.extern HandleCoprError,HandleMFError,HandleACError,HandleMCError,HandleXMError
.extern copy_param_kernel_stack

.extern do_timer/*,_do_hd,_send_kbmsg_to_kernel,_send_mousemsg_to_kernel
.extern _pointlimit,_do_floppy*/
.extern sys_call
#中断处理
.code32
handle_divide_error:
        cli
        push %eax
        push %ds
        mov $0x10,%eax
        mov %ax,%ds
	call HandleDivideError
        pop %ds
        pop %eax
        iret

handle_single_step_exception:
       cli
        push %eax
        push %ds
        mov $0x10,%eax
        mov %ax,%ds
	call HandleSingleStepException
        pop %ds
        pop %eax
        iret

handle_nmi:
        cli
        push %eax
        push %ds
        mov $0x10,%eax
        mov %ax,%ds
	call HandleNmi
        pop %ds
        pop %eax
        iret

handle_breakpoint_exception:
        cli
        push %eax
        push %ds
        mov $0x10,%eax
        mov %ax,%ds
	call HandleBreakpointException
        pop %ds
        pop %eax
        iret

#int4溢出
handle_overflow:
         cli
        push %eax
        push %ds
        mov $0x10,%eax
        mov %ax,%ds
	call HandleOverflow
        pop %ds
        pop %eax
        iret

#int5越界
handle_bounds_check:
        cli
        push %eax
        push %ds
        mov $0x10,%eax
        mov %ax,%ds
	call HandleBoundsCheck
        pop %ds
        pop %eax
        iret
#int6无效的操作码
handle_inval_opcode:
        cli
        push %eax
        push %ds
        mov $0x10,%eax
        mov %ax,%ds
	call HandleInvalOpcode
        pop %ds
        pop %eax
        iret
#int7设备不可用（无数学协处理器）
handle_copr_not_available:
        cli
        push %eax
        push %ds
        mov $0x10,%eax
        mov %ax,%ds
	call HandleCoprNotAvailable
        pop %ds
        pop %eax
        iret
#int8 双重错误
handle_double_fault:
        cli
        push %eax
        push %ds
        mov $0x10,%eax
        mov %ax,%ds
	call HandleDoubleFault
        pop %ds
        pop %eax
        iret
#int9 保留
handle_copr_seg_overrun:
        cli
        push %eax
        push %ds
        mov $0x10,%eax
        mov %ax,%ds
	call HandleCoprSegOverrun
        pop %ds
        pop %eax
        iret

#int10 无效TSS
handle_inval_tss:
        push %eax
        push %ds
        mov $0x10,%eax
        mov %ax,%ds
	call HandleInvalTss
        pop %ds
        pop %eax
        iret
#int11 段不存在
handle_segment_not_present:
        push %eax
        push %ds
        mov $0x10,%eax
        mov %ax,%ds
	call HandleSegmentNotPresent
        pop %ds
        pop %eax
        iret
#int12 堆栈段错误
handle_stack_exception:
	cli
        push %eax
        push %ds
        mov $0x10,%eax
        mov %ax,%ds
	call HandleStackException
        pop %ds
        pop %eax
        iret
#int13 常规保护错误
handle_general_protection:
	cli
        push %eax
        push %ds
        mov $0x10,%eax
        mov %ax,%ds
	call HandleGeneralProtection
        pop %ds
        pop %eax
        iret
#int14 页错误
handle_page_fault:
        pusha
        push %eax
        push %ds
        push %es
        mov $0x10,%eax
        mov %ax,%ds
        mov %ax,%es
        movl 12(%esp),%eax ##取页错误码
        push %eax
        mov %cr2,%ebx      # #取页错误地址
        push %ebx
	call HandlePageFault
        add $8,%esp
        pop %es
        pop %ds
        pop %eax
        popa
        add $4,%esp # #错误码出堆栈
        iret
#int15 保留
handle_copr_error:
        cli
        push %eax
        push %ds
        mov $0x10,%eax
        mov %ax,%ds
	call HandleCoprError
        pop %ds
        pop %eax
        iret
#x87协处理器
handle_mf_error:
        cli
        push %eax
        push %ds
        mov $0x10,%eax
        mov %ax,%ds
        call HandleMFError
        pop %ds
        pop %eax
        iret
 #alignment check
handle_ac_error:
        cli
        push %eax
        push %ds
        mov $0x10,%eax
        mov %ax,%ds
        call HandleACError
        pop %ds
        pop %eax
        iret
#machine check abort
handle_mc_error:
        cli
        push %eax
        push %ds
        mov $0x10,%eax
        mov %ax,%ds
        call HandleMCError
        pop %ds
        pop %eax
        iret
# simd exception
handle_xm_error:
        cli
        push %eax
        push %ds
        mov $0x10,%eax
        mov %ax,%ds
        call HandleXMError
        pop %ds
        pop %eax
        iret
#系统调用，先从用户栈复制参数到内核栈
int2e_interrupt:
        push %ds
	push %es
	push %fs
	push %gs
	push %esi
	push %edi
	push %ebp
        push %eax
        push %ebx
        push %edx
        push %ecx
        mov $0x10,%eax
        mov %ax,%ds
        mov %ax,%es
	/*copy param to kernel stack*/
	sub $72,%esp
	cld
	mov $18,%ecx
	mov %esp,%edi
	mov %edx,%esi
	rep movsl
	call sys_call(,%ebx,4)
	add $72,%esp	/* pop all data */
        pop %ecx
        pop %edx
        pop %ebx
        add $4,%esp        #pop eax
        pop %ebp
        pop %edi
        pop %esi
        pop %gs
        pop %fs
        pop %es
        pop %ds
        iret

clock_interrupt:
  	pusha
  	push %edx
	push %ds

  	movb $0x20,%al
 	out %al,$0x20
   	out %al,$0xa0

	movl $0x10,%edx
	mov %dx,%ds

	movl 44(%esp),%edx
	and $3,%edx 	#%eax is CPL (0 or 3, 0=supervisor)
	push %edx
	call do_timer
	add $4,%esp

	pop %ds
	pop %edx
	popa
	iret
/*
	#硬盘中断处理
_hard_handle:
   pushad

   push edx
   push ds

   mov edx,10h
   mov ds,edx

   mov edx, DWORD ptr [_do_hd]#_hd_read_intr
   call edx

   mov al,20h
   out 20h,al
   out 0a0h,al

   pop ds
   pop eax
   popad
   iret
   #floppy interrupt handle
_floppy_handle:
   pushad

   push edx
   push ds

   mov edx,10h
   mov ds,edx

   mov edx, DWORD ptr [_do_floppy]#_hd_read_intr
   call edx

   mov al,20h
   out 20h,al
   out 0a0h,al

   pop ds
   pop eax
   popad
   iret
_keyboard_handle:
   pushad
   push edx
   push ds

   mov eax,10h #// 将ds、es 段寄存器置为内核数据段。
   mov ds,ax

   xor eax,eax #
   in al,60h #// 读取扫描码->al。
   cmp al,0e0h #// 该扫描码是0xe0 吗？如果是则跳转到设置e0 标志代码处。
   je set_e0
   cmp al,0e1h #// 扫描码是0xe1 吗？如果是则跳转到设置e1 标志代码处。
   je set_e1

   push eax
   call _send_kbmsg_to_kernel
   add esp,4
	#call key_table[eax*4] #// 调用键处理程序ker_table + eax * 4（参见下面502 行）。
   mov e0,0 #// 复位e0 标志。
#// 下面这段代码(55-65 行)是针对使用8255A 的PC 标准键盘电路进行硬件复位处理。端口0x61 是
#// 8255A 输出口B 的地址，该输出端口的第7 位(PB7)用于禁止和允许对键盘数据的处理。
#// 这段程序用于对收到的扫描码做出应答。方法是首先禁止键盘，然后立刻重新允许键盘工作。

e0_e1:
    in al,61h #/#/ 取PPI 端口B 状态，其位7 用于允许/禁止(0/1)键盘。
    jmp l1 #// 延迟一会。
l1: jmp l2
l2: or al,80h #// al 位7 置位(禁止键盘工作)。
    jmp l3 #// 再延迟一会。
l3: jmp l4
l4: out 61h,al #// 使PPI PB7 位置位。
    jmp l5 #// 延迟一会。
l5: jmp l6
l6: and al,7Fh #// al 位7 复位。

    out 61h,al #// 使PPI PB7 位复位（允许键盘工作）。
    mov al,20h #// 向8259 中断芯片发送EOI(中断结束)信号。
    out 20h,al
    out 0a0h,al

    pop ds
    pop edx
    popad
    iret
set_e0:
	mov dword ptr e0,1 #// 收到扫描前导码0xe0 时，设置e0 标志（位0）。
	jmp e0_e1
set_e1:
	mov dword ptr e0,2 #// 收到扫描前导码0xe1 时，设置e1 标志（位1）。
	jmp e0_e1
e0 dd 0
*/
keyboard_handle:
	pusha
	mov $01,%eax
	in $0x60,%al
	popa 	
iret
#//鼠标中断处理
#//鼠标第一次中断会回应一个0FAh,需要跳过,我还不知道其它方法来跳过
mouse_handle:
   	pusha
   	push %edx
   	push %ds

   	mov $0x10,%edx
   	mov %edx,%ds
	xor %eax,%eax
	in $0x60,%al
#下面函数是计算鼠标左边x,y的偏移量
	incl (count)
	cmpl $2,(count)
	je c1
	cmpl $3,(count)
	je c2
	cmpl $4,(count)
	je c3
	jmp cf
c1:
   	mov 	%eax,%edx
   	mov 	%edx,temp
	shr	$4,%edx
	and	$1,%edx
	movl    %edx,(x_sign)
	shr    $5,%eax
	and    $1,%eax
	movl   %eax,(y_sign)
	#取左右键按下状态2011/1/7晚上8点10分
	mov    (temp),%eax
	mov    %eax,%edx
	and    $1,%edx
	movl    %edx,(left_button)
	shr    $1,%eax
	and    $1,%eax
	mov    %eax,(right_button)
   	jmp cf
c2:
	cmp $0,(x_sign)
	jne d1
   	mov (x_position),%ecx
   	add %eax,%ecx
   	mov %ecx,(x_position)
   	jmp cf
d1:
   	neg %eax
   	and $0xff,%eax
   	mov (x_position),%ecx
   
   	sub %eax,%ecx
   	mov %ecx,(x_position)
   	jmp cf
c3:
   	cmp $0,(y_sign)
   	je d2
   	neg %eax
   	and $0xff,%eax
   	movl (y_position),%ebx
   
   	add %eax,%ebx
   	mov %ebx,(y_position)
   	jmp d3
d2:
   	mov (y_position),%ebx
   
   	sub %eax,%ebx
   	mov %ebx,(y_position)
d3:
  	movl $1,(count)
   #点限制函数,我不想用汇编了
   	pushl y_position
   	pushl x_position
   	call pointlimit
   	add $8,%esp
   #鼠标消息函数,C语言实现
   	push right_button
  	push left_button
   	push y_position
   	push x_position
   	call send_mousemsg_to_kernel
   	add $16,%esp
cf:
	#中断完成，send EOI
   	mov $0x20,%al
	out %al,$0x20
	out %al,$0xa0

	call dummy_test

	pop %ds
	pop %edx
   	popa
   	iret
count:		.4byte 0
temp: 		.4byte 0
x_sign: 	.4byte 0
y_sign: 	.4byte 0
x_position: 	.4byte 0
y_position: 	.4byte 0
left_button: 	.4byte 0
right_button: 	.4byte 0

#end
