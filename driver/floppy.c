#include "hd.h"
#include "floppy.h"
#include "../kernel/interrup.h"



static int reset = 0;

#define BLOCK_SIZE 512
#define tmp_floppy_area 0x80000

static unsigned char reply_buffer[7];
static u32 current_dev = 0;



void floppy_init(){
	set_trap((u32) &floppy_handle,0x26);
}

void _inline copy_buffer(void* from, void* to)
{
	_asm{
	    pushf
		mov ecx,512/4
		mov esi,from
		mov edi,to
		cld
		rep movsd
		popf
}
}

void _inline immoutb_p(unsigned char val,unsigned short port)
{_asm{
	    mov al,val
		mov dx,port
		out dx,al
		jmp l1
        l1: jmp l2
l2:
}}

static int
result (void)
{
	int i = 0, counter, status;
	
	if (reset)
		return -1;
	for (counter = 0; counter < 10000; counter++)
	{
		status = inb_p (FD_STATUS) & (STATUS_DIR | STATUS_READY | STATUS_BUSY);
		if (status == STATUS_READY)
			return i;
		if (status == (STATUS_DIR | STATUS_READY | STATUS_BUSY))
		{
			if (i >= 7)
				break;
			reply_buffer[i++] = inb_p (FD_DATA);
		}
	}
	reset = 1;
	//printk ("Getstatus times out\n\r");
	return -1;
}

static void
setup_DMA (u8* buf,u8 command)
{
	long addr = (long) buf;	// 当前请求项缓冲区所处内存中位置（地址）。
	
	_asm cli;
	// 如果缓冲区处于内存1M 以上的地方，则将DMA 缓冲区设在临时缓冲区域(tmp_floppy_area 数组)
	// (因为8237A 芯片只能在1M 地址范围内寻址)。如果是写盘命令，则还需将数据复制到该临时区域。
	if (addr >= 0x100000)
	{
		addr = (long) tmp_floppy_area;
		if (command == FD_WRITE)
			copy_buffer ((void*)buf, (void*)tmp_floppy_area);
	}
	/* mask DMA 2 *//* 屏蔽DMA 通道2 */
	// 单通道屏蔽寄存器端口为0x10。位0-1 指定DMA 通道(0--3)，位2：1 表示屏蔽，0 表示允许请求。
	immoutb_p (4 | 2, 10);
	/* 输出命令字节。我是不知道为什么，但是每个人（minix，*/
	/* sanches 和canton）都输出两次，首先是12 口，然后是11 口 */
	// 下面嵌入汇编代码向DMA 控制器端口12 和11 写方式字（读盘0x46，写盘0x4A）。
	if (command == FD_READ)
		_asm mov al,DMA_READ;
	else
		_asm mov al,DMA_WRITE;
	_asm {
		out 12,al
			jmp l1
l1: jmp l2
l2: out 11,al
	jmp l3
l3: jmp l4
l4:
	}
	//  __asm__ ("outb %%al,$12\n\tjmp 1f\n1:\tjmp 1f\n1:\t"
	//	   "outb %%al,$11\n\tjmp 1f\n1:\tjmp 1f\n1:"::
	//	   "a" ((char) ((command == FD_READ) ? DMA_READ : DMA_WRITE)));
	/* 8 low bits of addr *//* 地址低0-7 位 */
	// 向DMA 通道2 写入基/当前地址寄存器（端口4）。
	immoutb_p ((unsigned char)addr, 4);
	addr >>= 8;
	/* bits 8-15 of addr *//* 地址高8-15 位 */
	immoutb_p ((unsigned char)addr, 4);
	addr >>= 8;
	/* bits 16-19 of addr *//* 地址16-19 位 */
	// DMA 只可以在1M 内存空间内寻址，其高16-19 位地址需放入页面寄存器(端口0x81)。
	immoutb_p ((unsigned char)addr, 0x81);
	/* low 8 bits of count-1 (1024-1=0x3ff) *//* 计数器低8 位(1024-1=0x3ff) */
	// 向DMA 通道2 写入基/当前字节计数器值（端口5）。
	immoutb_p (0xff, 5);
	/* high 8 bits of count-1 *//* 计数器高8 位 */
	// 一次共传输1024 字节（两个扇区）。
	immoutb_p (3, 5);
	/* activate DMA 2 *//* 开启DMA 通道2 的请求 */
	// 复位对DMA 通道2 的屏蔽，开放DMA2 请求DREQ 信号。
	immoutb_p (0 | 2, 10);
	_asm sti;
}

