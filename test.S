[BITS 32] ;Tells the assembler that its a 16 bit code
[ORG 0xbe00]  ;Origin, tell the assembler where the code will
        ;be in memory after it is been loaded

  ;mov al, 0xff
  ;out 0xa1, al
  ;out 0x21, al
  int 32

  mov dword [0xb8030], 0x02690248
  jmp $


TIMES 0x4000 - ($ - $$) db 0 ;Fill the rest of 16k with 0
