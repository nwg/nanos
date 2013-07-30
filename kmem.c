#include "kmem.h"

void *kcurrent = KALLOC_START;

void *kalloc(size_t size) {
    return kalloc_aligned(size, NATIVE_WORD_SIZE);
}

uintptr_t align(uintptr_t num, size_t bounds) {
    uintptr_t top = num & ~(bounds - 1);
    if (num != top) {
        num = top + bounds;
    }
    return num;
}

void *kalloc_aligned(size_t size, size_t bounds) {
	kcurrent = (void*)align((uintptr_t)kcurrent, bounds);
    void *tmp = kcurrent;
    kcurrent += size;
    return tmp;
}


