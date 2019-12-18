#include "../gui/gdi.h"
#include "../kernel/thread.h"
/*
void draw_engchar(u32 x,u32 y,char * s,u32 color,u32 * start,u32 width){
	return;
};*/
//idt�ж���������ʾ

u32 screen_width = 0;
extern unsigned long int2e_interrupt;
extern unsigned long clock_interrupt;
extern unsigned long keyboard_handle;
extern LPThread current;

//��������
extern u32 handle_divide_error,handle_single_step_exception,handle_nmi;
extern u32 handle_breakpoint_exception,handle_overflow,handle_bounds_check;
extern u32 handle_inval_opcode,handle_copr_not_available,handle_double_fault;
extern u32 handle_copr_seg_overrun,handle_inval_tss,handle_segment_not_present;
extern u32 handle_stack_exception,handle_general_protection;
extern u32 handle_page_fault,handle_copr_error,clock_handle;
extern u32 handle_mf_error,handle_ac_error,handle_mc_error,handle_xm_error;

void HandleDivideError(){
	draw_engchar(400,300,"Divide Error",0xffff,(u32 *)vesa_start,screen_width);
	for(;;);
}
//�����쳣
void HandleSingleStepException(){
	draw_engchar(400,300,"Debug Expt",0xffff,(unsigned int *)vesa_start,screen_width);
	for(;;);
}
//�������ж�
void HandleNmi(){
	draw_engchar(400,300,"NMI INT",0xffff,(unsigned int *)vesa_start,screen_width);
	for(;;);
}
//�ϵ��쳣
void HandleBreakpointException(){
	draw_engchar(400,300,"Breakpoint Exp",0xffff,(unsigned int *)vesa_start,screen_width);
	for(;;);
}
//;int4���
void HandleOverflow(){
	draw_engchar(400,300,"Overflow",0xffff,(unsigned int *)vesa_start,screen_width);
	for(;;);
}
//;int5Խ��
void HandleBoundsCheck(){
	draw_engchar(400,300,"BOUND Range Exceeded",0xffff,(unsigned int *)vesa_start,screen_width);
	for(;;);
}

//;int6��Ч�Ĳ�����
void HandleInvalOpcode(){
	draw_engchar(400,300,"Invalid Opcode",0xffff,(unsigned int *)vesa_start,screen_width);	
	for(;;);
}
//;int7�豸�����ã�����ѧЭ��������
void HandleCoprNotAvailable(){
	draw_engchar(400,300,"Device Not Available",0xffff,(unsigned int *)vesa_start,screen_width);
	for(;;);
}
//;int8 ˫�ش���
void HandleDoubleFault(){
	draw_engchar(400,300,"Double Fault",0xffff,(unsigned int *)vesa_start,screen_width);
	for(;;);
}
//			;int9 xie����
void HandleCoprSegOverrun(){
	draw_engchar(400,300,"Coprocessor",0xffff,(unsigned int *)vesa_start,screen_width);
	for(;;);
}

//			  ;int10 ��ЧTSS

void HandleInvalTss(int dummy1,int dummy2,int error_code,int eip,int cs,int flags,int esp,int ss){
	//char buf[40] = {0};
	//sprintf(buf,"error code is %d",p4 >> 3);
	//p3 = p3 >> 3;
	draw_char(400,200,(char)(ss + 0x30),0xffff,(unsigned int *)vesa_start,screen_width);
	draw_engchar(400,300,"Invalid TSS Exception",0xffff,(unsigned int *)vesa_start,screen_width);
	for(;;);
}
//			  ;int11 �β�����
void HandleSegmentNotPresent(){
	draw_engchar(400,300,"segment is not exist",0xffff,(unsigned int *)vesa_start,screen_width);
	for(;;);
}
//			  ;int12 ��ջ�δ���
void HandleStackException(){
	draw_engchar(400,300,"stack wrong or flow",0xffff,(unsigned int *)vesa_start,screen_width);
	for(;;);
}
//				 ;int13 ���汣������
void HandleGeneralProtection(int p1,int p2,int p3,int p4,int p5,int ds,int eax,int error_code){
/*
    char buf[100]={0};
	sprintf(buf,"error code is %x,eax:%x,ds:%x,%x,%x,%x,%x,%x",error_code,eax,ds,p1,p2,p3,p4,p5);
	draw_engchar(400,300,"generate protect",0,(unsigned int *)vesa_start,screen_width);
	draw_engchar(400,320,buf,0,(unsigned int *)vesa_start,screen_width);
*/
	for(;;);
}
//				;int14 ҳ����
void HandlePageFault(long address,long error){
	unsigned long *table;
	unsigned long dir;
	unsigned long mid;
	/*
	dir =*(unsigned long*)(((address>>20) & 0xffc)  + current->tss.cr3);
	mid = (address>>10) & 0xffc;
	table = (unsigned long*)((dir & 0xfffff000) + mid);
	
	if (error&1)
		un_wp_page(table);
	else
		put_page(get_mem4k(),address,current->tss.cr3);	
*/
	for(;;);
}
//			   ;int15 ����
void HandleCoprError(){
	draw_engchar(400,300,"resever",0,(unsigned int *)vesa_start,screen_width);
	for(;;);
	
}
//int16
void HandleMFError(){
	draw_engchar(400,300,"HandleMFError",0,(unsigned int *)vesa_start,screen_width);
	for(;;);
	
}
//int17
void HandleACError(){
	draw_engchar(400,300,"HandleACError",0,(unsigned int *)vesa_start,screen_width);
	for(;;);
	
}
//int 18
void HandleMCError(){
	draw_engchar(400,300,"HandleMCError",0,(unsigned int *)vesa_start,screen_width);
	for(;;);
	
}
//int 19
void HandleXMError(){
	draw_engchar(400,300,"HandleXMError",0,(unsigned int *)vesa_start,screen_width);
	for(;;);
	
}



