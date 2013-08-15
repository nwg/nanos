#ifndef __KERNEL_H__
#define __KERNEL_H__

#include <stdint.h>

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
} system_state_t;

#endif
