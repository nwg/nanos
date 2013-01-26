
run: bigboot
	qemu-system-x86_64 ./bigboot

bigboot: boot user1 kernel user2
	cat boot kernel user1 user2 >bigboot

gdt: gdt.py
	python gdt.py >gdt

kernel: kernel.S
	nasm kernel.S

user1: user1.S
	nasm user1.S

user2: user2.S
	nasm user2.S

boot: boot.S
	nasm boot.S
