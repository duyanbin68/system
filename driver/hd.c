#include "hd.h"
#include "../kernel/interrup.h"
#include "../debug/debug.h"
//全局缓冲指针
unsigned int * pub_buf;
extern int hard_handle;
unsigned long bootdata_addr;
extern int int_flags;


void hd_init(){
	set_interrupt((u32)&hard_handle,0x2e);

	outb(inb_p(0x21)&0xfb,0x21);
	outb(inb_p(0xA1)&0xbf,0xA1);

}



//写端口
void  outb(unsigned short port, unsigned char data )
{
	_asm{
		mov al,data
			mov dx,port
			out dx,al
	}
}
//读端口
/*
char  inb_p(unsigned short port)
{
	_asm{
		mov dx,port
			in al,dx
	}
}*/
// 读端口port，共读nr 字，从buf 中取数据
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

// 写端口port，共写nr 字，从buf 中取数据。

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
//检测硬盘控制器准备是否就绪 就绪返回1 否则返回0
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
	//等待 HD_DRQ_STATE 标志置位
	draw_engchar(10,30,"hd read interrupt!",0xffff,vesa_start,screen_width);
	while(!((state=inb_p(HD_STATE_PORT))&HD_DRQ_STATE));
	//HD_DRQ_STATE 标志已置位，传送一个扇区数据到buffer
	
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

