/*设置文件*/
.code16
.equ BaseOfLoader,0x6000
.equ OffsetOfLoader,0
.text
start:	/*ah=0 设置显示模式，al =03 表示文本模式80*25分辨率*/
	mov $3,%ax
	int $0x10 
	
	mov $2,%ah
	xor %dx,%dx
	int $0x10
	/*显示进入设置文件*/
	mov $0x9000,%ax
	mov %ax,%ds
	mov %ax,%es
	mov $0x800,%ax
	mov %ax,%ss
	mov $0x100,%sp
	/*进入本程序中后打印一条信息*/
	mov $boot_msg,%ax
	push %ax
	call show_str
	add $2,%sp
	/*add code for load kernel*/
	
	mov $LoaderFileName,%si
	mov $BaseOfLoader,%bx
	mov $OffsetOfLoader,%bp
	call LoadFile

	/*end load*/
	/*打印换行*/
	call show_enter
	/* 对于图形界面 vesa定义了一系列标准
	ah=0x4f,int 0x10,es:di中返回vesa信息*/
	mov $0x3000,%di
	mov $0x4f00,%ax
	int $0x10
	/*显示the vesa version is*/
	mov $vesa_msg,%ax
	push %ax
	call show_str
	add $2,%sp
	/*取vesa版本号，显示vesa的版本号*/
	movw %es:4(%di),%ax
	shr $0x08,%ax
	add $0x30,%ax
	movw $0x104,%bx
	movb $0x0e,%ah
	int $0x10
	/*打印换行*/
	call show_enter
	
	push %es
	push %di
	push %bx
	push %cx
	movw %es:6(%di),%bx
	movw %es:8(%di),%cx
	mov %bx,%di
	mov %cx,%es
		
	call show_str_far	
	call show_enter
	pop %cx
	pop %bx	
	pop %di
	pop %es
	/*取模式号指针的地址,偏移14处记录了偏移，偏移16记录了段*/
	mov %es:14(%di),%bx
	mov %es:16(%di),%cx
	mov %bx,%di
	mov %cx,%es
repeat_0:
	mov %es:(%di),%ax
	push %ax
	add $2,%di
	addw $1,(enternext)
	cmpw $3,(enternext) /*如果输出了3个字符串，那么就换下一行*/
	jne lt1
	movw $0,(enternext)
lt1:
	mov %ax,%bx
	/*转换成字符串并显示,模式号从0x100~0x1ff*/
	call itoa_m

	push %es
	push %di
	push %ax
	mov $0x9000,%ax
	mov %ax,%ds
	mov %ax,%es
	mov $0x1000,%di
	mov $0x2000,%si
	call get_mode

	/*把屏幕宽度、高度、颜色位数压入堆栈，然后调用itoa显示*/
	xor %ax,%ax
	movb %es:25(%di),%al /*颜色位数*/
	push %ax
	movw %es:20(%di),%ax /*屏幕高度*/
	push %ax
	movw %es:18(%di),%ax /*屏幕宽度*/
	push %ax
	/*整数转换为字符串，字符串地址存放在ds:si中*/
	call itoa_my
	add $2,%sp
	/*显示----*/
	movw $sign_harizon,%ax
	push %ax
	call show_str
	add $2,%sp
	/*取ds:si中字符串，显示*/
	lea (%si),%ax
	push %ax
	call show_str
	add $2,%sp
	/*显示x号*/
	mov $sign_x,%ax
	push %ax
	call show_str
	add $2,%sp
	/*显示屏幕高*/
	pop %ax
	push %ax
	call itoa_my
	add $2,%sp
	
	lea (%si),%ax
	push %ax
	call show_str
	add $2,%sp
	/*显示x号*/
	mov $sign_x,%ax
	push %ax
	call show_str
	add $2,%sp
	/*显示颜色位数*/
	pop %ax
	push %ax
	call itoa_my
	add $2,%sp
	lea (%si),%ax
	push %ax
	call show_str
	add $2,%sp

	cmpw $0,(enternext)
	jne lt2
	call show_enter
	jmp c0
lt2:
	/*显示一个TAB的宽度*/
	mov $sign_tab,%ax
	push %ax
	call show_str
	add $2,%sp
c0:
	pop %ax
	pop %di
	pop %es
	/*如果ax！=-1则继续*/
	cmp $0xffff,%ax
	jne repeat_0
	/*等待键盘输入*/
	mov $0,%ah
	int $0x16
	/*切换到图形模式*/
	movw $0x144,(mode)
	call set_video
	
	mov $0x9000,%ax
	mov %ax,%ds
	mov %ax,%ss
	mov $0x1000,%sp
	/*关中断*/
	cli
	/*打开A20*/
	inb $0x92,%al
	orb $0x02,%al
	outb %al,$0x92
	/*设置8259A*/
	//call set_8259A
	/*加载gdt*/
	lgdt gdt_48
	/*进入保护模式*/
	mov $1,%ax
	lmsw %ax
	/*跳转到32位模式下偏移地址*/
	ljmp $0x18,$protect_mode
