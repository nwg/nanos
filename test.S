[BITS 32] ;Tells the assembler that its a 16 bit code
[ORG 0x700]  ;Origin, tell the assembler where the code will
        ;be in memory after it is been loaded

  mov dword [0xb8000], 0x01690148
  jmp $


TIMES 0x4000 - ($ - $$) db 0 ;Fill the rest of 16k with 0
