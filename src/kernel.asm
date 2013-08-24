%define TOP KERNEL_START
%define OFFS(a) (TOP + a - $$)
%include "common.mac"

global _start
global k_tss64_sp
global k_replace_system_state

extern kernel_init
extern handle_interrupt

[BITS 64]
;[org TOP]; start of second block of conventional memory

_start:

  ; Load gdt with tss ptr
  lgdt [gdt_d]

  mov rsp, KERNEL_STACK

  ; Enable IRQ0 (PIT)
  mov al, 0xfc
  out 0xa1, al
  mov al, 0xff
  out 0x21, al

  ; Disable PIC
  ; mov al, 0xff
  ; out 0xa1, al
  ; out 0x21, al

  ; Load interrupt descriptor table
  lidt [idt_hdr]

  ; set up tss (task register, 16-byte descriptor in GDT)
  mov ax, 0x28
  ltr ax

  ; remap the PIC
  ; Master is irq0 to irq7 and starts at 32 (0x20)
  ; Slave is irq8 to irq15 and starts at handler 40 (0x28)
  in al, PIC1_DATA
  push rax
  in al, PIC2_DATA
  push rax

  mov al, ICW1_INIT+ICW1_ICW4
  out PIC1_COMMAND, al
  IOWAIT
  out PIC2_COMMAND, al
  IOWAIT
  mov al, 0x20
  out PIC1_DATA, al
  IOWAIT
  mov al, 0x28
  out PIC2_DATA, al
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

  pop rax
  out PIC1_DATA, al
  IOWAIT
  pop rax
  out PIC2_DATA, al
  IOWAIT

  ;mov dword [USER_VIDEO + 80*2*14], 0x01690148

  call kernel_init
  jmp interrupt.return

irqno:
  dq 0

%macro INT 1
mov byte [irqno], %1
jmp interrupt
%endmacro


cpu0:
  INT 0
cpu1:
  INT 1
cpu2:
  INT 2
cpu3:
  INT 3
cpu4:
  INT 4
cpu5:
  INT 5
cpu6:
  INT 6
cpu7:
  INT 7
cpu8:
  INT 8
cpu9:
  INT 9
cpu10:
  INT 10
cpu11:
  INT 11
cpu12:
  INT 12
cpu13:
  INT 13
cpu14:
  INT 14
cpu15:
  INT 15
cpu16:
  INT 16
cpu17:
  INT 17
cpu18:
  INT 18
cpu19:
  INT 19
cpu20:
  INT 20
cpu21:
  INT 21
cpu22:
  INT 22
cpu23:
  INT 23
cpu24:
  INT 24
cpu25:
  INT 25
cpu26:
  INT 26
cpu27:
  INT 27
cpu28:
  INT 28
cpu29:
  INT 29
cpu30:
  INT 30
cpu31:
  INT 31

irq0:
  INT 32
irq1:
  INT 33
irq2:
  INT 34
irq3:
  INT 35
irq4:
  INT 36
irq5:
  INT 37
irq6:
  INT 38
irq7:
  INT 39
irq8:
  INT 40
irq9:
  INT 41
irq10:
  INT 42
irq11:
  INT 43
irq12:
  INT 44
irq13:
  INT 45
irq14:
  INT 46
irq15:
  INT 47

software_interrupt:
  INT 48

interrupt:
  PUSHA

  ; handle_interrupt(code, state)
  mov byte rdi, [irqno]
  mov qword rsi, rsp
  call handle_interrupt

  ; send eoi
  mov al, 20h
  out 20h, al

  ; Swap out system state (@rsp) if requested
.return:
  cmp dword [k_replace_system_state], 0
  je .finish_return
  mov rsp, [k_replace_system_state]
  mov dword [k_replace_system_state], 0

.finish_return:
  POPA
  iretq

k_replace_system_state:
  dq 0

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
k_tss64_sp:
  dq 0 ; rsp will be set to this value when switching back to kernel
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
