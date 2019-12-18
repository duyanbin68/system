/*内核文件*/
.code32
.global _start,vesa_start
.extern virtual_to_physical,main,task_switch
_start:
	mov $0x10,%eax
	mov %ax,%ds
	mov %ax,%es
	mov %ax,%ss
	mov $0x90000,%esp

	call set_8259A
	/*设置时钟*/
	mov $0x36,%al
	mov $0x43,%edx
	out %al,%dx
	mov $0x11930,%eax
	mov $0x40,%edx
	out %al,%dx
	mov %ah,%al
	out %al,%dx
	/*设置完成*/
	push $0xbfffffff
   	push $0x300000
	call check_memory
	mov %eax,(0x9000a)	
	add $8,%esp
	/*开启分页*/
	call setup_pages
	/*映射内存
	void virtual_to_physical(uint cr3,uint line_addr,uint size,uint phy_addr,uint tmp)*/
	pushl $0x70000  /*页表存放在0x70000*/
	pushl $0x100000 /*映射的物理地址*/
	pushl $0x1000000  /*映射大小*/
	pushl $0x80000000 /*映射的线性地址*/
	pushl $0x80000 /*页目录基址0x80000*/
	call virtual_to_physical
	add $5 * 4,%esp

	mov %cr3,%eax
	mov %eax,%cr3

	ljmp $0x08,$here
here:
	
	/*显存地址存放在0x9000这个物理地址里*/
	movl $0x90000,%esi
	movl %ds:(%esi),%eax
	movl %eax,(vesa_start)	
	
	pushl $0x200000  /*页表存放在0x200000*/
	pushl %eax /*映射的物理地址*/
	pushl $0x800000  /*映射大小*/
	pushl %eax /*映射的线性地址*/
	pushl $0x80000 /*页目录基址0x80000*/
	call virtual_to_physical
	add $4,%esp
	pop %eax
	add $4 * 3,%esp
	
	mov $1024*768,%ecx
	movl %eax,%esi
draw:
	movl $0xCC6051,%ds:(%esi)  /*显示一个米黄色桌面*/		 
	add $4,%esi	
	loop draw

	call main

	jmp .

setup_pages:
	mov $0x10,%eax
	mov %ax,%es
	mov $0x8,%ecx  	/*8个页表，每个页表映射4M内存，1K个页表映射4G内存*/
	mov $0x80000,%edi /*页目录的首地址*/
	xor %eax,%eax
	mov $0x81007,%eax /*假设页表和页目录是连续的，页目录占了4K，所以页表的首地址是0x81000，
	低12是页表的属性，7表示用户，可用，存在*/
page1:
	stosl	/*eax -> es:edi*/
	add $0x1000,%eax
	loop page1
	/*此时的页目录表是这样的(页表在内存中可以是不连续的，而我们为了简单，连续设置)
	页目录索引0(地址0x80000)：0x81007
	页目录索引1(地址0x80004)：0x82007
	...
	页目录索引7(地址0x8001C)：0x88007
	*/
	/*初始化页表*/
	mov $0x10,%eax
	mov %ax,%es
	mov $0x1000 * 8 / 4,%ecx
	mov $0x81000,%edi
	xor %eax,%eax
	mov $0x7,%eax    /*地址0*/
page2:
	stosl /*eax -> es:edi eax内容存储到edi中*/
	add $0x1000,%eax
	loop page2
	/*设置完成后页表内容
	页表索引（地址0x81000）: 0x0007
	页表索引（地址0x81004）: 0x1007
	...
	*/
	mov $0x81000,%edi
	movl $3,(%edi)
	/*设置页目录*/
	mov $0x80000,%eax
	mov %eax,%cr3
	/*切换到分页模式*/
	mov %cr0,%eax
	or $0x80000000,%eax
	mov %eax,%cr0
	nop
	ret
/*分页机制，线性地址映射到物理地址的过程，我们要访问物理地址0，在页表中查找索引0（值内容是0x81007），
指向了0x81000这个地址（低12位是属性），0x81000这个页表内容指向了地址0*/
/*得到本机内存总数,通过向一个地址写入，再读出判断内存地址是否存在*/
check_memory:
	push 	%ebp
	mov 	%esp,%ebp
	push 	%esi
	push 	%edi
	push 	%edx
	push 	%ebx
	mov	$0xaa55aa55,%esi	#pat0 = 0xaa55aa55;
	mov	$0x55aa55aa,%edi	#pat1 = 0x55aa55aa;
	mov	24(%esp),%eax		#i = start_addr;
mts_loop:
	mov	%eax,%ebx
	add	$0xffc,%ebx		#p = i + 0xffc;
	movl	(%ebx),%edx		#old = *p;
	movl	%esi,(%ebx)		#*p = pat0;
	xorl	$0xffffffff,(%ebx)	#*p ^= 0xffffffff;
	cmpl	(%ebx),%edi		#if (*p != pat1) goto fin;
	jne	mts_fin
	xorl	$0xffffffff,(%ebx) 	# *p ^= 0xffffffff;
	cmpl	(%ebx),%esi		#if (*p != pat0) goto fin;
	jne	mts_fin
	movl	%edx,(%ebx)			# *p = old;
	add	$0x1000,%eax			#i += 0x1000;
	cmpl	28(%esp),%eax			# if (i <= end) goto mts_loop;
	jbe	mts_loop
	jmp     fin
mts_fin:
	movl	%edx,(%ebx)			# *p = old;
fin:
	pop 	%ebx
	pop 	%edx
	pop 	%edi
	pop 	%esi
	leave
	ret
set_8259A:
   /*master 8259A*/
	mov	$0x11,%al
	out	%al,$0x20
   	jmp m1
m1:
   	nop
   	mov $0x30,%al
   	out %al,$0x21
   	jmp m2
m2:
   	nop
   	mov $0x04,%al
   	out %al,$0x21
   	jmp m3
m3:
   	nop
   	mov $01,%al
   	out %al,$0x21
   	jmp slave
   /*slave 8259A*/
slave:
   	nop
   	mov $0x11,%al
   	out %al,$0xa0
   	jmp s1
s1:
   	nop
   /*ICW2 */
   	mov $0x38,%al
   	out %al,$0xa1
   	jmp s2
s2:
  	nop
   	/*;ICW3 */
   	mov $2,%al
   	out %al,$0xa1
   	jmp s3
s3:
   	nop
   	/*;ICW4*/
   	mov $1,%al
   	out %al,$0xa1
   
	mov	$0xff,%al		/* mask off all interrupts for now*/
	out	%al,$0x21		
	nop
	nop
	out	%al,$0xa1		
	ret
vesa_start: .4byte 0
/*程序完*/
	
	
