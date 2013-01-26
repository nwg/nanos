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

; prog1 is 16k total (0xbe00 - 0xfe00)
; user stack grows down from end (0xfdfc)
; kernel stack is last "1K" of kernel @ 0xbdfc
%define PROG1_TEXT 0xbe00
%define PROG1_BASE_STACK_U 0xfdfc
%define PROG1_BASE_STACK_K 0xbdfc

; prog2 is 16k total (0xfe00 - 0x13e00)
; user stack grows down from end (0x13dfc)
; kernel stack is second-from-last "1K" of kernel @ 0xb9fc
%define PROG2_TEXT 0xfe00
%define PROG2_BASE_STACK_U 0x13dfc
%define PROG2_BASE_STACK_K 0xb9fc

[bits 32]
[org TOP]; start of second block of conventional memory
  
  ; Reset gdt
  lgdt [gdt_d]        
  jmp 0x08:cont

cont:

  ; Disable PIC
  ;mov al, 0xff
  ;out 0xa1, al
  ;out 0x21, al

  ; Load interrupt descriptor table
  lidt [idt_hdr]

  ; set up tss (task register)
  mov ax, 0x28 | 3
  ltr ax

  ; remap the PIC
  %define PIC1   0x20
  %define PIC2   0xA0
  %define PIC1_COMMAND PIC1
  %define PIC1_DATA  (PIC1+1)
  %define PIC2_COMMAND PIC2
  %define PIC2_DATA  (PIC2+1)

  %define ICW1_ICW4  0x01 ;    /* ICW4 (not) needed */
  %define ICW1_SINGLE 0x02 ;   /* Single (cascade) mode */
  %define ICW1_INTERVAL4  0x04 ;   /* Call address interval 4 (8) */
  %define ICW1_LEVEL  0x08 ;   /* Level triggered (edge) mode */
  %define ICW1_INIT 0x10 ;   /* Initialization - required! */

  %define ICW4_8086 0x01 ;   /* 8086/88 (MCS-80/85) mode */
  %define ICW4_AUTO 0x02 ;   /* Auto (normal) EOI */
  %define ICW4_BUF_SLAVE ; 0x08    /* Buffered mode/slave */
  %define ICW4_BUF_MASTER ; 0x0C    /* Buffered mode/master */
  %define ICW4_SFNM 0x10 ;   /* Special fully nested (not) */

  %define IOWAIT out 0x80, al

  %macro IOWAIT 0
  push ax
  mov al, 0
  out 0x80, al
  pop ax
  %endmacro

  in al, PIC1_DATA
  push ax
  in al, PIC2_DATA
  push ax

  mov al, ICW1_INIT+ICW1_ICW4
  out PIC1_COMMAND, al
  IOWAIT
  out PIC2_COMMAND, al
  IOWAIT
  mov al, 0x20
  out PIC1_DATA, al
  IOWAIT
  mov al, 0x28
  out PIC1_DATA, al
  IOWAIT
  mov al, 4
  out PIC1_DATA, al
  IOWAIT
  mov al, 2
  out PIC2_DATA, al
  IOWAIT

  mov al, ICW4_8086
  out PIC1_DATA, al
  IOWAIT
  out PIC2_DATA, al
  IOWAIT

  pop ax
  out PIC1_DATA, al
  IOWAIT
  pop ax
  out PIC2_DATA, al
  IOWAIT

  mov esp, PROG2_BASE_STACK_K
  push  dword 0x20 | 3
  ; put stack at end of our user program's 16k (0xbe00 - 0xfe00)
  push  dword PROG2_BASE_STACK_U
  pushfd
  or    dword [esp], 0x200 ; Set IF in EFLAGS (the dw we just pushed to top of stack) so that interrupts will be reenabled in user mode
  push  dword 0x18 | 3
  push  dword PROG2_TEXT
  pusha
  mov dword [prog2_esp], esp

  mov esp, PROG1_BASE_STACK_K
  push  dword 0x20 | 3
  ; put stack at end of our user program's 16k (0xbe00 - 0xfe00)
  push  dword PROG1_BASE_STACK_U
  pushfd
  or    dword [esp], 0x200 ; Set IF in EFLAGS (the dw we just pushed to top of stack) so that interrupts will be reenabled in user mode
  push  dword 0x18 | 3
  push  dword PROG1_TEXT
  mov dword [prog1_esp], esp

  mov dword [tss_sp], PROG1_BASE_STACK_K

  mov dword [curprg], 1

  ; Enter user mode
  mov   ax, 0x20 | 3
  mov   ds, ax
  mov es,ax 
  mov fs,ax 
  mov gs,ax ;we don't need to worry about SS. it's handled by iret

  iret

int_handler:
  mov dword [0xb8010], 0x04690448
  jmp $

pit_handler:
  pusha
  cmp dword [curprg], 0
  jnz schedule
  popa
  jmp pit_done

schedule:
  cmp dword [curprg], 1
  jne prog2

prog1:
  ; Just ran prog1 -- switch back to prog2
  mov [prog1_esp], esp
  mov esp, [prog2_esp]
  popa
  mov dword [tss_sp], PROG2_BASE_STACK_K
  mov dword [curprg], 2
  jmp pit_done

prog2:
  ; Just ran prog2 -- switch back to prog1
  mov [prog2_esp], esp
  mov esp, [prog1_esp]
  popa
  mov dword [tss_sp], PROG1_BASE_STACK_K
  mov dword [curprg], 1
  jmp pit_done

pit_done:

  ; End-of-interrupt
  mov al, 0x20
  out 0x20, al

  iret

curprg: dd 0xddccbbaa
prog1_esp: dd 0
prog2_esp: dd 0

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
tss_sp:
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

dw pit_handler
dw 0x08
db 0
db 11101111b
dw (OFFS(pit_handler)  >> 16) & 0xFFFF

TIMES 0x4000 - ($ - $$) db 0
