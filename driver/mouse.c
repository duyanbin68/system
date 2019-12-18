#include "../kernel/msg.h"
#include "../kernel/thread.h"
#include "../gui/gdi.h"
#include "mouse.h"
#include "hd.h"

extern int x_position,y_position;
int int_mx,int_my; //Mouse old point
int oldlbutton = 0,oldrbutton = 0; //鼠标按键
int int_x,int_y; //当前鼠标坐标，全局变量
int oldtime = 0,nowtime = 0;

extern int screen_width,screen_height,vesa;
//extern int mouse_handle;
extern void set_interrupt(unsigned long addr,int n);

#define port_keydat 0x60
#define port_keysta 0x64
#define port_keycmd 0x64
#define keysta_send_notready 0x02
#define keycmd_write_mode 0x60
#define kbc_mode 0x47

#define keycmd_sendto_mouse 0xd4
#define mousecmd_enable 0xf4

void _outb_p(unsigned char value, unsigned short port)
{
	asm("outb %%al,%%dx;"::"a"(value),"d"(port));
}

#define inb_p(port) _inb_p((unsigned short)(port))
char _inb_p(unsigned short port)
{
	unsigned char ret;
	asm("inb %%dx,%%al;":"=a"(ret):"d"(port));
	return ret;
}

void wait_kbc_sendready(){
	for(;;){
		if((_inb_p(port_keysta) & keysta_send_notready) == 0) 
			break;
	}

}

void init_keyboard(){
	//init keyboard control ciruit
	wait_kbc_sendready();
	_outb_p(port_keycmd,keycmd_write_mode);
	wait_kbc_sendready();
	_outb_p(port_keydat,kbc_mode);	
}

void enable_mouse()
{
	wait_kbc_sendready();
	_outb_p(port_keycmd,keycmd_sendto_mouse);
	wait_kbc_sendready();
	_outb_p(port_keydat,mousecmd_enable);	
}
unsigned long GetSystemTime()
{
	return 0;
}
//鼠标箭头图形
short __arrow[]={
	        00,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,
		00,00,-1,-1,-1,-1,-1,-1,-1,-1,-1,
		00,15,00,-1,-1,-1,-1,-1,-1,-1,-1,
		00,15,15,00,-1,-1,-1,-1,-1,-1,-1,
		00,15,15,15,00,-1,-1,-1,-1,-1,-1,
		00,15,15,15,15,00,-1,-1,-1,-1,-1,
		00,15,15,15,15,15,00,-1,-1,-1,-1,
		00,15,15,15,15,15,15,00,-1,-1,-1,
		00,15,15,15,15,15,15,15,00,-1,-1,
		00,15,15,15,15,15,15,15,15,00,-1,
		00,15,15,15,15,00,00,00,00,00,00,
		00,15,15,00,15,15,00,-1,-1,-1,-1,
		00,15,00,00,15,15,00,-1,-1,-1,-1,
		00,00,-1,-1,00,15,15,00,-1,-1,-1,
		00,-1,-1,-1,00,15,15,00,-1,-1,-1,
		-1,-1,-1,-1,-1,00,15,15,00,-1,-1,
		-1,-1,-1,-1,-1,00,15,15,00,-1,-1,
		-1,-1,-1,-1,-1,-1,00,00,00,-1,-1,
		-1,-1,-1,-1,-1,-1,-1,-1,-1,-1,-1
};

//#define outb_p(value,port) _outb_p((unsigned char)(value),(unsigned short)(port))


void init_mouse_command(){
	asm(
	"mov $0xa8,%al;"
	"out %al,$0x64;"
	"mov $0xd4,%al;"
	"out %al,$0x64;"
	"mov $0xf4,%al;"
	"out %al,$0x60;"
	"mov $0x60,%al;"
	"out %al,$0x64;"
	"mov $0x47,%al;"
	"out %al,$0x60;"
	);
}

void mouse_init(){
    //取显示模式的宽高
	//screen_width = *(unsigned short*)(0x90000+4);
	//screen_height = *(unsigned short*)(0x90000+6);
    //初始化鼠标位置
	x_position = int_mx = int_x = screen_width / 2;
	y_position = int_my = int_y = screen_height / 2;
	//初始化鼠标命令
    //init_mouse_command();
	init_keyboard();
	enable_mouse();
    //设置鼠标中断
    set_interrupt((unsigned long)&mouse_handle,0x3c);
    //打开鼠标中断
	//_outb_p(/*_inb_p(0x21)&*/0xfb,0x21);
	//_outb_p(/*_inb_p(0xA1)&*/0xef,0xA1);

}


