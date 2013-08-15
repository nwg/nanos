KERNEL_C = kmem.o ll.o process.o schedule.o pages.o memory.o kernel_init.o \
		   syscall.o video.o stack.o ptr.o string.o stdio.o

USER1_OBJ = user1.o video.o string.o stdio.o memory.o
SRC = src/
KERNEL_HEADERS = $(wildcard src/*.h)
KERNEL_ASM = kernel.o
KERNEL_OBJ = $(KERNEL_ASM) $(KERNEL_C)
OBJ = $(KERNEL_OBJ)
FLAT_BINS = boot.bin kernel.bin pages.bin user1.bin user2.bin
CC = x86_64-elf-gcc
CFLAGS = -fno-builtin -Wall -Werror -std=c99
LD = x86_64-elf-ld
PAD = ./util/pad.sh
ELF = kernel.elf
BOCHS = /usr/local/bin/bochs
BOCHS_IMG = bochs.img
MKLDSYM = ./util/mkldsym
OBJCOPY = x86_64-elf-objcopy

.PHONY: all run run-qemu clean

.SUFFIXES:

all: bigboot

# kernel.o must be the first object passed to the linker
kernel.bin: $(KERNEL_ASM) $(KERNEL_C) kernel.elf custom.lnk
	$(OBJCOPY) -O binary --set-section-flags .bss=alloc,load,contents kernel.elf kernel.bin
	$(PAD) 65536 $@

kernel.elf: $(KERNEL_ASM) $(KERNEL_C) custom.lnk
	$(LD) -o $@ -T custom.lnk --oformat elf64-x86-64 $(KERNEL_OBJ)

kernel.ldsym: kernel.elf
	$(MKLDSYM) kernel.elf kernel.ldsym

user1.bin: $(USER1_OBJ) user.lnk
	$(LD) -T user.lnk -o $@ $(USER1_OBJ)
	$(PAD) 16384 $@

$(BOCHS_IMG): bigboot
	cp $< $@
	$(PAD) 258048 $@

run-bochs: $(BOCHS_IMG) kernel.ldsym
	$(BOCHS) -q

debug: $(BOCHS_IMG) kernel.ldsym
	$(BOCHS) -q -f bochsrc.debug

%.bin: $(SRC)/%.asm $(SRC)/common.mac
	nasm -I$(SRC) -o $@ $<

%.o: $(SRC)/%.c $(KERNEL_HEADERS)
	$(CC) -I$(SRC) $(CFLAGS) -c $<

%.o: $(SRC)/%.asm $(SRC)/common.mac
	nasm -I$(SRC) -f elf64 -o $@ $<

run: bigboot
	qemu-system-x86_64 ./bigboot

bigboot: $(FLAT_BINS)
	cat $(FLAT_BINS) >bigboot

clean:
	rm -f $(OBJ) $(FLAT_BINS) $(ELF) $(USER1_OBJ) $(BOCHS_IMG)
