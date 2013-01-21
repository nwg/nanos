%define TOP 0x7E00

[bits 32]
[org TOP]; start of second block of conventional memory

  ; Disable PIC
  mov al, 0xff
  out 0xa1, al
  out 0x21, al

  ; Load interrupt descriptor table
  lidt [idt_hdr]

  sti

  ; Enter user mode
  cli
  mov   ax, 0x20 | 3
  mov   ds, ax
  mov es,ax 
  mov fs,ax 
  mov gs,ax ;we don't need to worry about SS. it's handled by iret

  push  dword 0x20 | 3
  ; put stack at end of our user program's 16k (0xbe00 - 0xfe00)
  push  dword 0xfdfc
  pushfd
  or    dword [esp], 0x200 ; Set IF in EFLAGS (the dw we just pushed to top of stack) so that interrupts will be reenabled in user mode
  push  dword 0x18 | 3
  push  dword 0xbe00
  iret

  jmp $

int_handler:
  mov dword [0xb8010], 0x04690448
  jmp $
  iret
  ;jmp $

idt_hdr:
dw 0x100
dd id_data

id_data:
%rep 32
dw int_handler
dw 0x08
db 0
db 10001110b
;dw ((TOP + int_handler - $$)  >> 16) & 0xFFFF
dw 0
%endrep

TIMES 0x4000 - ($ - $$) db 0 ;Fill the rest of sector with 0
