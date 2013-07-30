%define TOP PAGES_START
%define OFFS(a) (TOP + a - $$)
%include "common.mac"

;[bits 64]

; repeat all the top-level page structures to just point to the same (16MB) map

prog1_pml4:
TIMES 512 dq OFFS(prog1_pdpt) | 00000111b

prog1_pdpt:
TIMES 512 dq OFFS(prog1_pdt) | 00000111b

prog1_pdt:
TIMES 512 dq OFFS(prog1_pt) | 00000111b

; first and only 2MB map
prog1_pt:
%assign i 0
%rep    510
dq (i*0x1000) | 000000011b
%assign i i+1
%endrep

; video at 1fe000
dq 0xb8000 | 00000111b
dq 0xb9000 | 00000111b


;---

prog2_pml4:
TIMES 512 dq OFFS(prog2_pdpt) | 00000111b

prog2_pdpt:
TIMES 512 dq OFFS(prog2_pdt) | 00000111b

prog2_pdt:
TIMES 512 dq OFFS(prog2_pt) | 00000111b

prog2_pt:
%assign i 0
%rep    510
dq (i*0x1000) | 000000011b
%assign i i+1
%endrep

; video at 1fe000
dq 0xb8000 | 00000111b
dq 0xb9000 | 00000111b
