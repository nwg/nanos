%define TOP 0x7E00
%define OFFS(a) (TOP + a - $$)

; GDT base limit access flags
%macro GDT 4
  dw %2
  dw %1
  db %1 >> 16
  db %3
  db (%2 >> 16) & 0x0F | (%4 << 4)
  db %1 >> 24
%endmacro

[bits 32]
[org TOP]; start of second block of conventional memory
  
  ; Reset gdt
  lgdt [gdt_d]        
  jmp 0x08:cont

cont:

  ; Disable PIC
  mov al, 0xff
  out 0xa1, al
  out 0x21, al

  ; Load interrupt descriptor table
  lidt [idt_hdr]

  sti

  ; set up tss (task register)
  mov ax, 0x28 | 3
  ltr ax

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

gdt_d:
  dw end_gdt - gdt - 1
  dd gdt

gdt:
  GDT 0, 0, 0, 0
  GDT 0, 0xfffff, 10011010b, 1100b
  GDT 0, 0xfffff, 10010010b, 1100b
  GDT 0, 0xfffff, 11111010b, 1100b
  GDT 0, 0xfffff, 11110010b, 1100b
  GDT OFFS(tss), (end_tss - tss), 11101001b, 0000b
end_gdt:

tss:
  dd 0
  dd 0xbdfc ; kernel stack address
  dd 0x10 ; kernel stack segment (second gdt member)
  TIMES 15 dd 0
  dd 0x10
  dd 0x08
  dd 0x10
  dd 0x10
  dd 0x10
  dd 0x10
  TIMES 8 db 0
end_tss:

int_handler:
  mov dword [0xb8010], 0x04690448
  jmp $

softint_handler:
  mov dword [0xb8020], 0x01690148
  ;jmp $
  iret

idt_hdr:
dw 0x108
dd id_data

id_data:
%rep 32
dw int_handler
dw 0x08
db 0
db 10001110b
dw (OFFS(int_handler)  >> 16) & 0xFFFF
%endrep

dw softint_handler
dw 0x08
db 0
db 11101111b
dw (OFFS(softint_handler)  >> 16) & 0xFFFF

TIMES 0x4000 - ($ - $$) db 0 ;Fill the rest of sector with 0
