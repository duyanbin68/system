#include "../kernel/thread.h"
#include "../gui/gdi.h"
#include "../kernel/msg.h"
#include "../mm/memory.h"
#include "../driver/mouse.h"

extern void draw_window_bar(int x1,int y1,int x2,int y2,unsigned int color,int width,void * buf);
extern int sprintf(char * buffer,const char *fmt, ...);
extern void my_itoa(int n,char s[]);
extern void draw_mouse(int,int);
#pragma pack(4)

extern unsigned int * vesa_start;

int screen_height,screen_width;
unsigned char color_bit;

void task_test(){

	MSG msg;
	int x,y;
	u32 window_addr;
	NRECT t;	
	t.width = 400;
	t.height = 300;
	draw_engchar(300,100,"test task",0,vesa_start,screen_width);
	window_addr = (u32)alloc_mem(400*300*4);
	draw_window((LPNRECT)&t,(void *)window_addr);
	copy_window_to_screen(100,200,(void*)window_addr,400,300);

	for(;;){
		if(sys_GetMessage(&KnMsgQ,&msg)){
			x = LOWORD(msg.lParam);
			y = HIWORD(msg.lParam);
			draw_mouse(700,100);	
		}
	}
}
void task1()
{
	asm("mov $0x20 | 3, %ax\n mov %ax,%ds\n mov %ax,%es\n");

	int i;
	
	char buf[30]={0};
	my_itoa(123456,(char*)buf);
	//sprintf(buf,"%d",123456);
	
	//draw_window_bar(200,200,400,300,0,screen_width,(void*)vesa_start);

	draw_textbox_color(10,10,400,100,screen_width,(void*)vesa_start,0xff0000);
	draw_engchar(0,100,buf,0,vesa_start,screen_width);
	//asm("pushl $1\n push $2\n push $3\n push $4\n");
	//asm("mov %esp,%edx\n int $0x2e");
	CreateThread(0,(unsigned long)task_test,0,0,0);

	asm("jmp .");
}

void task_switch()
{ 
 	draw_engchar(100,10,"this string from 0x2e",0x800451AB,vesa_start,screen_width);     
}

extern void init_interupt();

void main()
{
	
	
	screen_width = *(unsigned short *)(0x90000 + 4);
	screen_height = *(unsigned short *)(0x90000 + 6);
	color_bit = *(unsigned char *)(0x90000 + 8);
	
	init_interupt();
	init_thread();
	memory_init();
	alloc_mem4k();
	alloc_mem(512);
	msg_init();
	mouse_init();
	//加载tr，切换是保存旧tr
	asm("mov $0x28,%ax \n ltr %ax\n");
	
	draw_engchar(100,200,"This is a test string",0,vesa_start,screen_width);

	asm("mov $0x0,%al\n out %al,$0x21\n out %al,$0xa1\n");
	
	asm("pushfl\n"
	"andl $0xffffbfff, (%esp)\n"
	"popfl\n");

	asm("sti");
	//asm("int $0x3c;");
	asm("pushl $0x20 | 3 \n" 
	"pushl  $0x60000\n "
	"pushfl\n "
	"pushl $0x18 | 3 \n "
	"push $task1\n"
	"iret\n");		

}


