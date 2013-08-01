#ifndef __KERNEL_H__
#define __KERNEL_H__

#include "types.h"

#define KERNEL_HEAP 0x100000

extern uintptr_t *kernel_pml4;
extern uintptr_t *kernel_pdpt;
extern uintptr_t *kernel_pdt;
extern uintptr_t *kernel_pt;

typedef struct {
	u64_t rax;
	u64_t rbx;
	u64_t rcx;
	u64_t rdx;
	u64_t rbp;
	u64_t rsi;
	u64_t rdi;
} system_state_t;

#endif