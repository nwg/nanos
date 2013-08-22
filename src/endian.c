#include "endian.h"
#include <stdbool.h>

#define LITTLE_ENDIAN (((union { unsigned x; unsigned char c; }){1}).c)

void swap(void *tgt, size_t count) {
    uint8_t *bytes = (uint8_t*)tgt;
    for (int i = 0; i < count / 2; i++) {
        uint8_t tmp = bytes[i];
        bytes[i] = bytes[count - 1 - i];
        bytes[count - 1 - i] = tmp;
    }
}

uint64_t swap64(uint64_t i) {
    swap(&i, sizeof(uint64_t));
    return i;
}

uint64_t big64(uint64_t i) {
    if (LITTLE_ENDIAN) {
        return swap64(i);
    }
    return i;
}
