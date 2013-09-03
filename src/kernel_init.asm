%define TOP KERNEL_START
%define OFFS(a) (TOP + a - $$)
%include "common.mac"

global _start
global k_tss64_sp               ; forced rsp @ ring3 -> ring0 transition
global k_replace_system_state   ; registers and iretq will be popped starting here

extern kernel_init
extern handle_irq
extern handle_cpu_exception
extern handle_syscall
extern intel_8259_set_idt_start

[BITS 64]
;[org TOP]; start of second block of conventional memory

_start:

  ; Load gdt with tss ptr
  lgdt [gdt_d]

  ; DS constant at ring3 GDT data descriptor
  mov ax, 0x20
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax

  ; Set tss64 GDT offset
  mov ax, 0x28
  ltr ax

  mov rsp, KERNEL_STACK

  ; Load interrupt descriptor table
  lidt [idt_hdr]

  ; Set IRQ idt start index to 32 to match our idt setup below
  mov rdi, 32
  call intel_8259_set_idt_start

  call kernel_init
  jmp interrupt.return

int_no:
  dq 0

int_handler:
  dq 0

%macro INT 2
mov qword [int_handler], %1
mov qword [int_no], %2
jmp interrupt
%endmacro

%define IRQ(n) INT handle_irq, n
%define CPU(n) INT handle_cpu_exception, n

cpu0:
  CPU(0)
cpu1:
  CPU(1)
cpu2:
  CPU(2)
cpu3:
  CPU(3)
cpu4:
  CPU(4)
cpu5:
  CPU(5)
cpu6:
  CPU(6)
cpu7:
  CPU(7)
cpu8:
  CPU(8)
cpu9:
  CPU(9)
cpu10:
  CPU(10)
cpu11:
  CPU(11)
cpu12:
  CPU(12)
cpu13:
  CPU(13)
cpu14:
  CPU(14)
cpu15:
  CPU(15)
cpu16:
  CPU(16)
cpu17:
  CPU(17)
cpu18:
  CPU(18)
cpu19:
  CPU(19)
cpu20:
  CPU(20)
cpu21:
  CPU(21)
cpu22:
  CPU(22)
cpu23:
  CPU(23)
cpu24:
  CPU(24)
cpu25:
  CPU(25)
cpu26:
  CPU(26)
cpu27:
  CPU(27)
cpu28:
  CPU(28)
cpu29:
  CPU(29)
cpu30:
  CPU(30)
cpu31:
  CPU(31)

irq0:
  IRQ(0)
irq1:
  IRQ(1)
irq2:
  IRQ(2)
irq3:
  IRQ(3)
irq4:
  IRQ(4)
irq5:
  IRQ(5)
irq6:
  IRQ(6)
irq7:
  IRQ(7)
irq8:
  IRQ(8)
irq9:
  IRQ(9)
irq10:
  IRQ(10)
irq11:
  IRQ(11)
irq12:
  IRQ(12)
irq13:
  IRQ(13)
irq14:
  IRQ(14)
irq15:
  IRQ(15)

software_interrupt:
  cli
  INT handle_syscall, 0

interrupt:
  PUSHA

  ; handle_interrupt(code, state)
  mov qword rdi, rsp
  mov rsi, [int_no]
  call [int_handler]

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

align 8
dw 0
gdt_d:
  dw end_gdt - gdt - 1
  dd gdt

align 8

gdt:
  GDT 0, 0, 0, 0
  GDT 0, 0xfffff, 10011010b, 1010b ; ring0 cs (all mem)
  GDT 0, 0xfffff, 10010010b, 1010b ; ring0 ds (all mem)
  GDT 0, 0xfffff, 11111010b, 1010b ; ring3 cs (all mem), sz=0, L=1 for 64-bit
  GDT 0, 0xfffff, 11110010b, 1010b ; ring3 ds (all mem), sz=0, L=1 for 64-bit
  GDT64 OFFS(tss64), (end_tss64 - tss64), 11101001b, 0001b ; 64-bit pointer to ts, byte granularity
end_gdt:

align 8
; See "TASK MANAGEMENT IN 64-BIT MODE" of http://download.intel.com/products/processor/manual/325384.pdf
tss64:
  dd 0
k_tss64_sp:
  dq 0 ; rsp will be set to this value when switching back to kernel
  TIMES 23 dd 0
end_tss64:

align 8
  dw 0 ; force idt header at odd address
idt_hdr:
  dw end_idt_data - idt_data
  dq idt_data

align 8

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
