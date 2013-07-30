%define TOP 0x7C00
%define OFFS(a) (TOP + a - $$)
%include "common.mac"

[BITS 16] ;Tells the assembler that its a 16 bit code
[ORG TOP]  ;Origin, tell the assembler that where the code w
        ;be in memory after it is been loaded
  mov ax, 0x800
  mov es, ax

  ; read #sectors
  mov al, 128

  ; start sector
  mov cl, 2

  ; start track
  mov ch, 0

  ; start head
  mov dh, 0

  ; memory dest
  mov bx, 0

  call ReadDisk

  mov ax, 0x1800
  mov es, ax

  ; read #sectors
  mov al, 128

  ; start sector
  mov cl, 4

  ; start track
  mov ch, 0

  ; start head
  mov dh, 2

  ; memory dest
  mov bx, 0

  call ReadDisk


  mov ax, 0
  mov es, ax

  lgdt [gdt_d]        
  cli

  ; Set protected mode flag
  MOV EAX, CR0
  OR AL, 1
  MOV CR0, EAX

  ; Enable A20
  in al, 0x92
  test al, 2
  jnz donea20
  or al, 2
  and al, 0xFE
  out 0x92, al
donea20:

  mov ax, 0x10      ; 0x10 is the offset in the GDT to our data segment
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax

  ; put stack at end of our kernel's 16k (0x8000 - 0xc000)
  mov esp, KERNEL_STACK_32

  ; jump to kernel
  jmp 0x08:0x00008000

gdt_d:
  dw end_gdt - gdt - 1
  dd gdt

gdt:
  GDT 0, 0, 0, 0
  GDT 0, 0xfffff, 10011010b, 1100b
  GDT 0, 0xfffff, 10010010b, 1100b
end_gdt:

; al: sector count
; ch: cylinder 9:2
; cl: cylinder 1:0 and sector 5:0 (1-indexed)
; dh: head (0-indexed)
; es:bx: dest buffer
ReadDisk:

  ; This is a disk read
  mov ah, 0x02

  ; This is a hard disk (0x80), and it's the first one (0x00)
  mov dl, 0x80

  ; Disk interrupt
  int 13h
  ret

TIMES 510 - ($ - $$) db 0 ;Fill the rest of sector with 0
DW 0xAA55     ;Add boot signature at the end of bootloader