//// 向软盘控制器输出一个字节数据（命令或参数）。
static void
output_byte (char byte)
{
	int counter;
	unsigned char status;
	
	if (reset)
		return;
	// 循环读取主状态控制器FD_STATUS(0x3f4)的状态。如果状态是STATUS_READY 并且STATUS_DIR=0
	// (CPU->FDC)，则向数据端口输出指定字节。
	for (counter = 0; counter < 10000; counter++)
	{
		status = inb_p (FD_STATUS) & (STATUS_READY | STATUS_DIR);
		if (status == STATUS_READY)
		{
			outb ((unsigned short)byte, (u8)FD_DATA);
			return;
		}
	}
	// 如果到循环1 万次结束还不能发送，则置复位标志，并打印出错信息。
	reset = 1;
	//printk ("Unable to send byte to FDC\n\r");
}

void do_floppy(){

	// 如果返回结果字节数不等于7，或者状态字节0、1 或2 中存在出错标志，则若是写保护
	// 就显示出错信息，释放当前驱动器，并结束当前请求项。否则就执行出错计数处理。
	// 然后继续执行软盘请求操作。
	// ( 0xf8 = ST0_INTR | ST0_SE | ST0_ECE | ST0_NR )
	// ( 0xbf = ST1_EOC | ST1_CRC | ST1_OR | ST1_ND | ST1_WP | ST1_MAM，应该是0xb7)
	// ( 0x73 = ST2_CM | ST2_CRC | ST2_WC | ST2_BC | ST2_MAM )
	if (result () != 7 || (ST0 & 0xf8) || (ST1 & 0xbf) || (ST2 & 0x73))
	{
		return;
	}
	// 如果当前请求项的缓冲区位于1M 地址以上，则说明此次软盘读操作的内容还放在临时缓冲区内，
	// 需要复制到请求项的缓冲区中（因为DMA 只能在1M 地址范围寻址）。
/*
	if (command == FD_READ && (unsigned long) (buf) >= 0x100000)
		copy_buffer (tmp_floppy_area, fd_buf);*/

}

void floppy_setup(){
	int i;

	outb((u16)FD_DOR, (u8)0x08); // 关闭软驱，允许DMA和中断。复位FDC
	
	for (i=0 ; i<100 ; i++){
	    _asm nop;
	}	

	outb((u16)FD_DOR, (u8)0xc); // 选择DMA模式，选择软驱A

	outb((u16)FD_DCR, (u8)0); // 500kpbs


	output_byte((char)FD_SPECIFY);
	
	output_byte((char)0xCF); /* 马达步进速度、磁头卸载时间=32ms */
	
    output_byte((char)6); /* Head load time =6ms, DMA */
}


void fd_read(u32 sectNo, u8 *buf)

{
	
    u32 head, track, block, sector, seek_track;
	
	
	
    if (0 == buf)
		
    {	
        //printf("FloppyReadSector para error.\r");
		
        return;
		
    }
	
	
	
    if (sectNo >= (floppy_type.head * floppy_type.track * floppy_type.sect))
		
    {
		
       // printf("FloppyReadSector sectNo error: %x.\r", sectNo);
		
        return;
		
    }
	
	
	
    /* 计算参数 */
	
    sector = sectNo % floppy_type.sect + 1;
	
    block = sectNo / floppy_type.sect;
	
    track = block / floppy_type.head;
	
    head = block % floppy_type.head;
	
    seek_track = track << floppy_type.stretch;
	
    
	
    /* 软盘重新校正 */
	
    output_byte((char)FD_RECALIBRATE);
	
    output_byte((char)current_dev);
	
    
	
    /* 寻找磁道 */
	
    output_byte((char)FD_SEEK);
	
    output_byte((char)current_dev);
	
    output_byte((char)seek_track);
	
    
	
    /* 设置DMA，准备传送数据 */
	
    setup_DMA((u8*)buf,(u8)FD_READ); 
	
	
	
    /* 发送读扇区命令 */
	
    output_byte((char)FD_READ); /* command */
	
    output_byte((char)current_dev); /* driver no. */
	
    output_byte((char)track); /* track no. */
	
    output_byte((char)head); /* head */
	
    output_byte((char)sector); /* start sector */
	
    output_byte((char)2); /* sector size = 512 */
	
    output_byte((char)floppy_type.sect); /* Max sector */
	
    output_byte((char)floppy_type.gap); /* sector gap */
	
    output_byte((char)0xFF); /* sector size (0xff when n!=0 ?) */
	
}