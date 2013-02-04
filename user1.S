%include "common.mac"
%define START (USER_VIDEO + 0x6e0)
%define DELAY 26843545

[BITS 64]
[ORG USER_TEXT]
  int 48

  mov edx, START

draw_char:
  mov dword [edx], 0x04070407

  mov ebx, DELAY
sleep:
  sub ebx, 1
  jnz sleep

  add edx, 4
  cmp edx, START + 80*2
  jl draw_char

  jmp $

TIMES 0x4000 - ($ - $$) db 0 ; Pad 0s to 16k
