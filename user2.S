%define DELAY 26843545
%define START 0xb8780

[BITS 32] ;Tells the assembler that its a 16 bit code
[ORG 0xfe00]  ;Origin, tell the assembler where the code will
        ;be in memory after it is been loaded

  ;mov al, 0xff
  ;out 0xa1, al
  ;out 0x21, al

  ;int 32

  ;mov dword [0xb8000], 0x02690248
  ;mov dword [ecx], 0x02690269
  mov ecx, START

draw_char:
  mov dword [ecx], 0x01070107

  mov ebx, DELAY
sleep:
  sub ebx, 1
  jnz sleep


  add ecx, 4
  cmp ecx, START + 80*2
  jl draw_char

  jmp $


TIMES 0x4000 - ($ - $$) db 0 ;Fill the rest of 16k with 0
