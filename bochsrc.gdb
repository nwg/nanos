megs: 32
romimage: file=$BXSHARE/BIOS-bochs-latest
vgaromimage: file=$BXSHARE/VGABIOS-lgpl-latest
vga: extension=vbe
ata0-master: type=disk, path=bochs.img, cylinders=2, heads=4, spt=63
boot: c
log: bochs.log
mouse: enabled=0
cpu: ips=15000000
#vga_update_interval: 150000
debug: action=report
info: action=report
error: action=report
panic: action=ask
gdbstub: enabled=1, port=1234, text_base=0, data_base=0, bss_base=0
