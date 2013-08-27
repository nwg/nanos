#include "memory.h"
#include "pages.h"
#include "sizes.h"
#include "kmem.h"
#include "asm.h"
#include "kernel.h"
#include <stdint.h>

uintptr_t *get_pagedir(uintptr_t *pml4, int ipdpt, int ipdt, int ipt) {
	uintptr_t *pdpt = (uintptr_t*)(pml4[ipdpt] & PAGE_MASK);
	uintptr_t *pdt = (uintptr_t*)(pdpt[ipdt] & PAGE_MASK);
	uintptr_t *pt = (uintptr_t*)(pdt[ipt] & PAGE_MASK);

	return pt;
}
