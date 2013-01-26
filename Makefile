OBJECTS = boot.bin kernel.bin user1.bin user2.bin

%.bin: %.S
	nasm -o $@ $<

run: bigboot
	qemu-system-x86_64 ./bigboot

bigboot: $(OBJECTS)
	cat $(OBJECTS) >bigboot

clean:
	rm -f *.bin
