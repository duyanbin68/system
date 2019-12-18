#include "msg.h"

KernelMessageQueue KnMsgQ;

void msg_init(){

     InitMessageQueue(&KnMsgQ);

}
//GetMessage（LPMSG lpMsg，HWND hWnd，UINT wMsgFilterMin，UINT wMsgFilterMax)
/*
int get_message(LPMSG lpMsg,HWND hWnd,UINT wMsgFilterMin,UINT wMsgFilterMax)
{
    LPTASKWINDOW win;
	int ret = 0;
	char str[256] = {0}; 
    win = (LPTASKWINDOW)hWnd;
	 

	ret = KernelGetMessage(&win->winmsgqueue,lpMsg);

	return ret;
}
*/
int dispatch_sys_message(MSG *message){
/*
	char str[256] = {0}; 
	struct task_struct **p;

	
	for(p = &FIRST_TASK;p < &LAST_TASK; ++p){
		if(&(*p)->twin == tail){
			if(message->message == WM_KEYDOWN)
				{
					debug_printb("keydown happen\n");
					sys_KernelPutMessage(&((*p)->twin.winmsgqueue),message);
					sys_wakeup_process(*p);
					sprintf(str,"(*p)->twin.winmsgqueue is %x\n",(int)&((*p)->twin.winmsgqueue));
					debug_printb(str);
				}
			}
	}
*/
	return 0;
}
int get_thread_message(int threadid,MSG * lpmsg){
	//
	//1.thread = get_thread(threadid);
	//2.检索线程消息 thread.msgqueue
	return 0;
}
/*
int KernelGetMessage(KernelMessageQueue *kmq,MSG *km){  
	int retval =0;
	_asm{
			push ecx
			push edx
            push edi

		    mov edx,dword ptr km
			mov ecx,dword ptr kmq
			mov edi,_SYS_KernelGetMessage
			int 0x80

			pop edi
			pop edx
			pop ecx
			mov dword ptr [retval],eax
	}
	if(!retval)
		schedule();
	return retval;
	
}

int KernelPutMessage(KernelMessageQueue *kmq,MSG *km){
    _asm{
		push ecx
		push edx
        push edi

		mov edx,dword ptr km
		mov ecx,dword ptr kmq
		mov edi,_SYS_KernelPutMessage
		int 0x80

		pop edi
		pop edx
		pop ecx
	}
}
*/
//取消息函数
int sys_GetMessage(KernelMessageQueue *kmq,MSG *km)
{
	
	if(kmq->head==kmq->tail) //队列为空 
	{
		//sys_sleep(-1);
		return 0;
	}
	
	kmq->head++;

	if(kmq->head > MAX_MSG_NUMBER) kmq->head = 0;
	
	*km = kmq->krmsgqueue[kmq->head];//取消息

	return 1;
	
}

int sys_PutMessage(KernelMessageQueue *kmq,MSG *km)
{
	int tail=kmq->tail+1; 
	
	if(tail > MAX_MSG_NUMBER) tail = 0;
	
	if(tail == kmq->head) //队列已满 
	{
		return 0;
	}
	
	kmq->tail = tail;// 
	
	kmq->krmsgqueue[kmq->tail] = *km; // 放入第1个消息 
	
	return 1;
	
}
//初始化队列 
void InitMessageQueue(KernelMessageQueue *kmq)
{
	kmq->head=0;
	kmq->tail=0;
	
}
