
run: bigboot
	qemu-system-x86_64 ./bigboot

bigboot: boot test
	cat boot test test test >bigboot

test: test.S
	nasm test.S

boot: boot.S
	nasm boot.S
