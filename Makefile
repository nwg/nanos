KERNEL_C = kmem.o ll.o process.o schedule.o pages.o memory.o kernel_init.o syscall.o video.o
USER1_OBJ = user1.o video.o
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

.PHONY: all run clean

.SUFFIXES:

all: bigboot

# kernel.o must be the first object passed to the linker
kernel.bin: $(KERNEL_ASM) $(KERNEL_C) custom.lnk
	$(LD) -T custom.lnk -o $@ $(KERNEL_OBJ)
	$(PAD) 32768 $@

kernel.elf: $(KERNEL_ASM) $(KERNEL_C) custom.lnk
	$(LD) -o $@ $(KERNEL_OBJ)
	$(PAD) 32768 $@

user1.bin: $(USER1_OBJ) user.lnk
	$(LD) -T user.lnk -o $@ $(USER1_OBJ)
	$(PAD) 16384 $@

%.bin: $(SRC)/%.asm $(SRC)/common.mac
	nasm -I$(SRC) -o $@ $<

%.o: $(SRC)/%.c $(KERNEL_HEADERS)
	$(CC) -I$(SRC) $(CFLAGS) -c $<

%.o: $(SRC)/%.asm $(SRC)/common.mac
	nasm -I$(SRC) -f elf64 -o $@ $<

run: bigboot
	qemu-system-x86_64 ./bigboot
	#qemu-system-x86_64 -s -S ./bigboot

bigboot: $(FLAT_BINS)
	cat $(FLAT_BINS) >bigboot

clean:
	rm -f $(OBJ) $(FLAT_BINS) $(ELF) $(USER1_OBJ)
