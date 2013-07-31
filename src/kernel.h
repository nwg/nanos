#ifndef __KERNEL_H__
#define __KERNEL_H__

#include "types.h"

#define KERNEL_HEAP 0x100000

extern uintptr_t *kernel_pml4;
extern uintptr_t *kernel_pdpt;
extern uintptr_t *kernel_pdt;
extern uintptr_t *kernel_pt;

#endif