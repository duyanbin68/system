#include "thread.h"
#include "../inter/interrupt.h"
#include "../mm/memory.h"


//线程ID数组
char ThreadIdArray[120] ={0};

void init_thread()
{
	//初始化gdt，idt
	short low,high;
	int i;

	gdt_opcode.a = 4096 * 2;
	gdt_opcode.b = (unsigned long)gdt_table;

	idt_opcode.a = 256 * 8;
	idt_opcode.b = (unsigned long)idt_table;


	task[0] = &task0;
	//内核代码段 0x08
	gdt_table[1].a = 0xffff;gdt_table[1].b = 0x0;gdt_table[1].c = 0x9a00;gdt_table[1].d = 0xcf;
	//内核数据段 0x10
	gdt_table[2].a = 0xffff;gdt_table[2].b = 0x0;gdt_table[2].c = 0x9200;gdt_table[2].d = 0xcf;
	//用户代码段 0x18
	gdt_table[3].a = 0xffff;gdt_table[3].b = 0x0;gdt_table[3].c = 0xfa00;gdt_table[3].d = 0xcf;
	//用户数据段 0x20
	gdt_table[4].a = 0xffff;gdt_table[4].b = 0x0;gdt_table[4].c = 0xf200;gdt_table[4].d = 0xcf;
	//任务段0 0x28
	i = (int)&task0.tss;
	low = (short)((i >> 16) & 0x00ff);
	high =(short)((i >> 16) & 0xff00);

	gdt_table[5].a = 0x68;
	gdt_table[5].b = i & 0xffff;
	gdt_table[5].c = 0xe900 | low;/*任务段属性，只有0级的程序才能进行任务切换*/
	gdt_table[5].d = 0x0080 | high;

	task0.tss.esp0 = (long)&krn_stack + 264;
	task0.tss.ss0 = 0x10;
	task0.tss.cr3 = 0x80000;

	current = &task0;
	//重新加载gdt，位置已变化
	asm("lgdt %0\n\t" : : "m"(gdt_opcode));

	asm("lidt %0\n\t" : : "m"(idt_opcode));

}

unsigned short TSS(int n)
{
	unsigned short ret = 0;
	ret = 0x28 + n * 8;
	return ret;
}
/*进程切换函数*/
void switch_to(int next){

	struct {long a,b;} _tmp;
	_tmp.b = (long)TSS(next);

	if(task[next] == current){
		return;
	}
	current = task[next];
	//进程切换，长跳转到指定的TSS
	asm("ljmp %0;"
	::"m"(*&_tmp.a),"m"(*&_tmp.b));
}
/*
进程调度函数
*/
void thread_schedule()
{
	LPThread *p;

	int i,c,next;

	while (1) {
		c = -1;
		next = 0;
		i = 0;
		p = &task[0];
		while (++i < TASK_TOTAL) {
			if (!*++p)
				continue;
			if ((*p)->state == TASK_RUNNING && (*p)->counter > c)
				c = (*p)->counter, next = i;
		}
		if (c) break;

		for(p = &FIRST_TASK;p < &LAST_TASK; ++p){
			if (*p)
				(*p)->counter = (*p)->priority + 15;
		}
	}


	switch_to(next);
}

void do_timer()
{
	thread_schedule();
}

int alloc_threadid(){
	int i;
	//如果id未被占用，则返回
	for(i = 1;i < 120;i++){
		if(ThreadIdArray[i] == 0){
			ThreadIdArray[i] = 1;
			return i;
		}
	}
	return -1;
}

void set_tss_desc(int tid,char tp)
{
	int addr,low,high;
	addr = (int)&task[tid]->tss;

	low = (short)((addr >> 16) & 0x00ff);
	high =(short)((addr >> 16) & 0xff00);

	gdt_table[tid + 5].a = 0x68;
	gdt_table[tid + 5].b = addr & 0xffff;
	gdt_table[tid + 5].c = 0xe900 | low;/*任务段属性，只有0级的程序才能进行任务切换*/
	gdt_table[tid + 5].d = 0x0080 | high;
}

int set_thread(UINT dwStackSize,ULONG eip,void * param,ULONG flags,ULONG id)
{
	int tid;
	LPThread p;
	//1.线程描述符分配内存,为线程分配id
	tid = alloc_threadid();
	if(tid == -1)
		return -1;
	task[tid] = p = (LPThread)alloc_mem(0x1000 * 3);

	if(p == 0)
		return -1;
	//2.设置gdt中的tss结构
	set_tss_desc(tid,0);
	//3.初始化各个变量
	p->pid = tid;
	p->counter = 25;
	p->priority = 10;
	p->tss.eflags = 0x246;
	p->tss.cs = 0x1b;
	p->tss.ds = 0x23;
	p->tss.es = 0x23;
	p->tss.ss = 0x23;
	p->tss.esp = 0x50000;
	p->tss.ss0 = 0x10;
	p->tss.esp0 = 0x1000 * 3 + (long) p;
    	p->tss.eip = (long)eip;
	//页目录重新设置,如果是内核进程，共用0x80000
	p->tss.cr3 = 0x80000;
	//4.设置分页映射
    //alloc memory to thread stack,and set
    	//virtual_to_physical(p->tss.cr3,(UINT)0x200000,(UINT)0x2000,(UINT)0x200000,(UINT)alloc_mem4k());
	//允许访问显存
   // virtual_to_physical(p->tss.cr3,(UINT)vesa_start,(UINT)1600*1200*4,(UINT)vesa_start,(UINT)alloc_mem4k());
	//5.设置为可调度
	p->state = TASK_RUNNING; //可调度
	return 0;
}

int CreateThread(unsigned int dwStackSize,//initialstacksize
	unsigned long lpStartAddress,	//threadfunction
	void * lpParameter,		//threadargument
	unsigned long dwCreationFlags,	//creationoption
	unsigned long lpThreadId	//threadidentifier
)
{

	asm("mov %esp,%edx\n\t"
	"add $8,%edx\n\t"
	"mov $0,%ebx\n\t"
	"int $0x2e");

	return 0;
}
