CC=gcc
LD=ld
LDFILE_KN=kn.ld
OBJCOPY=objcopy
TRIM_FLAGS=-R .pdr -R .comment -R .note -S -O binary

all: kernel.bin boot.img copy

kernel.bin: #boot/head.o memory/memory.o init/main.o inter/interrupt.o inter/interrupt_handle.o gui/gdi.o
	cd boot && make
	cd gui && make
	cd init && make
	cd inter && make
	cd mm && make
	cd kernel && make
	cd driver && make
	$(LD) -s boot/head.o mm/memory.o init/main.o init/vsprintf.o inter/interrupt.o inter/interrupt_handle.o \
	kernel/thread.o kernel/sys.o kernel/msg.o gui/gdi.o driver/mouse.o -o kernel.elf -Map kr.map -T$(LDFILE_KN)
	$(OBJCOPY) $(TRIM_FLAGS) kernel.elf $@
boot.img: boot/boot.bin
	@dd if=./boot/boot.bin of=boot.img bs=512 count=1
	@dd if=/dev/zero of=boot.img skip=1 seek=1 bs=512 count=2879

copy: boot.img ./boot/LOADER.BIN
	sudo mkdir -p /tmp/floppy1;\
	sudo mount -o loop boot.img /tmp/floppy1/ -o fat=12;\
	sudo cp ./boot/LOADER.BIN /tmp/floppy1/;
	sudo cp kernel.bin /tmp/floppy1/;
	sudo umount /tmp/floppy1/;
	sudo rm -rf /tmp/floppy1/;

clean:
	@rm -rf *.o *.elf *.bin *.img *.BIN
