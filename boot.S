[BITS 16] ;Tells the assembler that its a 16 bit code
[ORG 0x7C00]  ;Origin, tell the assembler that where the code will
        ;be in memory after it is been loaded

;call ReadDisk
MOV SI, HelloString
CALL PrintString  ;Call print string procedure
call ReadDisk
jmp 0x500
CALL PrintString  ;Call print string procedure
MOV SI, HelloString


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

; Read 1 sector (512 bytes)
mov al, 1

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

TIMES 510 - ($ - $$) db 0 ;Fill the rest of sector with 0
DW 0xAA55     ;Add boot signature at the end of bootloader

