CC=gcc

all: thread.o sys.o msg.o

thread.o: thread.c thread.h
	$(CC) -c -fno-stack-protector thread.c
sys.o: sys.c sys.h
	$(CC) -c -fno-stack-protector sys.c
msg.o: msg.c msg.h
	$(CC) -c msg.c

clean:
	rm *o
