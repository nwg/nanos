#include "ring.h"
#include <memory.h>
#include "num.h"

ring_t *ring_alloc(ring_allocator allocator, int length, size_t width) {
    ring_t *ring = allocator(sizeof(ring_t));
    ring->rows = allocator(sizeof(void*)*length);
    ring->length = length;
    ring->width = width;
    ring->current_length = 0;
    ring->top = 0;
    ring->allocator = allocator;

    return ring;
}

void ring_dealloc(ring_deallocator dealloc, ring_t *ring) {
    for (int i = ring->top; i < ring->top + ring->length; i++) {
        dealloc(ring->rows[i % ring->length]);
    }
    dealloc(ring->rows);
    dealloc(ring);
}

void *ring_add_row(ring_t *ring) {
    void *row = ring->allocator(ring->width);

    if (ring->current_length == ring->length) {
        ring->rows[ring->top] = row;
        ring->top = (ring->top + 1) % ring->length;
    } else {
        ring->rows[ring->current_length++] = row;
    }

    return row;
}

void ring_add_rows(ring_t *ring, int count) {
    for (int i = 0; i < count; i++) {
        ring_add_row(ring);
    }
}

void *ring_get_row(ring_t *ring, int index) {
    if (index >= ring->current_length) return NULL;
    if (index < -ring->current_length) return NULL;

    return ring->rows[NEGMOD(ring->top + index, ring->current_length)];
}
