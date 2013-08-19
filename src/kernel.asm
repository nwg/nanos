%define TOP KERNEL_START
%define OFFS(a) (TOP + a - $$)
%include "common.mac"

global _start
global tss64_sp
global return_to_process

extern schedule, return_from_schedule
extern handle_timer_interrupt
extern spawn
extern kernel_init
extern handle_syscall
extern spawn_test_programs
extern process_running
extern test_print

[BITS 64]
;[org TOP]; start of second block of conventional memory

_start:

  ; Load gdt with tss ptr
  lgdt [gdt_d]

  mov rsp, KERNEL_STACK

  ; Enable IRQ0 (PIT)
  mov al, 0xfe
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
  call spawn_test_programs
  jmp schedule

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

  ; Let scheduler know we are done with this process
  PUSHA
  mov rdi, rsp
  call return_from_schedule

  ; handler in c
  mov rdi, rsp
  jmp handle_syscall

pit_handler:

  ; send eoi
  mov al, 20h
  out 20h, al
  cli

  ; Immediately stash state
  PUSHA

  ; Was there a process running?
  call process_running
  cmp rax, 0
  je no_process

  ; If yes, Notify scheduler we are done with running process
  mov rdi, rsp
  call return_from_schedule
  jmp done_pit

  ; If no, fix stack
no_process:
  POPA

done_pit:
  ; schedule next process
  jmp handle_timer_interrupt

prog1_rsp: dq 0
prog2_rsp: dq 0

; Called by scheduler to return to user mode
return_to_process:
  POPA
  iretq

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