//放消息函数,鼠标中断函数 
/************************************************************************/
/* 鼠标消息的处理还不够好                                                                     */
/************************************************************************/
void send_mousemsg_to_kernel(int x,int y,int leftbutton,int rightbutton )
{
	MSG message;
	int dtime;
	//保存鼠标全局变量
	int_x = x;
	int_y = y;
//test
	
    //windows 参考xPos = LOWORD(lParam); yPos = HIWORD(lParam);
	if ((int_mx == x) && (int_my == y))
	{	
		//发送右键抬起消息
		if (oldrbutton ==1 && rightbutton == 0)
		{
			message.message = WM_RBUTTONUP;
			message.lParam = x | y << 16 ;
			message.wParam = 0;
		}
		//发送左键抬起消息
		if (oldlbutton ==1 && leftbutton == 0)
		{
			message.message = WM_LBUTTONUP;
			message.lParam = x | y << 16 ;
			message.wParam = 0;

		}
        //左键按下
		if(oldlbutton ==0 && leftbutton == 1)
		{
			message.message = WM_LBUTTONDOWN;
			message.lParam = x | y << 16 ;
			message.wParam = 0;
			
			//sys_KernelPutMessage(&KnMsgQ,&message);
			//wakeup_process(task[1]);

			nowtime = GetSystemTime();
			dtime = nowtime - oldtime;
			
			message.timeticks =  dtime; //消息的时间戳,用来记录2次鼠标消息产生的时差
			
			if (message.timeticks < 50 && message.timeticks > 0)
			{
				message.message = WM_LBUTTONDBLCLK;
				message.lParam = x | y << 16 ;
				message.wParam = 0;
			}
			oldtime = nowtime;
		}
      //右键按下
		else if(oldrbutton==0 && rightbutton == 1)
		{
			message.message = WM_RBUTTONDOWN;
			message.lParam = x | y << 16 ;
			message.wParam = 0;
		}
		
		oldlbutton = leftbutton;
		oldrbutton = rightbutton;
	}
	else
	{
		message.message = WM_MOUSEMOVE;
		message.lParam = x | y << 16 ;
		message.wParam = 0;
		int_mx = x;
		int_my = y;
		
		//sys_KernelPutMessage(&KnMsgQ,&message);
		
	   // sys_wakeup_process(task[1]);

	//	return;
	}
	//发送右键抬起消息
	if (oldrbutton ==1 && rightbutton == 0)
	{
		message.message = WM_RBUTTONUP;
		message.lParam = x | y << 16 ;
		message.wParam = 0;
	}
    //发送左键抬起消息
	if (oldlbutton ==1 && leftbutton == 0)
	{
		message.message = WM_LBUTTONUP;
		message.lParam = x | y << 16 ;
		message.wParam = 0;
	}

    sys_PutMessage(&KnMsgQ,&message);
	//暂时注释	
	//sys_wakeup_process(task[1]);
	
}


//点限制函数
void pointlimit(int x,int y)
{
    if(x<0) x=0;
	if(y<0) y=0;
	if(x >screen_width-1 ) x=screen_width-1;
	if(y >screen_height-1 ) y=screen_height-1;
	
	x_position=x;
	y_position=y;
	
}

int sys_draw_mouse_icon(int x,int y,u16 * color){
	//用数组表示鼠标
	int x1,y1;
	for(y1=y;y1<y+19;y1++)
	{
		for(x1=x;x1<x+11;x1++)
		{
			if (*color == 15)
				draw_window_point(x1,y1,COLOR_FFFFFF,(void *)vesa_start,screen_width);
			else if(*color != 0xffff)
				draw_window_point(x1,y1,(u32)*color,(void *)vesa_start,screen_width);			
			color++;
			
		}
	}

	return 0;
}
void draw_mouse(int x,int y){
	sys_draw_mouse_icon(x,y,__arrow);
}

BOOL GetCursorPos(LPPOINT lpPoint){
	lpPoint->x = int_x;
	lpPoint->y = int_y;
	return 1;
}

void dummy_test(){
	draw_engchar(400,100,"test mouse",0,vesa_start,screen_width);
}
