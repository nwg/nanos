extern main
global _start

%define SYSCALL_EXIT 1
%define SYSCALL_INT 0x30

_start:
    call main
    
    mov rsi, rax
    mov rdi, SYSCALL_EXIT
    int SYSCALL_INT

    jmp $
