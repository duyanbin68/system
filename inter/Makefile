CC=gcc

all: interrupt.o interrupt_handle.o

interrupt.o:interrupt.c interrupt.h
	$(CC) -c interrupt.c
interrupt_handle.o:interrupt_handle.s
	$(CC) -c interrupt_handle.s

clean:
	rm *.o
