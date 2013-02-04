%define TOP KERNEL_START
%define OFFS(a) (TOP + a - $$)
%include "common.mac"

[BITS 32]
[org TOP]; start of second block of conventional memory

  ; Enable pae
  mov eax, cr4
  or eax, 1 << 5
  mov cr4, eax

  ; Point to long-mode paging root
  mov eax, PROG1_PML4
  mov cr3, eax

  ; Set lm bit in the EFER MSR (0xC0000080)
  mov ecx, 0xC0000080
  rdmsr
  or eax, 1 << 8
  wrmsr

  ; Enable paging
  mov eax, cr0
  or eax, 0x80000000
  mov cr0, eax

  ; Reset gdt
  lgdt [gdt_d]        
  jmp 0x08:cont

[BITS 64]
cont:
  ; Disable PIC
  ;mov al, 0xff
  ;out 0xa1, al
  ;out 0x21, al

  ; Load interrupt descriptor table
  lidt [idt_hdr]

  ; set up tss (task register, 16-byte descriptor in GDT)
  mov ax, 0x28
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
  mov rsp, PROG2_BASE_STACK_K
  push  qword 0x20 | 3
  ; put stack at end of our second user program's 16k (0xfe00 - 0x13e00)
  push  qword USER_STACK
  pushfq
  or    qword [rsp], 0x200 ; Set IF in EFLAGS (the quad we just pushed to top of stack) so that interrupts will be reenabled in user mode
  push  qword 0x18 | 3
  push  dword USER_TEXT
  PUSHA ; pusha as if the second program has already been scheduled once
  mov qword [prog2_rsp], rsp

  ; Set up stack for first program
  mov rsp, PROG1_BASE_STACK_K
  push  qword 0x20 | 3
  ; put stack at end of our user stack
  push  qword USER_STACK
  pushfq
  or    qword [rsp], 0x200 ; Set IF in EFLAGS (the dd we just pushed to top of stack) so that interrupts will be reenabled in user mode
  push  qword 0x18 | 3
  push  qword USER_TEXT
  mov qword [prog1_rsp], rsp

  mov qword [tss64_sp], PROG1_BASE_STACK_K

  mov dword [curprg], 1

  ; Enter user mode
  mov   ax, 0x20 | 3
  mov   ds, ax
  mov es,ax 
  mov fs,ax 
  mov gs,ax ;we don't need to worry about SS. it's handled by iret

  iretq

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
  mov rax, cr0
  and rax, 0x7fffffff
  mov cr0, rax

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
  iretq

software_interrupt:
  mov dword [USER_VIDEO + 80*2*14], 0x01690148
  iretq

pit_handler:
  PUSHA
  cmp dword [curprg], 0
  jnz schedule

not_initialized:
  POPA
  jmp pit_done

schedule:
  cmp dword [curprg], 1
  jne prog2

prog1:
  ; Just ran prog1 -- switch back to prog2
  mov [prog1_rsp], rsp
  mov rsp, [prog2_rsp]
  POPA

  mov rax, PROG2_PML4
  mov cr3, rax

  mov qword [tss64_sp], PROG2_BASE_STACK_K
  mov dword [curprg], 2
  jmp pit_done

prog2:
  ; Just ran prog2 -- switch back to prog1
  mov [prog2_rsp], rsp
  mov rsp, [prog1_rsp]
  POPA

  mov rax, PROG1_PML4
  mov cr3, rax

  mov qword [tss64_sp], PROG1_BASE_STACK_K
  mov dword [curprg], 1
  jmp pit_done

pit_done:

  ; End-of-interrupt
  mov al, 0x20
  out 0x20, al

  iretq

curprg: dd 0
prog1_rsp: dq 0
prog2_rsp: dq 0


gdt_d:
  dw end_gdt - gdt - 1
  dd gdt

gdt:
  GDT 0, 0, 0, 0
  GDT 0, 0xfffff, 10011010b, 1010b ; ring0 cs (all mem)
  GDT 0, 0xfffff, 10010010b, 1010b ; ring0 ds (all mem)
  GDT 0, 0xfffff, 11111010b, 1010b ; ring3 cs (all mem), sz=0, L=1 for 64-bit
  GDT 0, 0xfffff, 11110010b, 1010b ; ring3 ds (all mem), sz=0, L=1 for 64-bit
  GDT64 OFFS(tss64), (end_tss64 - tss64), 11101001b, 0001b ; 64-bit pointer to ts, byte granularity
end_gdt:

; See "TASK MANAGEMENT IN 64-BIT MODE" of http://download.intel.com/products/processor/manual/325384.pdf
tss64:
  dd 0
tss64_sp:
  dq PROG1_BASE_STACK_K
  TIMES 23 dd 0
end_tss64:

idt_hdr:
dw end_idt_data - idt_data
dq idt_data

idt_data:

; CPU Exception Handlers
%assign i 0
%rep 32
IDT64 0x08, OFFS(cpu %+ i), 10001110b
%assign i i+1
%endrep

; IRQ Handlers
%assign i 0
%rep    16
IDT64 0x08, OFFS(irq %+ i), 11101110b
%assign i i+1
%endrep

; Software Interrupt at 48 (30h)
IDT64 0x08, OFFS(software_interrupt), 11101111b

end_idt_data:

TIMES 0x4000 - ($ - $$) db 0
