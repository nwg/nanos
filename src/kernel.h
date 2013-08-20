#ifndef __KERNEL_H__
#define __KERNEL_H__

#include <stdint.h>
#include <stdio.h>

#define KERNEL_HEAP 0x100000

extern uintptr_t *kernel_pml4;
extern uintptr_t *kernel_pdpt;
extern uintptr_t *kernel_pdt;
extern uintptr_t *kernel_pt;

typedef struct {
	uint64_t rax;
	uint64_t rbx;
	uint64_t rcx;
	uint64_t rdx;
	uint64_t rbp;
	uint64_t rsi;
    uint64_t rdi;
    uint64_t r8;
    uint64_t r9;
    uint64_t r10;
    uint64_t r11;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
} system_state_t;

#define PANIC(s) ({ printf("%s", s); while (true) {} })

#endif
