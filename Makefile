
run: bigboot
	qemu-system-x86_64 ./bigboot

bigboot: boot test gdt kernel
	cat boot kernel test >bigboot

gdt: gdt.py
	python gdt.py >gdt

kernel: kernel.S
	nasm kernel.S

test: test.S
	nasm test.S

boot: boot.S
	nasm boot.S
