#ifndef _MSG_H
#define _MSG_H


#define WM_LBUTTONUP                         1 
#define WM_LBUTTONDOWN                       2
#define WM_RBUTTONDOWN                       3
#define WM_RBUTTONUP                         4
#define WM_MOUSEMOVE                         5
#define WM_LBUTTONDBLCLK                     6
#define WM_KEYDOWN                      0x0100
#define WM_KEYUP                        0x0101
#define WM_WINDOWMAX                    0x0102
#define WM_TIMECHANGE                   0x0103
#define WM_SYSTIMER                     0x0104
//最大消息数量64个，多余的丢弃
#define MAX_MSG_NUMBER 64


typedef int HWND;
typedef unsigned int UINT;
typedef unsigned long ULONG;
typedef unsigned short USHORT;
typedef unsigned char UCHAR;
typedef long LONG;
typedef int BOOL;
typedef unsigned short WORD;
typedef unsigned long DWORD;

typedef struct tagRECT
{
    LONG    left;
    LONG    top;
    LONG    right;
    LONG    bottom;
} RECT, *PRECT;

typedef struct tagPoint
{
    LONG    x;
    LONG    y;
} POINT,* LPPOINT;
//消息结构体
typedef struct _tagMSG{
	HWND hwnd;
	unsigned int message;
	unsigned int wParam;
	unsigned int lParam;
	unsigned int timeticks;
}MSG,*LPMSG;
//内核消息队列
typedef struct _tagKernelMessageQueue
{
	int head;
	int tail;
	MSG krmsgqueue[MAX_MSG_NUMBER];
}KernelMessageQueue;

#define LOWORD(l) ((WORD)(((DWORD)(l)) & 0xffff))
#define HIWORD(l) ((WORD)(((DWORD)(l)>>16)&0xFFFF))

extern KernelMessageQueue KnMsgQ;
extern void msg_init();
extern int get_message();
extern int dealwith_message();
extern int translate_message();
extern int dispatch_message();


extern int sys_GetMessage(KernelMessageQueue *kmq,MSG *km);
extern int sys_PutMessage(KernelMessageQueue *kmq,MSG *km);
extern int KernelPutMessage(KernelMessageQueue *kmq,MSG *km);
extern int KernelGetMessage(KernelMessageQueue *kmq,MSG *km);

extern void InitMessageQueue(KernelMessageQueue *kmq);
extern int dispatch_sys_message(LPMSG message);

#endif
