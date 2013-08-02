%include "common.mac"
%define START (USER_VIDEO + 0x6e0)
%define DELAY 26843545

[BITS 64]
[ORG USER_TEXT]

  ; This is the test interrupt (prints "Hi")
  mov rax, 0
  int 48

  ; This is a heap extend, which gives us a page at 0x300000
  mov rax, 1
  mov rbx, 1
  int 48

  ; Test that we can write to our new page
  mov rax, 0x300000
  mov [rax], dword 10

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
