KERNEL_C = kmem.o ll.o process.o schedule.o pages.o kernel.o \
		   syscall.o user_vga.o stack.o ptr.o stdio.o timer.o intel_8254.o \
		   asm.o intel_8042.o intel_8042_nanos.o keyboard.o interrupt.o \
		   intel_8254_nanos.o term.o ring.o unistd.o intel_8259.o intel_8259_nanos.o \
		   vga.o file.o inbuf.o termbuf.o pci.o pci_nanos.o endian.o ata.o \
		   ata_nanos.o devio.o

USER_OBJ = crt0.o unistd.o stdio.o wait.o user_vga.o nanos_user.o

USER_SH_OBJ = $(USER_OBJ) sh.o
USER1_OBJ = $(USER_OBJ) user1.o
USER2_OBJ = $(USER_OBJ) user2.o

NEWLIB = /opt/local/x86_64-elf/lib/libc.a
KERNEL_LIB = $(NEWLIB)
USER_LIB = $(NEWLIB)

SRC = src/
KERNEL_HEADERS = $(wildcard src/*.h)
KERNEL_ASM = kernel_init.o
KERNEL_OBJ = $(KERNEL_ASM) $(KERNEL_C)
OBJ = $(KERNEL_OBJ)
FLAT_BINS = boot.bin kernel.bin sh.bin user1.bin user2.bin
CC = /opt/local/bin/x86_64-elf-gcc
CFLAGS = -fno-builtin -Wall -Werror -std=c99 -g
LDFLAGS = 
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
USER_TEXT_SIZE = 65536

.PHONY: all run run-qemu clean bochs-gdb bochs-native

.SUFFIXES:

all: bigboot

# kernel_init.o must be the first object passed to the linker
kernel.bin: $(KERNEL_ASM) $(KERNEL_C) kernel.elf custom.lnk
	$(OBJCOPY) -O binary --set-section-flags .bss=alloc,load,contents kernel.elf kernel.bin
	$(PAD) 196608 $@

kernel.elf: $(KERNEL_ASM) $(KERNEL_C) custom.lnk
	$(LD) -o $@ -T custom.lnk --oformat elf64-x86-64 $(LDFLAGS) $(KERNEL_OBJ) $(KERNEL_LIB)

kernel.sym: kernel.elf
	$(OBJCOPY) --only-keep-debug kernel.elf kernel.sym

kernel.ldsym: kernel.elf
	$(MKLDSYM) kernel.elf kernel.ldsym

sh.elf: $(USER_SH_OBJ) user.lnk
	$(LD) -T user.lnk --oformat elf64-x86-64 -o $@ $(LDFLAGS) $(USER_SH_OBJ) $(USER_LIB)

sh.bin: sh.elf
	$(OBJCOPY) -O binary --set-section-flags .bss=alloc,load,contents $< $@
	$(PAD) $(USER_TEXT_SIZE) $@

user1.elf: $(USER1_OBJ) user.lnk
	$(LD) -T user.lnk --oformat elf64-x86-64 -o $@ $(LDFLAGS) $(USER1_OBJ) $(USER_LIB)

user1.bin: user1.elf
	$(OBJCOPY) -O binary --set-section-flags .bss=alloc,load,contents $< $@
	$(PAD) $(USER_TEXT_SIZE) $@
	
user2.elf: $(USER2_OBJ) user.lnk
	$(LD) -T user.lnk --oformat elf64-x86-64 -o $@ $(LDFLAGS) $(USER2_OBJ) $(USER_LIB)

user2.bin: user2.elf
	$(OBJCOPY) -O binary --set-section-flags .bss=alloc,load,contents $< $@
	$(PAD) $(USER_TEXT_SIZE) $@

$(BOCHS_IMG): bigboot
	cp $< $@
	$(PAD) 1290240 $@

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
