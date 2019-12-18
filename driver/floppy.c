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
	long addr = (long) buf;	// ��ǰ��������������ڴ���λ�ã���ַ����
	
	_asm cli;
	// ��������������ڴ�1M ���ϵĵط�����DMA ������������ʱ��������(tmp_floppy_area ����)
	// (��Ϊ8237A оƬֻ����1M ��ַ��Χ��Ѱַ)�������д��������轫���ݸ��Ƶ�����ʱ����
	if (addr >= 0x100000)
	{
		addr = (long) tmp_floppy_area;
		if (command == FD_WRITE)
			copy_buffer ((void*)buf, (void*)tmp_floppy_area);
	}
	/* mask DMA 2 *//* ����DMA ͨ��2 */
	// ��ͨ�����μĴ����˿�Ϊ0x10��λ0-1 ָ��DMA ͨ��(0--3)��λ2��1 ��ʾ���Σ�0 ��ʾ��������
	immoutb_p (4 | 2, 10);
	/* ��������ֽڡ����ǲ�֪��Ϊʲô������ÿ���ˣ�minix��*/
	/* sanches ��canton����������Σ�������12 �ڣ�Ȼ����11 �� */
	// ����Ƕ���������DMA �������˿�12 ��11 д��ʽ�֣�����0x46��д��0x4A����
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
	/* 8 low bits of addr *//* ��ַ��0-7 λ */
	// ��DMA ͨ��2 д���/��ǰ��ַ�Ĵ������˿�4����
	immoutb_p ((unsigned char)addr, 4);
	addr >>= 8;
	/* bits 8-15 of addr *//* ��ַ��8-15 λ */
	immoutb_p ((unsigned char)addr, 4);
	addr >>= 8;
	/* bits 16-19 of addr *//* ��ַ16-19 λ */
	// DMA ֻ������1M �ڴ�ռ���Ѱַ�����16-19 λ��ַ�����ҳ��Ĵ���(�˿�0x81)��
	immoutb_p ((unsigned char)addr, 0x81);
	/* low 8 bits of count-1 (1024-1=0x3ff) *//* ��������8 λ(1024-1=0x3ff) */
	// ��DMA ͨ��2 д���/��ǰ�ֽڼ�����ֵ���˿�5����
	immoutb_p (0xff, 5);
	/* high 8 bits of count-1 *//* ��������8 λ */
	// һ�ι�����1024 �ֽڣ�������������
	immoutb_p (3, 5);
	/* activate DMA 2 *//* ����DMA ͨ��2 ������ */
	// ��λ��DMA ͨ��2 �����Σ�����DMA2 ����DREQ �źš�
	immoutb_p (0 | 2, 10);
	_asm sti;
}

//// �����̿��������һ���ֽ����ݣ�������������
static void
output_byte (char byte)
{
	int counter;
	unsigned char status;
	
	if (reset)
		return;
	// ѭ����ȡ��״̬������FD_STATUS(0x3f4)��״̬�����״̬��STATUS_READY ����STATUS_DIR=0
	// (CPU->FDC)���������ݶ˿����ָ���ֽڡ�
	for (counter = 0; counter < 10000; counter++)
	{
		status = inb_p (FD_STATUS) & (STATUS_READY | STATUS_DIR);
		if (status == STATUS_READY)
		{
			outb ((unsigned short)byte, (u8)FD_DATA);
			return;
		}
	}
	// �����ѭ��1 ��ν��������ܷ��ͣ����ø�λ��־������ӡ������Ϣ��
	reset = 1;
	//printk ("Unable to send byte to FDC\n\r");
}

void do_floppy(){

	// ������ؽ���ֽ���������7������״̬�ֽ�0��1 ��2 �д��ڳ����־��������д����
	// ����ʾ������Ϣ���ͷŵ�ǰ����������������ǰ����������ִ�г����������
	// Ȼ�����ִ���������������
	// ( 0xf8 = ST0_INTR | ST0_SE | ST0_ECE | ST0_NR )
	// ( 0xbf = ST1_EOC | ST1_CRC | ST1_OR | ST1_ND | ST1_WP | ST1_MAM��Ӧ����0xb7)
	// ( 0x73 = ST2_CM | ST2_CRC | ST2_WC | ST2_BC | ST2_MAM )
	if (result () != 7 || (ST0 & 0xf8) || (ST1 & 0xbf) || (ST2 & 0x73))
	{
		return;
	}
	// �����ǰ������Ļ�����λ��1M ��ַ���ϣ���˵���˴����̶����������ݻ�������ʱ�������ڣ�
	// ��Ҫ���Ƶ�������Ļ������У���ΪDMA ֻ����1M ��ַ��ΧѰַ����
/*
	if (command == FD_READ && (unsigned long) (buf) >= 0x100000)
		copy_buffer (tmp_floppy_area, fd_buf);*/

}

void floppy_setup(){
	int i;

	outb((u16)FD_DOR, (u8)0x08); // �ر�����������DMA���жϡ���λFDC
	
	for (i=0 ; i<100 ; i++){
	    _asm nop;
	}	

	outb((u16)FD_DOR, (u8)0xc); // ѡ��DMAģʽ��ѡ������A

	outb((u16)FD_DCR, (u8)0); // 500kpbs


	output_byte((char)FD_SPECIFY);
	
	output_byte((char)0xCF); /* ��ﲽ���ٶȡ���ͷж��ʱ��=32ms */
	
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
	
	
	
    /* ������� */
	
    sector = sectNo % floppy_type.sect + 1;
	
    block = sectNo / floppy_type.sect;
	
    track = block / floppy_type.head;
	
    head = block % floppy_type.head;
	
    seek_track = track << floppy_type.stretch;
	
    
	
    /* ��������У�� */
	
    output_byte((char)FD_RECALIBRATE);
	
    output_byte((char)current_dev);
	
    
	
    /* Ѱ�Ҵŵ� */
	
    output_byte((char)FD_SEEK);
	
    output_byte((char)current_dev);
	
    output_byte((char)seek_track);
	
    
	
    /* ����DMA��׼���������� */
	
    setup_DMA((u8*)buf,(u8)FD_READ); 
	
	
	
    /* ���Ͷ��������� */
	
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