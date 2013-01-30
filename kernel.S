%define TOP KERNEL_START
%define OFFS(a) (TOP + a - $$)
%include "common.mac"

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
  ; Master is irq0 to irq7 and starts at 32 (0x20)
  ; Slave is irq8 to irq15 and starts at handler 40 (0x28)
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

  ; Set up stack for second program
  mov esp, PROG2_BASE_STACK_K
  push  dword 0x20 | 3
  ; put stack at end of our second user program's 16k (0xfe00 - 0x13e00)
  push  dword USER_STACK
  pushfd
  or    dword [esp], 0x200 ; Set IF in EFLAGS (the dd we just pushed to top of stack) so that interrupts will be reenabled in user mode
  push  dword 0x18 | 3
  push  dword USER_TEXT
  pusha ; pusha as if the second program has already been scheduled once (for the below shit scheduling algorithm)
  mov dword [prog2_esp], esp

  ; Set up stack for first program
  mov esp, PROG1_BASE_STACK_K
  push  dword 0x20 | 3
  ; put stack at end of our user stack
  push  dword USER_STACK
  pushfd
  or    dword [esp], 0x200 ; Set IF in EFLAGS (the dd we just pushed to top of stack) so that interrupts will be reenabled in user mode
  push  dword 0x18 | 3
  push  dword USER_TEXT
  mov dword [prog1_esp], esp

  mov dword [tss_sp], PROG1_BASE_STACK_K

  mov dword [curprg], 1

  ; enable paging
  mov eax, PROG1_PAGE_DIRECTORY
  mov cr3, eax

  mov eax, cr0
  or eax, 0x80000000
  mov cr0, eax

  ; Enter user mode
  mov   ax, 0x20 | 3
  mov   ds, ax
  mov es,ax 
  mov fs,ax 
  mov gs,ax ;we don't need to worry about SS. it's handled by iret

  iret

cpu0:
cpu1:
cpu2:
cpu3:
cpu4:
cpu5:
cpu6:
cpu7:
cpu8:
cpu9:
cpu10:
cpu11:
cpu12:
cpu13:
cpu14:
cpu15:
cpu16:
cpu17:
cpu18:
cpu19:
cpu20:
cpu21:
cpu22:
cpu23:
cpu24:
cpu25:
cpu26:
cpu27:
cpu28:
cpu29:
cpu30:
cpu31:

  ; disable paging
  mov eax, cr0
  and eax, 0x7fffffff
  mov cr0, eax

  mov dword [0xb8010], 0x04690448
  jmp $

irq0:
  jmp pit_handler

irq1:
irq2:
irq3:
irq4:
irq5:
irq6:
irq7:
irq8:
irq9:
irq10:
irq11:
irq12:
irq13:
irq14:
irq15:
  iret

software_interrupt:
  mov dword [USER_VIDEO + 80*2*14], 0x01690148
  iret

pit_handler:
  pusha
  cmp dword [curprg], 0
  jnz schedule

not_initialized:
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

  mov eax, PROG2_PAGE_DIRECTORY
  mov cr3, eax

  mov dword [tss_sp], PROG2_BASE_STACK_K
  mov dword [curprg], 2
  jmp pit_done

prog2:
  ; Just ran prog2 -- switch back to prog1
  mov [prog2_esp], esp
  mov esp, [prog1_esp]
  popa

  mov eax, PROG1_PAGE_DIRECTORY
  mov cr3, eax

  mov dword [tss_sp], PROG1_BASE_STACK_K
  mov dword [curprg], 1
  jmp pit_done

pit_done:

  ; End-of-interrupt
  mov al, 0x20
  out 0x20, al

  iret

curprg: dd 0
prog1_esp: dd 0
prog2_esp: dd 0

gdt_d:
  dw end_gdt - gdt - 1
  dd gdt

gdt:
  GDT 0, 0, 0, 0
  GDT 0, 0xfffff, 10011010b, 1100b ; ring0 cs (all mem)
  GDT 0, 0xfffff, 10010010b, 1100b ; ring0 ds (all mem)
  GDT 0, 0xfffff, 11111010b, 1100b ; ring3 cs (all mem)
  GDT 0, 0xfffff, 11110010b, 1100b ; ring3 ds (all mem)
  GDT OFFS(tss), (end_tss - tss), 11101001b, 0000b ; pointer to ts, byte granularity
end_gdt:

tss:
  dd 0
tss_sp:
  dd 0xbdfc ; kernel stack address
  dd 0x10 ; kernel stack segment (second gdt member)
  TIMES 15 dd 0
  dd 0x10 ; es
  dd 0x08 ; cs
  dd 0x10 ; ss
  dd 0x10 ; ds
  dd 0x10 ; fs
  dd 0x10 ; gs
  TIMES 8 db 0
end_tss:

idt_hdr:
dw end_idt_data - idt_data
dd idt_data

idt_data:
%assign i 0
%rep 32

dw cpu %+ i
dw 0x08
db 0
db 10001110b
dw (OFFS(cpu %+ i)  >> 16) & 0xFFFF

%assign i i+1
%endrep

; Handle irqs
%assign i 0
%rep    16

dw irq %+ i
dw 0x08
db 0
db 11101110b
dw (OFFS(irq %+ i)  >> 16) & 0xFFFF

%assign i i+1
%endrep

dw software_interrupt
dw 0x08
db 0
db 11101111b
dw (OFFS(software_interrupt)  >> 16) & 0xFFFF

end_idt_data:

TIMES 0x4000 - ($ - $$) db 0
