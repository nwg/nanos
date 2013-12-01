#include "endian.h"
#include <stdbool.h>

void swap(void *tgt, size_t count) {
    uint8_t *bytes = (uint8_t*)tgt;
    for (int i = 0; i < count / 2; i++) {
        uint8_t tmp = bytes[i];
        bytes[i] = bytes[count - 1 - i];
        bytes[count - 1 - i] = tmp;
    }
}

void inplace_swap64(uint64_t *i) {
    swap(i, sizeof(uint64_t));
}

void inplace_swap32(uint32_t *i) {
    swap(i, sizeof(uint32_t));
}

void inplace_swap16(uint16_t *i) {
    swap(i, sizeof(uint16_t));
}

uint64_t swap64(uint64_t i) {
    swap(&i, sizeof(uint64_t));
    return i;
}

uint32_t swap32(uint32_t i) {
    swap(&i, sizeof(uint32_t));
    return i;
}

uint16_t swap16(uint16_t i) {
    swap(&i, sizeof(uint16_t));
    return i;
}

uint32_t tofromlittle32(uint32_t i) {
    if (BIG_ENDIAN) {
        return swap32(i);
    }
    return i;
}
