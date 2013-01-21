%define TOP 0x7C00
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

[BITS 16] ;Tells the assembler that its a 16 bit code
[ORG TOP]  ;Origin, tell the assembler that where the code w
        ;be in memory after it is been loaded

  call ReadDisk

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

  ; put stack at end of our kernel's 16k (0x7e00 - 0xbe00)
  mov esp, 0xbdfc

  ; jump to kernel
  jmp 0x08:0x00007E00

gdt_d:
  dw end_gdt - gdt - 1
  dd gdt

gdt:
  GDT 0, 0, 0, 0
  GDT 0, 0xfffff, 10011010b, 1100b
  GDT 0, 0xfffff, 10010010b, 1100b
end_gdt:

ReadDisk:

  ; This is a disk read
  mov ah, 0x02

  ; Read 64 512-byte sectors (32KiB)
  mov al, 64

  ; First track
  mov ch, 0

  ; Second Sector (1-indexed, bytes 512 onward)
  mov cl, 2

  ; First head (0-indexed)
  mov dh, 0

  ; This is a hard disk (0x80), and it's the first one (0x00)
  mov dl, 0x80

  ; Set es:bx to second block of conventional memory
  ;xor es, es
  ;mov ax, 0
  ;mov es, ax
  mov bx, 0x00007e00
  ;mov ah, 0x02
  ;mov al, 1

  ; Disk interrupt
  int 13h
  ret

TIMES 510 - ($ - $$) db 0 ;Fill the rest of sector with 0
DW 0xAA55     ;Add boot signature at the end of bootloader


