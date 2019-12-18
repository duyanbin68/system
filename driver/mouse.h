#ifndef _MOUSE_H
#define _MOUSE_H 

extern int x_position,y_position;
extern short __arrow[];


typedef unsigned long DWORD;
typedef unsigned short WORD;

//#define LOWORD(l)           ((WORD)(l))
//#define HIWORD(l)           ((WORD)(((DWORD)(l) >> 16) & 0xFFFF))

extern int mouse_handle();
extern void mouse_init();
extern void send_mousemsg_to_kernel(int x,int y,int leftbutton,int rightbutton);
extern void pointlimit(int x,int y);
extern unsigned long GetSystemTime();
extern int sys_draw_mouse_icon(int x,int y,u16 * color);

#endif
