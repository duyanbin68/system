#include "sys.h"
#include "thread.h"
#define m_IOBase 0x1f0

int dragflag = 0;
int dragx,dragy;

//extern KB_INPUT secondy;
int int_flags = 0;
int sleep_count;
extern struct Thread *current;
extern unsigned int * pub_buf;

typedef int (* fn_ptr)();
extern int set_thread(long eip);
/*
//系统调用_读硬盘函数
int  sys_read(unsigned char count,unsigned int startsector,unsigned int * buffer,char cmd)
{
        pub_buf = buffer;
  // if(!controller_ready())
	//	return 0;
	if (cmd == HD_CMD_WRITE)
		do_hd = hd_write_intr;
	if (cmd == HD_CMD_READ)
		do_hd = hd_read_intr;
	outb((u16)m_IOBase + 6, 0xE0 | (startsector >> 24) & 0xf); // LBA, master,
	outb((u16)m_IOBase + 2, (unsigned char)count); // read 1 sector
	outb((u16)m_IOBase + 3,  startsector & 0xff); // bit 0 - 7 of the sector id
	outb((u16)m_IOBase + 4, (startsector >> 8) & 0xff); // bit 8 - 15 of the sector id
	outb((u16)m_IOBase + 5, (startsector >> 16) & 0xff); // bit 16 - 23 of the sector id
	outb((u16)m_IOBase + 7, HD_CMD_READ);

	while(inb_p(m_IOBase + 7) < 0);
		
	return 1;

}
*/
/************************************************************************/
/* 进程睡眠
                                                                     */
/************************************************************************/
/*
int sys_sleep(int mseconds){


	if(mseconds == -1)
		current->state = TASK_SLEEP;
	//to do, after mseconds how to wakeup thread
	return 0;
}

BOOL IsinRec(int x1,int y1,int x2,int y2,int x,int y){
	if((x>x1&&x<x2)&&(y>y1&&y<y2))
		return -1;
	return 0;
}
*/
//内核抓取消息,处理消息函数,this is system message queue. it will transfer message to every process or thread.
//Now , I tested keyboard message in the function. Later I will change this proceed.
/*
int kernel_getmessage(MSG *msg,LPPOINT p1,LPPOINT p2){

	struct task_window *p;
    int mx,my;
    RECT r1;
    char buf[100]={0};
//	scan_bus();

	if(KernelGetMessage(&KnMsgQ,msg)){
		switch (msg->message)
		{
		case WM_KEYDOWN:
			//draw_engchar(50,50,secondy.buf,0x0,vesa_start,screen_width);
			if(msg->lParam == 13)
			{
				 //strcpy(str_pub,secondy.buf);
				// memset(secondy.buf,0,64);
			}
			dispatch_sys_message(msg);
			break;
		case WM_SYSTIMER:
		    OnTimer(msg->hwnd,msg->message,msg->lParam,GetSystemTime());
			break;
		case WM_MOUSEMOVE:
			//get current mouse location
			p1->x = LOWORD(msg->lParam);
			p1->y = HIWORD(msg->lParam);
			//if drag occur,calaulate the drag offset
			if(dragflag == 1){
				mx = p1->x - dragx;
				my = p1->y - dragy;
				if(mx == 0 && my == 0)
					break;
				// refresh all windows
				sys_windows_refresh(p1->x,p1->y,mx,my);

				dragx = p1->x;
				dragy = p1->y;
			}
			//refresh the mouse
			sys_mouse_refresh(p1->x,p1->y,p2->x,p2->y);

			p2->x = p1->x;
			p2->y = p1->y;
			break;
		case WM_LBUTTONDOWN:

		    debug_printb("lbuttondwon\n");
			//from down to top,find mouse belong
			for(p=tail;p->previous;p=p->previous)
			{
				if(IsinRec(p->x,p->y,p->right,p->bottom,p1->x,p1->y))
				{
					if(sys_bring_window_to_top(p)){
						r1.left = tail->x;
						r1.top = tail->y;
						r1.right = tail->right;
						r1.bottom = tail->bottom;
						sys_bitblt(&r1,p);
						sys_draw_mouse_icon(p1->x,p1->y,(u16*)__arrow);

					}
					break;
				}

			}
			//当鼠标在标题栏时，表面要拖动窗体
			for(p=tail;p;p=p->previous)
			{
				if(IsinRec(p->x,p->y,p->right-55,p->y+18,p1->x,p1->y))
				{
					dragflag = 1;
					break;
				}
			}
			dragx = p1->x;
			dragy = p1->y;

			break;
		case WM_LBUTTONUP:
			dragflag = 0;
			debug_printb("lbuttonup\n");

			break;

		}

	}
	 // dispatch msg to each process


	return 0;
}
*/
int sys_setpriority(LPThread p,int priority){

	p->priority = priority;
	return 0;
}

int sys_halt(){
	asm("hlt;");
	return 0;
}

fn_ptr sys_call[]= {set_thread};
