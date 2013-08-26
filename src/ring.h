#ifndef __RING_H__
#define __RING_H__

#include <stddef.h>

typedef void * (*ring_allocator)(size_t size);
typedef void * (*ring_deallocator)();

typedef struct {
    ring_allocator allocator;
    void **rows;
    size_t width;
    int length;
    int current_length;
    int top;
} ring_t;

ring_t *ring_alloc(ring_allocator allocator, int length, size_t width);
void ring_dealloc(ring_deallocator dealloc, ring_t *ring);
void *ring_add_row(ring_t *ring);
void ring_add_rows(ring_t *ring, int count);
void *ring_get_row(ring_t *ring, int index); // overflow ok

#endif
