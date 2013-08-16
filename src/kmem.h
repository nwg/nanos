#ifndef __KMEM_H__
#define __KMEM_H__

#include <stddef.h>

#define KALLOC_START ((void*)0x100000)

void *kalloc(size_t size);
void *kalloc_aligned(size_t size, size_t align);
void kfree(void *ptr);

#endif
