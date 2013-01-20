
run: bigboot
	qemu-system-x86_64 ./bigboot

bigboot: boot test gdt
	cat boot gdt test test >bigboot

gdt: gdt.py
	python gdt.py >gdt

test: test.S
	nasm test.S

boot: boot.S
	nasm boot.S
