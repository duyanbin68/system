#ifndef _INTERRUPT_H
#define _INTERRUPT_H

void set_interrupt(unsigned long addr,int n);
void set_trap(unsigned long addr,int n);
void set_task(unsigned short sel,int n);

#endif
