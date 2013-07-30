KERNEL_C = kmem.o ll.o process.o schedule.o pages.o memory.o
KERNEL_HEADERS = kmem.h ll.h process.h schedule.h types.h memory.h asm.h
KERNEL_ASM = kernel.o
KERNEL_OBJ = $(KERNEL_ASM) $(KERNEL_C)
OBJ = $(KERNEL_OBJ)
FLAT_BINS = boot.bin kernel.bin pages.bin user1.bin user2.bin
CC = x86_64-elf-gcc
CFLAGS = -fno-builtin-memchr -Wall -Werror -std=c99
LD = x86_64-elf-ld
PAD = ./util/pad.sh
ELF = kernel.elf

.PHONY: kernel

all: bigboot

# kernel.o must be the first object passed to the linker
kernel.bin: $(KERNEL_ASM) $(KERNEL_C) custom.lnk
	$(LD) -T custom.lnk -o $@ $(KERNEL_OBJ)
	$(PAD) 32768 $@

kernel.elf: $(KERNEL_ASM) $(KERNEL_C) custom.lnk
	$(LD) -o $@ $(KERNEL_OBJ)
	$(PAD) 32768 $@

%.bin: %.asm common.mac
	nasm -o $@ $<

%.o: %.c $(KERNEL_HEADERS)
	$(CC) $(CFLAGS) -c $<

%.o: %.asm common.mac
	nasm -f elf64 -o $@ $<

run: bigboot
	qemu-system-x86_64 ./bigboot
	#qemu-system-x86_64 -s -S ./bigboot

bigboot: $(FLAT_BINS)
	cat $(FLAT_BINS) >bigboot

clean:
	rm -f $(OBJ) $(FLAT_BINS) $(ELF)
