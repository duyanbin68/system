CC=gcc

all: main.o vsprintf.o

main.o: main.c
	$(CC) -c -fno-stack-protector main.c
vsprintf.o: vsprintf.c
	$(CC) -c -fno-stack-protector vsprintf.c

clean:
	rm *.o
