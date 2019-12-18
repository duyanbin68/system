#include "hd.h"
#include "../kernel/interrup.h"
#include "../debug/debug.h"
//ȫ�ֻ���ָ��
unsigned int * pub_buf;
extern int hard_handle;
unsigned long bootdata_addr;
extern int int_flags;


void hd_init(){
	set_interrupt((u32)&hard_handle,0x2e);

	outb(inb_p(0x21)&0xfb,0x21);
	outb(inb_p(0xA1)&0xbf,0xA1);

}



//д�˿�
void  outb(unsigned short port, unsigned char data )
{
	_asm{
		mov al,data
			mov dx,port
			out dx,al
	}
}
//���˿�
/*
char  inb_p(unsigned short port)
{
	_asm{
		mov dx,port
			in al,dx
	}
}*/
// ���˿�port������nr �֣���buf ��ȡ����
_inline void port_read(unsigned short port, void* buf,unsigned long nr)
{
	_asm{
			pushf
			mov dx,port
			mov edi,buf
			mov ecx,nr
			cld
			rep insw
			popf
	}
}

// д�˿�port����дnr �֣���buf ��ȡ���ݡ�

_inline void port_write(unsigned short port, void* buf,unsigned long nr)
{
	_asm{
		pushf
			mov dx,port
			mov esi,buf
			mov ecx,nr
			cld
			rep outsw
			popf
	}
}
//���Ӳ�̿�����׼���Ƿ���� ��������1 ���򷵻�0
int controller_ready()
{
	int i;
	for(i=0;i<10000;i++)
	{
		if((inb_p(HD_STATE_PORT)&(HD_READY_STATE|HD_BUSY_STATE))==HD_READY_STATE)
			return 1;
	}
	
	return 0;
}
extern int vesa_start,screen_width;
void hd_read_intr()
{
	
	unsigned char state=0;
	//�ȴ� HD_DRQ_STATE ��־��λ
	draw_engchar(10,30,"hd read interrupt!",0xffff,vesa_start,screen_width);
	while(!((state=inb_p(HD_STATE_PORT))&HD_DRQ_STATE));
	//HD_DRQ_STATE ��־����λ������һ���������ݵ�buffer
	
	port_read(0x1f0,(void *)pub_buf,256);
	
	state=inb_p(HD_STATE_PORT);
	draw_engchar(10,46,"hd read finish!",0xffff,vesa_start,screen_width);
	return;
	
}

void hd_write_intr()
{
	unsigned char state=0;
	state=inb_p(HD_STATE_PORT);
	
	return;
}

