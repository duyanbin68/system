#ifndef _SYS_H
#define _SYS_H

//#include "msg.h"

#define _SYS_SETTHREAD       0
#define _SYS_KernelGetMessage   1
#define _SYS_KernelPutMessage   2
#define _SYS_READ               3
#define _SYS_SCHEDULE           4
#define _SYS_READFLOPPY         5
#define _SYS_SLEEP              6
#define _SYS_SETTIMER           7
#define _SYS_KILLTIMER          8
#define _SYS_DEBUG_PRINT        9
#define _SYS_DRAWTASKBAR        10
#define _SYS_DRAWMOUSE          11
#define _SYS_DRAWRECT           12
#define _SYS_KERNELMESSAGE      13
#define _SYS_CREATEWINDOW       14
#define _SYS_SETPRIORITY        15
#define _SYS_HALT		16
#define _SYS_GETDIR		17
#define _SYS_CREATECONSOLE      18
#define _SYS_WAKEUP             19
#define _SYS_INVALIDATE	        20
#define _SYS_DRAWTEXTENG        21
#define _SYS_LOCATECARET        22
#define _SYS_DRAWSTRING		23
#define _SYS_SETCARET		24
#define _SYS_CLEARBUF		25


/*
extern int sys_fork();
extern int sys_read(unsigned char count,unsigned int startsector,unsigned int * buffer,char cmd);
extern int sys_schedule();
extern int sys_sleep(int);
extern int sys_debug_print(char*,int,int);
extern int kernel_getmessage(MSG *msg,LPPOINT p1,LPPOINT p2);
void (*do_hd)();
extern int sys_getdir();
extern int sys_create_console();
int sys_locate_caret(LPPOINT pt,char c,int width);
*/

#endif
