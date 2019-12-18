#ifndef _TIMER_H
#define _TIMER_H

extern int systemcount;

extern void timer_init();
extern int GetSystemTime();

typedef int HWND;
typedef unsigned int UINT;
typedef unsigned long DWORD;
typedef void (* TIMERPROC)(HWND, UINT, UINT, DWORD);


#define TIMER_MAX 512
#define TIMER_FLAGS_FREE   0
#define TIMER_FLAGS_ALLOC  1
#define TIMER_FLAGS_USING  2

typedef struct _tagTimer{
	//标识
	UINT id;
	//超时计数值
    unsigned int timerout_value,timer_back;
	int flags;
	//计数器的窗口句柄，对于没有窗口的应用程序，仍然需要有一个句柄
	HWND hwnd;
	//函数指针
	TIMERPROC lptimerdeal;
	//指向下一个计数器指针
	struct _tagTimer* next;
}KnTimer,*lpKnTimer;

extern lpKnTimer timer_alloc();
extern void timer_count_free(lpKnTimer timer);
extern void timer_count_init();
extern void timer_count_settimer(lpKnTimer timer,unsigned int timerout,HWND hwnd);
extern int sys_settimer(HWND hWnd , UINT nIDEvent, UINT uElapse, TIMERPROC lpTimerFunc);
extern int sys_killtimer(HWND hWnd,UINT uIDEvent);
extern void OnTimer(HWND, UINT, UINT, DWORD);

#endif

