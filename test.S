[BITS 32] ;Tells the assembler that its a 16 bit code
[ORG 0x700]  ;Origin, tell the assembler where the code will
        ;be in memory after it is been loaded

;mov esi, HelloString
;call PrintString

;mov dword [0xb7F10], 0x02690248
mov dword [0xb8000], 0x02690248

sti

;cli
mov bx, 0x10      ; 0x10 is the offset in the GDT to our data segment
mov ds, bx
mov es, bx
mov fs, bx
mov gs, bx
mov ss, bx
mov esp, 0x00100000 + 4096

%define VIDMEM  0xb8000   ; video memory
;%define VIDMEM  0xB7F00   ; video memory
mov edi, VIDMEM   ; get pointer to video memory

;mov byte [edi], 'A'    ; print character 'A'
;mov byte [edi+1], 0x7    ; character attribute

mov bx, 0
pchar:

mov word ax, [edi]
and ax, 0x02FF
mov word [edi], ax
add edi, 2
add bx, 1
cmp bx, 80*20
jne pchar

JMP $     ;Infinite loop, hang it here.

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


HelloString db 'Hello from second program', 0 ;HelloWorld string ending with 0

TIMES 512 - ($ - $$) db 0 ;Fill the rest of sector with 0


