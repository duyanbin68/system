/*#�жϴ����������
comment !
���ļ������ϵͳ�жϴ�������������8259A��16���ж�
���ߣ� Jason_Du
�޸����ڣ�2013-10-18
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
#�жϴ���
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

#int4���
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

#int5Խ��
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
#int6��Ч�Ĳ�����
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
#int7�豸�����ã�����ѧЭ��������
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
#int8 ˫�ش���
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
#int9 ����
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

#int10 ��ЧTSS
handle_inval_tss:
        push %eax
        push %ds
        mov $0x10,%eax
        mov %ax,%ds
	call HandleInvalTss
        pop %ds
        pop %eax
        iret
#int11 �β�����
handle_segment_not_present:
        push %eax
        push %ds
        mov $0x10,%eax
        mov %ax,%ds
	call HandleSegmentNotPresent
        pop %ds
        pop %eax
        iret
#int12 ��ջ�δ���
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
#int13 ���汣������
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
#int14 ҳ����
handle_page_fault:
        pusha
        push %eax
        push %ds
        push %es
        mov $0x10,%eax
        mov %ax,%ds
        mov %ax,%es
        movl 12(%esp),%eax ##ȡҳ������
        push %eax
        mov %cr2,%ebx      # #ȡҳ�����ַ
        push %ebx
	call HandlePageFault
        add $8,%esp
        pop %es
        pop %ds
        pop %eax
        popa
        add $4,%esp # #���������ջ
        iret
#int15 ����
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
#x87Э������
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
#ϵͳ���ã��ȴ��û�ջ���Ʋ������ں�ջ
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
	#Ӳ���жϴ���
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

   mov eax,10h #// ��ds��es �μĴ�����Ϊ�ں����ݶΡ�
   mov ds,ax

   xor eax,eax #
   in al,60h #// ��ȡɨ����->al��
   cmp al,0e0h #// ��ɨ������0xe0 �����������ת������e0 ��־���봦��
   je set_e0
   cmp al,0e1h #// ɨ������0xe1 �����������ת������e1 ��־���봦��
   je set_e1

   push eax
   call _send_kbmsg_to_kernel
   add esp,4
	#call key_table[eax*4] #// ���ü��������ker_table + eax * 4���μ�����502 �У���
   mov e0,0 #// ��λe0 ��־��
#// ������δ���(55-65 ��)�����ʹ��8255A ��PC ��׼���̵�·����Ӳ����λ�����˿�0x61 ��
#// 8255A �����B �ĵ�ַ��������˿ڵĵ�7 λ(PB7)���ڽ�ֹ������Լ������ݵĴ���
#// ��γ������ڶ��յ���ɨ��������Ӧ�𡣷��������Ƚ�ֹ���̣�Ȼ����������������̹�����

e0_e1:
    in al,61h #/#/ ȡPPI �˿�B ״̬����λ7 ��������/��ֹ(0/1)���̡�
    jmp l1 #// �ӳ�һ�ᡣ
l1: jmp l2
l2: or al,80h #// al λ7 ��λ(��ֹ���̹���)��
    jmp l3 #// ���ӳ�һ�ᡣ
l3: jmp l4
l4: out 61h,al #// ʹPPI PB7 λ��λ��
    jmp l5 #// �ӳ�һ�ᡣ
l5: jmp l6
l6: and al,7Fh #// al λ7 ��λ��

    out 61h,al #// ʹPPI PB7 λ��λ��������̹�������
    mov al,20h #// ��8259 �ж�оƬ����EOI(�жϽ���)�źš�
    out 20h,al
    out 0a0h,al

    pop ds
    pop edx
    popad
    iret
set_e0:
	mov dword ptr e0,1 #// �յ�ɨ��ǰ����0xe0 ʱ������e0 ��־��λ0����
	jmp e0_e1
set_e1:
	mov dword ptr e0,2 #// �յ�ɨ��ǰ����0xe1 ʱ������e1 ��־��λ1����
	jmp e0_e1
e0 dd 0
*/
keyboard_handle:
	pusha
	mov $01,%eax
	in $0x60,%al
	popa 	
iret
#//����жϴ���
#//����һ���жϻ��Ӧһ��0FAh,��Ҫ����,�һ���֪����������������
mouse_handle:
   	pusha
   	push %edx
   	push %ds

   	mov $0x10,%edx
   	mov %edx,%ds
	xor %eax,%eax
	in $0x60,%al
#���溯���Ǽ���������x,y��ƫ����
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
	#ȡ���Ҽ�����״̬2011/1/7����8��10��
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
   #�����ƺ���,�Ҳ����û����
   	pushl y_position
   	pushl x_position
   	call pointlimit
   	add $8,%esp
   #�����Ϣ����,C����ʵ��
   	push right_button
  	push left_button
   	push y_position
   	push x_position
   	call send_mousemsg_to_kernel
   	add $16,%esp
cf:
	#�ж���ɣ�send EOI
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
