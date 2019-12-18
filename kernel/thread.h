#ifndef _PROCESS_H
#define _PROCESS_H
#pragma pack(2)

struct tss_struct
{
	long back_link;		
	long esp0;
	long ss0;		
	long esp1;
	long ss1;			
	long esp2;
	long ss2;		
	long cr3;
	long eip;
	long eflags;
	long eax, ecx, edx, ebx;
	long esp;
	long ebp;
	long esi;
	long edi;
	long es;			
	long cs;			
	long ss;			
	long ds;			
	long fs;			
	long gs;			
	long ldt;			
	long trace_bitmap;		
};
//gdt 描述符结构，8个字节
typedef struct gdt_tag{
	unsigned short a,b,c,d;
}GDT_S;
//指向描述符的段结构
typedef struct gdt_opcode{
	short a;
	unsigned long b;
}GDT_OPS;

typedef struct _tag_Thread{
	long state;	//线程状态 RUNNING ,SLEEPING	
	long counter;   //计数,默认20
	long priority;  //优先级
	long pid;       //线程id

	struct tss_struct tss;

}Thread,*LPThread;

#define TASK_RUNNING       1
#define TASK_SLEEP         2
#define TASK_STOP          3
#define TASK_SUSPENSION    TASK_SLEEP


#define TASK_TOTAL 64
#define FIRST_TASK task[0]
#define LAST_TASK task[TASK_TOTAL-1]
#define GDT_SIZE   128
#define IDT_SIZE   128
#define INIT_STACK_SIZE  512

typedef unsigned int UINT;
typedef unsigned long ULONG;

extern unsigned int * vesa_start;
//定义64个进程首指针
LPThread task[TASK_TOTAL];
LPThread current;
//GDT,IDT描述符
GDT_OPS gdt_opcode;
GDT_OPS idt_opcode;
//描述表
GDT_S gdt_table[GDT_SIZE];
GDT_S idt_table[IDT_SIZE];
//进程0内核栈
char krn_stack[INIT_STACK_SIZE];

Thread task0;
extern int CreateThread(unsigned int dwStackSize,//initialstacksize
	unsigned long lpStartAddress,	//threadfunction
	void * lpParameter,		//threadargument
	unsigned long dwCreationFlags,	//creationoption
	unsigned long lpThreadId	//threadidentifier
);
void init_thread();
#endif