.code32
protect_mode:
	mov $0x10,%ax
	mov %ax,%ds
	mov %ax,%es
	/*移动内核到物理地址0x100000(1M)*/
	cld
	mov $0x100000,%edi
	mov $0x60000,%esi	
	mov $0x20000,%ecx /*假设内核大小不超过128K*/
	rep movsb

	//ljmp $0x20,$0x0
	pushw $0x08
	pushl $0x100000	
	lret
/*
	#显存地址存放在0x9000这个物理地址里
	movl $0x9000,%esi
	movl %ds:(%esi),%eax
	mov $480000,%ecx
	movl %eax,%esi
draw:
	movw $0xff00,%ds:(%esi)  #显示一个米黄色桌面 
	add $2,%esi	
	loop draw
	jmp	.
*/
/*程序到此结束，下面是子函数和一些变量*/
gdt:
	.word  0,0,0,0 /*空描述符，保留*/
	/*内核代码段 段基地址0x90000，段限4G*/	
	.word 0xffff
	.word 0x0000
	.word 0x9a00
	.word 0x00cf
	/*内核数据段 段基地址0x90000，段限4G*/
	.word 0xffff
	.word 0x0000
	.word 0x9200
	.word 0x00cf
	/*数据段 段基地址0x0，段限4G*/
	.word 0xffff
	.word 0x0000
	.word 0x9a09
	.word 0x00cf
	/*内核代码段 段基地址0x0，段限4G*/	
	.word 0xffff
	.word 0x0000
	.word 0x9209
	.word 0x00cf
gdt_48: /*指示了gdt的位置和大小*/
	.word 0x800 /*最大0x800*/
	.word gdt   /* 32位基地址，这里拆分成2个字*/
	.word 0x9
.code16
show_str:
	push %bp
	mov %sp,%bp
	push %ds
	push %si
	push %bx
	push %ax
	movw %ss:4(%bp),%si
L1:
	movb %ds:(%si),%al
	cmp $0,%al
	je L2
	mov $0x104,%bx
	movb $0x0e,%ah
	int $0x10
	inc %si
	jmp L1	
L2:
	pop %ax
	pop %bx
	pop %si
	pop %ds
	mov %bp,%sp
	pop %bp
	ret
show_enter:
	mov $ent,%ax
	push %ax
	call show_str
	add $2,%sp
	ret
itoa_m:
	push %ax
	push %bx
	push %cx
	push %dx
	
	mov %ax,%dx
	push %ax
	mov $buffer0x,%ax
	push %ax
	call show_str
	add $2,%sp
	pop %ax
	
	shr $4,%ax
	and $0x0f,%ax
	cmp $0x0a,%ax
	jb t2
	add $7,%ax
t2:
	add $0x30,%ax
	push %ax
	call show_char
	add $2,%sp
	
	mov %dx,%ax
	and $0x0f,%ax
	cmp $0x0a,%ax
	jb t1
	add $7,%ax
t1:
	add $0x30,%ax
	push %ax
	call show_char
	add $2,%sp
	
	pop %dx
	pop %cx
	pop %bx
	pop %ax
	ret
	
show_char:
	push %bp
	mov %sp,%bp
	push %ax
	movw %ss:4(%bp),%ax
	movw $0x104,%bx
	movb $0x0e,%ah
	int $0x10
	pop %ax
	mov %bp,%sp
	pop %bp
	ret	
itoa_my:
	push %bx
	push %cx
	push %dx
	push %si
	
	xor %cx,%cx
	xor %dx,%dx
	mov $10,%bx
next1:	
	div %bx
	push %dx
	inc %cx
	or %ax,%ax
	jz next2
	xor %dx,%dx
	jmp next1
next2:
	pop %dx
	add $0x30,%dl
	movb %dl,(%si)
	inc %si
	loop next2
	movw $0,(%si)
	
	pop %si
	pop %dx
	pop %cx
	pop %bx
	ret
get_mode:
	push %bp
	mov %sp,%bp
	push %ax
	push %cx
	movw $0x4f01,%ax
	movw %ss:4(%bp),%cx
	int $0x10
	pop %cx
	pop %ax
	leave
	ret
show_str_far:
	push %bx
	push %ax
L3:
	movb %es:(%di),%al
	inc %di
	cmp $0,%al
	je L4
	mov $0x104,%bx
	movb $0x0e,%ah
	int $0x10
	jmp L3
L4:
	pop %ax
	pop %bx
	ret
