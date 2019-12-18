CC=gcc

all: mouse.o

mouse.o: mouse.c mouse.h 
	$(CC) -O -fno-stack-protector -c mouse.c
clean:
	rm *.o
