#ifndef __KMEM_H__
#define __KMEM_H__

#include "types.h"

#define KALLOC_START ((void*)0x100000)

// #define KERNEL_PAGES_START 0x100000
#define KERNEL_PAGES_START 0x1C0000
#define KERNEL_PML4 0x1C0000
#define KERNEL_PDPT 0x1C1000
#define KERNEL_PDT 0x1C2000
#define KERNEL_PT 0x1C3000

#define KERNEL_HEAP 0x104000
#define KERNEL_HEAP_SIZE 0xbc000

#define KERNEL_IDENT_END 0x1c0000
#define KERNEL_VIDEO 0x1c0000
#define KERNEL_VIDEO_SIZE 8192

void *kalloc(size_t size);
void *kalloc_aligned(size_t size, size_t align);

#endif