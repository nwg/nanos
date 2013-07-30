#include "memory.h"
#include "pages.h"
#include "types.h"
#include "sizes.h"
#include "kmem.h"
#include "video.h"
#include "asm.h"

/**
 * Configure first 2MB for initial kernel use
 */
void init_kernel_pages() {
	// u64_t *pml4 = (u64_t*)KERNEL_PML4;
	// uintnative_t *pdpt = (uintnative_t*)KERNEL_PDPT;
	// uintnative_t *pdt = (uintnative_t*)KERNEL_PDT;
	// uintnative_t *pt = (uintnative_t*)KERNEL_PT;

	memset((void*)KERNEL_PAGES_START, 0, 4096*4);
    
	// for (int i = 0; i < 512; i++) {
	// 	// pml4[i] = (uintptr_t)pdpt | PAGE_PRESENT;
	// 	// pml4[i] = 0xdeadb33fabcd1234 | PAGE_PRESENT;
	// }

	// for (int i = 0; i < 512; i++) {
	// 	pdpt[i] = (uintptr_t)pdt | PAGE_PRESENT;
	// }

	// for (int i = 0; i < 512; i++) {
	// 	pdt[i] = (uintptr_t)pt | PAGE_PRESENT;
	// }

	// // Identity map initial kernel regions (code/data, pages, heap)
	// for (int i = 0; i < (KERNEL_IDENT_END)/PAGE_SIZEQ; i++) {
	// 	pt[i] = i*PAGE_SIZEQ | PAGE_PRESENT;
	// }

	__asm__(
		// "movq $0x1c0000, %rax\n\t"
		// "movq $0x11223344, (%rax)\n\t"
		"movq $0x11223344, 0x1c0000\n\t"
		// "movabs $0xaabbccddeeff1122, %rax\n\t"
	);

	// // map physical video starting at KERNEL_VIDEO
	// uintnative_t *video = (uintnative_t*)KERNEL_VIDEO;
	// for (int i = 0; i < KERNEL_VIDEO/PAGE_SIZEQ; i++) {
	// 	video[i] = (VIDEO_START_PHYS + i*PAGE_SIZEQ) | PAGE_PRESENT;
	// }

	// pml4[0] = 0xdeadb33f;

		// blah[0]= (u64_t)0x0011223344556677;
		// blah[1]= (u64_t)0x8899aabbccddeeff;
		// blah[0] = (u64_t)&blah[0];
		// blah[1] = (u64_t)&blah[1];
		// *blah++ = (u64_t)0x0011223344556677;
		// *blah++ = (u64_t)0x8899aabbccddeeff;
		
		// u64_t *blah = (u64_t*)0x1c0000;
		// *blah = 0x11223344;
		// *blah = (uintptr_t)0x0011223344556677;
		// blah++;


		// *blah = (uintptr_t)blah;
		// blah++;

		// pml4[0] = (u64_t)0x0011223344556677;
		// pml4[2] = (u64_t)0x8899aabbccddeeff;

		// *(u64_t*)0x1c0000 = (u64_t)0x0011223344556677;
		// *(u64_t*)0x1c0008 = (u64_t)0x8899aabbccddeeff;


	// SET_CR3(pml4);
}