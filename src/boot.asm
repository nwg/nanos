
%define IMAGE_DEST 0x10000
%define IMAGE_SIZE 0x28000
%define ENTRY_POINT IMAGE_DEST
%define INIT_PAGES_ADDR 0x8000
%define INIT_STACK 0x8000

%define INIT_PML4 INIT_PAGES_ADDR
%define INIT_PDPT (INIT_PML4 + 0x1000)
%define INIT_PDT (INIT_PDPT + 0x1000)
%define INIT_PT (INIT_PDT + 0x1000)

; GDT base limit access flags
%macro GDT 4
  dw %2 & 0xFFFF
  dw %1
  db %1 >> 16
  db %3
  db (%2 >> 16) & 0x0F | (%4 << 4)
  db %1 >> 24
%endmacro

[BITS 16]
[ORG 0x7C00]
  mov eax, 1
  mov ebx, IMAGE_DEST
  mov ecx, (IMAGE_SIZE / 512) ; Num sectors

read:
  call readsector
  dec ecx
  cmp ecx, 0
  jnz read

  mov ax, 0
  mov es, ax

  lgdt [gdt_d]
  cli

  ; Set protected mode flag
  MOV EAX, CR0
  OR AL, 1
  MOV CR0, EAX

  ; Enable A20
  in al, 0x92
  test al, 2
  jnz donea20
  or al, 2
  and al, 0xFE
  out 0x92, al
donea20:

  mov ax, 0x10      ; 0x10 is the offset in the GDT to our data segment
  mov ds, ax
  mov es, ax
  mov fs, ax
  mov gs, ax
  mov ss, ax

  mov esp, INIT_STACK

  ; jump to kernel
  jmp 0x08:protmode


gdt_d:
  dw end_gdt - gdt - 1
  dd gdt

gdt:
  GDT 0, 0, 0, 0
  GDT 0, 0xfffff, 10011010b, 1100b
  GDT 0, 0xfffff, 10010010b, 1100b
end_gdt:

; eax: sector number
; ebx: memory dest
; http://en.wikipedia.org/wiki/INT_13H#INT_13h_AH.3D42h:_Extended_Read_Sectors_From_Drive
readsector:
  push eax
  push ebx
  push ecx

  push 0
  push 0
  push eax
  mov ecx, ebx
  shr ecx, 4
  and cx, 0xF000
  push cx
  push bx
  push 1
  push 16
  mov si, sp
  mov ah, 0x42
  mov dl, 0x80
  int 0x13
  add sp, 16

  pop ecx
  pop ebx
  pop eax
  inc eax
  add ebx, 512
  ret

[BITS 32]

protmode:
  mov ecx, 0

fill:

  ; Top level structures point at each other and single dirent
  mov dword [INIT_PML4+ecx], INIT_PDPT
  or dword [INIT_PML4+ecx], 111b
  mov dword [INIT_PML4+ecx+4], 0
  mov dword [INIT_PDPT+ecx], INIT_PDT
  or dword [INIT_PDPT+ecx], 111b
  mov dword [INIT_PDPT+ecx+4], 0
  mov dword [INIT_PDT+ecx], INIT_PT
  or dword [INIT_PDT+ecx], 111b
  mov dword [INIT_PDT+ecx+4], 0

  ; Identity mapping
  mov eax, (4096/8)
  mul ecx
  mov dword [INIT_PT+ecx], eax
  or dword [INIT_PT+ecx], 111b
  mov dword [INIT_PT+ecx+4], 0

  add ecx, 8
  cmp ecx, 4096
  jl fill

  ; Enable pae
  mov eax, 10100000b                ; Set the PAE and PGE bit.
  mov cr4, eax

  ; Point to long-mode paging root
  mov eax, INIT_PML4
  mov cr3, eax

  ; Set lm bit in the EFER MSR (0xC0000080)
  mov ecx, 0xC0000080
  rdmsr
  or eax, 1 << 8
  wrmsr

  ; Enable paging
  mov eax, cr0
  or eax, 0x80000000
  mov cr0, eax

  ; Reset gdt
  lgdt [gdt64_d]
  jmp 0x08:longmode

[BITS 64]
longmode:
  jmp ENTRY_POINT

gdt64_d:
  dw end_gdt64 - gdt64 - 1
  dd gdt64

gdt64:
  GDT 0, 0, 0, 0
  GDT 0, 0xfffff, 10011010b, 1010b ; ring0 cs (all mem)
  GDT 0, 0xfffff, 10010010b, 1010b ; ring0 ds (all mem)
  GDT 0, 0xfffff, 11111010b, 1010b ; ring3 cs (all mem), sz=0, L=1 for 64-bit
  GDT 0, 0xfffff, 11110010b, 1010b ; ring3 ds (all mem), sz=0, L=1 for 64-bit
end_gdt64:


TIMES 510 - ($ - $$) db 0 ;Fill the rest of sector with 0
DW 0xAA55     ;Add boot signature at the end of bootloader


