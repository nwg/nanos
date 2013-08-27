KERNEL_C = kmem.o ll.o process.o schedule.o pages.o memory.o kernel.o \
		   syscall.o user_vga.o stack.o ptr.o string.o stdio.o timer.o intel_8254.o \
		   asm.o intel_8042.o intel_8042_nanos.o keyboard.o interrupt.o \
		   intel_8254_nanos.o term.o ring.o unistd.o intel_8259.o intel_8259_nanos.o

USER1_OBJ = user1.o user_vga.o string.o stdio.o memory.o unistd.o
SRC = src/
KERNEL_HEADERS = $(wildcard src/*.h)
KERNEL_ASM = kernel_init.o
KERNEL_OBJ = $(KERNEL_ASM) $(KERNEL_C)
OBJ = $(KERNEL_OBJ)
FLAT_BINS = boot.bin kernel.bin user1.bin
CC = /opt/local/bin/x86_64-elf-gcc
CFLAGS = -fno-builtin -Wall -Werror -std=c99 -g
LD = /opt/local/bin/x86_64-elf-ld
PAD = ./util/pad.sh
ELF = kernel.elf
BOCHS_GDB = /usr/local/bin/bochs-gdb-stub
BOCHS_NATIVE = /usr/local/bin/bochs-native-debugger
BOCHS_IMG = bochs.img
MKLDSYM = ./util/mkldsym
OBJCOPY = /opt/local/bin/x86_64-elf-objcopy
NASM = nasm
AFLAGS = -w+error

.PHONY: all run run-qemu clean bochs-gdb bochs-native

.SUFFIXES:

all: bigboot

# kernel_init.o must be the first object passed to the linker
kernel.bin: $(KERNEL_ASM) $(KERNEL_C) kernel.elf custom.lnk
	$(OBJCOPY) -O binary --set-section-flags .bss=alloc,load,contents kernel.elf kernel.bin
	$(PAD) 65536 $@

kernel.elf: $(KERNEL_ASM) $(KERNEL_C) custom.lnk
	$(LD) -o $@ -T custom.lnk --oformat elf64-x86-64 $(KERNEL_OBJ)

kernel.sym: kernel.elf
	$(OBJCOPY) --only-keep-debug kernel.elf kernel.sym

kernel.ldsym: kernel.elf
	$(MKLDSYM) kernel.elf kernel.ldsym

user1.bin: $(USER1_OBJ) user.lnk
	$(LD) -T user.lnk -o $@ $(USER1_OBJ)
	$(PAD) 16384 $@

$(BOCHS_IMG): bigboot
	cp $< $@
	$(PAD) 258048 $@

bochs-native: $(BOCHS_IMG) kernel.ldsym
	$(BOCHS_NATIVE) -q

bochs-gdb: $(BOCHS_IMG) kernel.sym
	$(BOCHS_GDB) -q -f bochsrc.gdb

%.bin: $(SRC)/%.asm $(SRC)/common.mac
	$(NASM) $(AFLAGS) -I$(SRC) -o $@ $<

%.o: $(SRC)/%.c $(KERNEL_HEADERS)
	$(CC) -I$(SRC) $(CFLAGS) -c $<

%.o: $(SRC)/%.asm $(SRC)/common.mac
	$(NASM) $(AFLAGS) -g -I$(SRC) -f elf64 -o $@ $<

run: bigboot
	qemu-system-x86_64 ./bigboot

bigboot: $(FLAT_BINS)
	cat $(FLAT_BINS) >bigboot

clean:
	rm -f $(OBJ) $(FLAT_BINS) $(ELF) $(USER1_OBJ) $(BOCHS_IMG)
