#include "timer.h"
#include "../kernel/interrup.h"
#include "../kernel/process.h"
#include "../mm/memory.h"
#include "../debug/debug.h"
#include "../kernel/msg.h"
#include "../debug/stdio.h"
#include "../gui/gdi.h"

extern u32 clock_handle;
extern int sys_schedule();
lpKnTimer timer_head = 0;

int test_i = 0;

void timer_init(){

    set_interrupt((unsigned long)&clock_handle,0x20);
}

int GetSystemTime(){
	return systemcount;
}

void do_timer(long cpl){
	char str[20]={0};
	int i = 0;
	lpKnTimer p;

	MSG msg;
	//������++
	systemcount++;

	msg.message = WM_SYSTIMER;

	for (p = timer_head;p;p = p->next)
	{
		if (p->flags = TIMER_FLAGS_USING)
		{
			//�����ʱ��
			if(--p->timerout_value == 0){
				//�ָ�ԭֵ
				p->timerout_value = p->timer_back;
				//������Ϣ��WM_TIMER��ָ���Ĵ��ڳ���
				msg.lParam = p->id;
				msg.hwnd = p->hwnd;
				sys_KernelPutMessage(&KnMsgQ,&msg);
                
				wakeup_process(task[1]);
				}
		}
	}
	//���̼���--
	if(--current->counter>0)
		return;
	current->counter = 0;
	//������ں�̬������
	if(!cpl)
		return;
	//����
	sys_schedule();
}

//�������趨ʱ��
void timer_count_settimer(lpKnTimer timer,unsigned int timerout, HWND hwnd){
    timer->hwnd  = hwnd;
	timer->timerout_value = timerout;
	timer->timer_back = timerout;
	timer->flags = TIMER_FLAGS_USING;
}
/************************************************************************/
/* �ں˶�ʱ����
���û�̬��SetTimer���ã�
                                                                     */
/************************************************************************/
int sys_settimer(HWND hWnd , UINT nIDEvent, UINT uElapse, TIMERPROC lpTimerFunc){

	KnTimer *knt = 0;
//����һ����ʱ��
	knt = (lpKnTimer)alloc_mem(sizeof(KnTimer));
	//���뵽����ͷ
	if(knt == 0)
			return 0;
	knt->lptimerdeal = lpTimerFunc;
	if (timer_head ==0)
	{
       timer_head = knt;
	   timer_head->next = 0;
	}else{
		knt->next = timer_head->next;
        timer_head->next = knt;
	}
	//�¼�id
	knt->id = nIDEvent;
	//�¼�״̬
	knt->flags = TIMER_FLAGS_USING;
	//�¼����
	knt->hwnd = hWnd;
	knt->timer_back = knt->timerout_value = uElapse/10;

	return 1;
}

/************************************************************************/
/* ������ʱ                                                                     */
/************************************************************************/

int sys_killtimer(HWND hWnd,UINT uIDEvent){
	//�������в��ң�hwnd�����uIDevent�¼�
	lpKnTimer p,q;
	for (p = timer_head;p;p = p->next)
	{
		if(p->hwnd == hWnd && p->id == uIDEvent){
			if(p == timer_head)
				timer_head = p->next;
			//���������p��ǰһ���ڵ㣬Ȼ��ɾȥp�ڵ㡣
			for (q = timer_head;q;q =q->next)
			{
				if (q->next == p)
				{
					q->next = p->next;
					break;
				}
			}
			return 1;
		}
	}
	
	return 0;
}

void OnTimer(HWND hwnd,UINT uMsg,UINT idEvent,DWORD dwTime){

	char str_time[8] ={0};
	
	if(idEvent == 0){
		sprintf(str_time,"%d",test_i++);
		draw_engchar(test_i*8,0,str_time,0x7f00,(u16 *)vesa_start,screen_width);
	}

}