set_video:
	mov $0x200,%bx
	mov %bx,%es
	mov $0x9000,%bx
	mov %bx,%fs
	xor %di,%di
	mov $0x4f01,%ax
	movw (mode),%cx
	int $0x10
	
	movl %es:40(%di),%eax  /*显存映射的线性地址*/
	movl %eax,%fs:(0)
	movw %es:18(%di),%ax /*宽*/
	movw %ax,%fs:(4)
	movw %es:20(%di),%ax /*高*/
	movw %ax,%fs:(6)
	movb %es:25(%di),%al /*颜色位数*/
	movb %al,%fs:(8)
	
	mov $0x4f02,%ax
	movw (mode),%bx
	add $0x4000,%bx
	int $0x10
	ret
/*8259芯片主片端口是0x20-0x21,从片端口是0xa0-0xa1，输出值0x11表示初始化命令开始，
它是ICW1命令字，表示边沿触发、多片8259A级联、最后要发送ICW4命令字*/
set_8259A_1:
	mov $0x11,%al	/*初始化序列*/
	out %al,$0x20	/*发送到8259A主芯片*/
	.word 0xeb,0xeb
	out %al,$0xa0   /*再发送到8259A从芯片*/
	.word 0xeb,0xeb
/*映射外部中断到0x30-0x38-0x40*/
	mov $0x30,%al	/*硬件起始中断号，0x30*/
	out %al,$0x21	/*送主芯片ICW2命令字，设置起始中断号，要送奇端口*/
	.word 0xeb,0xeb 
	mov $0x38,%al	/*从芯片起始中断号，0x38*/
	out %al,$0xa1
	.word 0xeb,0xeb
	mov $0x04,%al	/*送从芯片ICW3命令字*/
	out %al,$0x21	/*主芯片的IR2连从芯片INT*/

	.word 0xeb,0xeb	
	mov $0x2,%al	/*送从芯片ICW3命令字，表示从芯片的INT连接到主芯片的IR2上*/
	out %al,$0xa1
	.word 0xeb,0xeb
	mov $1,%al	
	out %al,$0x21	/*送主芯片命令字，8086模式，普通EOI，非缓冲方式。需发送指令来复位。初始化芯片结束，芯片就绪*/
	.word 0xeb,0xeb 
	out %al,$0xa1	/*送从芯片命令字*/

	.word 0xeb,0xeb
	mov $0xff,%al
	out %al,$0x21 /*屏蔽主芯片的所有请求*/
	nop
	nop
	mov $0xff,%al
	out %al,$0xa1 /*屏蔽从芯片的所有请求*/
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
.include "fat12.s"

mode: .word 0
sign_x: .ascii "x\0"
sign_harizon: .ascii "----\0"
sign_tab: .ascii "        \0"
boot_msg: .ascii "We are entering setup file\0 "
vesa_msg: .ascii "The VESA version is \0"
ent: .byte 13,10,0
buffer0x: .ascii "0x1\0"
enternext: .word 0
LoaderFileName: .ascii "KERNEL  BIN"
BS_OEMName: 		.ascii "MSWIN4.1" /* OEM生成厂商的名字，微软建议使用MSWIN4.1保持兼容行 */
BPB_BytsPerSec:  	.2byte 512	  /*对面现代磁盘来说，这个数可能是4096，但是软盘这个是固定的*/
BPB_SecPerCluse: 	.byte  1	  /*每簇的扇区数，可以取2的n次方*/
BPB_ResvdSecCnt:	.2byte 1	  /*保留扇区数，必须为1*/
BPB_NumFATs:		.byte  2	  /*FAT的数目，一般是2*/
BPB_RootEntCnt:		.2byte 224	  /*根目录可容纳的条目，FAT32此项为0*/
BPB_TotSec16:		.2byte 2880	  /*16bit扇区总数*/
BPB_Media:		.byte  0xf0	  /*媒体介质类型，f0表示可移动磁盘*/
BPB_FATSz16:		.2byte 9	  /*FAT表的大小，9个扇区*/
BPB_SecPerTrk:		.2byte 18	  /*每磁道的扇区数 18 */
BPB_NumHeads:		.2byte 2	  /*磁头数，对于1.44M磁盘来说，这个数固定为2*/
BPB_HiddSec:		.4byte 0	  /*FAT分区之前的隐藏扇区数，没有分区的磁盘来说此项为0*/
BPB_TotSec32:		.4byte 0	  /*总扇区数，和tolsec16含义相同，16位无法表示可用此项*/
BS_DrvNum:		.byte  0	  /*用于0x13中断，0为软盘，0x80为硬盘*/
BS_Reserved1:		.byte  0	  /*保留NT使用*/
BS_BootSig:		.byte  0x29	  /*扩展引导标记，表明此后的3个域可用*/
BS_VolID:		.4byte 0	  /*卷标序列号*/
BS_VolLab:		.ascii "Solrex 0.01" /*卷标*/
BS_FileSysType:		.ascii "FAT12  "	/*文件类型，只是文本字符串，不可用来检测文件系统类型*/

