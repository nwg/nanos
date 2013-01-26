%define DELAY 26843545
%define START 0xb8780

[BITS 32]
[ORG 0xfe00]

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

TIMES 0x4000 - ($ - $$) db 0 ; Pad 0s to 16k
