CC=gcc
LD=ld
LDFILE=boot.ld
LDFILE_LOAD=loader.ld
OBJCOPY=objcopy
TRIM_FLAGS=-R .pdr -R .comment -R .note -S -O binary

all: boot.bin LOADER.BIN head.o

boot.bin: boot.S
	$(CC) -c boot.S
	$(LD) boot.o -o boot.elf -T$(LDFILE)
	@$(OBJCOPY) $(TRIM_FLAGS) boot.elf $@
LOADER.BIN: loader.S
	$(CC) -c -m32 loader.S
	$(LD) -s loader.o -o loader.elf -T$(LDFILE_LOAD)
	$(OBJCOPY) $(TRIM_FLAGS) loader.elf $@
head.o: head.s
	$(CC) -c head.s

clean:
	rm *.bin *.BIN *.o
