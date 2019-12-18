#ifndef _WINDOWS_H
#define _WINDOWS_H


#include "../process/msg.h"
#include "../process/thread.h"
#include "gdi.h"

#define TRUE  -1
#define FALSE 0

typedef struct task_window{
	KernelMessageQueue winmsgqueue; //窗口的消息队列
	char windowtitle[11];                  //窗口标题
	struct task_window * prev;         //链表指针
	struct task_window * next;
	int bx,by;           //窗口在缓冲区的位置
	int x,y;             //窗口在屏幕中的位置，可以表述为left,top
	int width,height;    //窗口大小
	int right,bottom;    //窗口右下角坐标
	unsigned long buf;   //窗口图形存储区
	unsigned long map;   //窗口位图
	RECT rects;          //窗口刷新区域
	char id;	//窗口id
	int hwnd;	//窗口句柄
}TASKWINDOW,*LPTASKWINDOW;

extern int sys_rect_intersect(RECT* pdrc, const RECT* psrc1, const RECT* psrc2);
extern int sys_writemap(PRECT r,char * bmp,char b);
extern int sys_bitblt(PRECT r,LPTASKWINDOW t);
extern u32 get_point_color(int x,int y,void *buf,int width);
extern int sys_bring_window_to_top(LPTASKWINDOW p);
extern void windows_init();
extern int sys_mouse_refresh(int x,int y,int oldx,int oldy);
extern int sys_windows_refresh(int x,int y,int mx,int my);
extern int sys_mousewritemap(PRECT r,char * bmp,char b);
extern int sys_CreateWindow(PRECT r);
extern int sys_invalidate(u32 hWnd);
extern int InvalidateWindow(HWND hwnd);
extern int sys_ScrollScreen(HWND hWnd);

extern char * windows_map;

#endif