void init_interupt(){
	int i;
	
	set_trap((u32)&handle_divide_error,0);

	set_trap((u32)&handle_single_step_exception,1);

	set_trap((u32)&handle_nmi,2);

	set_interrupt((u32)&handle_breakpoint_exception,3);

	set_interrupt((u32)&handle_overflow,4);

	set_interrupt((u32)&handle_bounds_check,5);

	set_interrupt((u32)&handle_inval_opcode,6);

	set_interrupt((u32)&handle_copr_not_available,7);

	set_interrupt((u32)&handle_double_fault,8);

	set_interrupt((u32)&handle_copr_seg_overrun,9);

	set_interrupt((u32)&handle_inval_tss,10);

	set_interrupt((u32)&handle_segment_not_present,11);

	set_interrupt((u32)&handle_stack_exception,12);

	set_interrupt((u32)&handle_general_protection,13);

	set_interrupt((u32)&handle_page_fault,14);

	set_interrupt((u32)&handle_copr_error,15);

	set_interrupt((u32)&handle_mf_error,16);

	set_interrupt((u32)&handle_ac_error,17);

	set_interrupt((u32)&handle_mc_error,18);

	set_interrupt((u32)&handle_xm_error,19);

	set_interrupt((unsigned long)&clock_interrupt,0x30);

	set_trap((unsigned long)&int2e_interrupt,0x2e);


	for(i = 0x31 ;i < 0x80;i++)
		set_interrupt((unsigned long)0x10000 + (i - 20)* 8, i);

 	set_interrupt((u32)&keyboard_handle,0x31);
	_outb_p(0x02, 0x21);
}

/************************************************************************/
/* �����ж���
 ��ڲ�����addr �жϴ����ַ
           n    �жϺ�
 ����ֵ����                                                          */
/************************************************************************/
void set_interrupt(unsigned long addr,int n){
     unsigned short * idt_addr;
//ȡidt ���׵�ַ
	 idt_addr = (unsigned short *)&idt_table;
 //���ڳ�ʼ��ʱ�����ж϶����ó���selectorΪ0x8
 //typeΪ0x8e00�����������������
	*(idt_addr + n * 4 + 3) = (unsigned short)(addr >> 16) & 0xffff;
	*(idt_addr + n * 4 + 2) = (unsigned short)0x8e00;
	*(idt_addr + n * 4 + 1) = (unsigned short)0x08;  
     	*(idt_addr + n * 4) = (unsigned short)addr & 0xffff;  

}
/************************************************************************/
/* ����������                                                           */
/************************************************************************/
void set_trap(unsigned long addr,int n){
	unsigned short * idt_addr;
	//ͬ��ȡidt�׵�ַ��ͬ�ϣ���ͬ���ǣ�Ҫ����typeΪ0xef00
	//ʹ��3����Ӧ�ó�����������ں�
	idt_addr = (unsigned short*)&idt_table;
	
	*(idt_addr + n * 4) = (unsigned short)addr & 0xffff; 
	*(idt_addr + n * 4 + 1) = (unsigned short)0x08;  
	*(idt_addr + n * 4 + 2) = (unsigned short)0xef00;
	*(idt_addr + n * 4 + 3) = (unsigned short)(addr >> 16) & 0xffff;
}

/************************************************************************/
/* ���������� (INTEL�ֲ���д����Ϊȷ����һ����Ч��TSS�����쳣�������ЧTSS�쳣
����ʹ��������)                                                          */
/************************************************************************/
void set_task(unsigned short sel,int n){
	unsigned short * idt_addr;
	//ͬ��ȡidt�׵�ַ��ͬ�ϣ���ͬ���ǣ�Ҫ����typeΪ0x8500
	idt_addr = (unsigned short*)&idt_table;
	*(idt_addr + n * 4 + 1) = sel;  
	*(idt_addr + n * 4 + 2) = (unsigned short)0x8500;
}


