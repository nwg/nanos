%define TOP 0x7C00

[BITS 16] ;Tells the assembler that its a 16 bit code
[ORG TOP]  ;Origin, tell the assembler that where the code w
        ;be in memory after it is been loaded

;call ReadDisk
;MOV SI, HelloString
;CALL PrintString  ;Call print string procedure

;call ReadDisk

;MOV SI, HelloString
;CALL PrintString  ;Call print string procedure




;jmp 0x700

lidt [idt_hdr]
;lgdt [0x500]        ; Load the GDT from 512-byte block
lgdt [gdt_d]        
cli
;jmp $

; Disable PIC
;mov al, 0xff
;out 0xa1, al
;out 0x21, al

; Set protected mode flag
MOV EAX, CR0
OR AL, 1
MOV CR0, EAX



jmp 0x08:myshit



jmp 0x08:(0x700)   ; 0x08 is the offset to our code segment: Far jump!
jmp 0x08:stop

jmp 0x08:stop
jmp $
;jmp 0x10:$
jmp 0x08:(0x700)   ; 0x08 is the offset to our code segment: Far jump!

stop:
  MOV SI, HelloString
  CALL PrintString  ;Call print string procedure
  jmp $

[BITS 32] ;Tells the assembler that its a 16 bit code
myshit:
;lidt [idt_data]

; Enable A20
in al, 0x92
test al, 2
jnz after
or al, 2
and al, 0xFE
out 0x92, al
after:
  mov ax, 0x10      ; 0x10 is the offset in the GDT to our data segment
  mov ds, ax
  mov es, ax
  mov ss, ax
  xor ax, ax
  mov fs, ax
  mov gs, ax
  ;mov esp, 0x00200000 + 4096
  mov esp, 0x700

  sti
  ;jmp $

  mov dword [0xb8000], 0x02690248
  ;sti
  add ax, 2
  jmp $

int_handler:
  mov dword [0xb8010], 0x04690248
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
dw ((TOP + int_handler - $$)  >> 16) & 0xFFFF
;dw 0
%endrep


[BITS 16] ;Tells the assembler that its a 16 bit code


exc:
  mov dword [0xb8000], 0x03690348
  jmp $

;mov esi, HelloString
;mov edi, 0x500
;cld
;mov ecx, 6
;rep movsb
;MOV SI, 0x00000500
;CALL PrintString  ;Call print string procedure

JMP $     ;Infinite loop, hang it here.

ReadDisk:

; This is a disk read
mov ah, 0x02

; Read 2 sectors (1024 bytes)
mov al, 2

; First track
mov ch, 0

; Second Sector (1-indexed)
mov cl, 2

; First head (0-indexed)
mov dh, 0

; This is a hard disk (0x80), and it's the first one (0x00)
mov dl, 0x80

; Set es:bx to first block of conventional memory
;xor es, es
;mov ax, 0
;mov es, ax
mov bx, 0x00000500
;mov ah, 0x02
;mov al, 1

; Disk interrupt
int 13h
ret

PrintCharacter: ;Procedure to print character on screen
  ;Assume that ASCII value is in register AL
MOV AH, 0x0E  ;Tell BIOS that we need to print one charater on screen.
MOV BH, 0x00  ;Page no.
MOV BL, 0x07  ;Text attribute 0x07 is lightgrey font on black background

INT 0x10  ;Call video interrupt
RET   ;Return to calling procedure



PrintString:  ;Procedure to print string on screen
  ;Assume that string starting pointer is in register SI

next_character: ;Lable to fetch next character from string
MOV AL, [SI]  ;Get a byte from string and store in AL register
INC SI    ;Increment SI pointer
OR AL, AL ;Check if value in AL is zero (end of string)
JZ exit_function ;If end then return
CALL PrintCharacter ;Else print the character which is in AL register
JMP next_character  ;Fetch next character from string
exit_function:  ;End label
RET   ;Return from procedure


;Data
HelloString db 'Hello World', 0 ;HelloWorld string ending with 0

;idt_data:
;  dw 0x100    
;  dd 0      ; Real Mode IVT @ 0x0000

gdt_d:
  dw (24-1)
  dd gdt

gdt:
  db 00,00,00,00,00,00,00,00, \
  0xff,0xff,0x00,0x00,0x00,0x9a,0xcf,0x00,0xff,0xff,0x00,0x00,0x00,0x92,0xcf,0x00

TIMES 510 - ($ - $$) db 0 ;Fill the rest of sector with 0
DW 0xAA55     ;Add boot signature at the end of bootloader


