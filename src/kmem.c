#include "kmem.h"
#include "ptr.h"
#include "native.h"

void *kcurrent = KALLOC_START;

void *kalloc(size_t size) {
    return kalloc_aligned(size, NATIVE_WORD_SIZE);
}

void *kalloc_aligned(size_t size, size_t bounds) {
	kcurrent = (void*)align((uintptr_t)kcurrent, bounds);
    void *tmp = kcurrent;
    kcurrent += size;
    return tmp;
}

void kfree(void *ptr) {
}
