#include "kernel.h"
#include "schedule.h"
#include "memory.h"
#include "kmem.h"
#include "pages.h"
#include "asm.h"

uintptr_t *kernel_pml4;
uintptr_t *kernel_pdpt;
uintptr_t *kernel_pdt;
uintptr_t *kernel_pt;

/**
 * Configure first 2MB for initial kernel use
 */
void init_kernel_pages() {
	kernel_pml4 = kalloc_aligned(4096, 4096);
	kernel_pdpt = kalloc_aligned(4096, 4096);
	kernel_pdt = kalloc_aligned(4096, 4096);
	kernel_pt = kalloc_aligned(4096, 4096);

	for (int i = 0; i < 512; i++) {
		kernel_pml4[i] = (uintptr_t)kernel_pdpt | PAGE_PRESENT;
		kernel_pdpt[i] = (uintptr_t)kernel_pdt | PAGE_PRESENT;
		kernel_pdt[i] = (uintptr_t)kernel_pt | PAGE_PRESENT;
	}

	// Just an identity map of first 2MB of memory
	for (int i = 0; i < 512; i++) {
		kernel_pt[i] = i*4096 | PAGE_PRESENT | PAGE_WRITEABLE;
	}

	SET_CR3(kernel_pml4);
}

void kernel_init() {
	init_kernel_pages();
	schedule_init();
}