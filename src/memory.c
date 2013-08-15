#include "memory.h"

void	*memset(void *m, int value, size_t count) {
	uint8_t *bytes = (uint8_t*)m;
	for (int i = 0; i < count; i++) {
		bytes[i] = value;
	}

	return m;
}

void *memcpy(void *dst, const void *src, size_t count) {
	uint8_t *src8 = (uint8_t*)src;
	uint8_t *dst8 = (uint8_t*)dst;

	for (int i = 0; i < count; i++) {
		dst8[i] = src8[i];
	}

	return dst;
}

void memsetqw(void *mem, uint64_t qw, size_t count) {
    for (int i = 0; i < count; i++) {
        ((uint64_t*)mem)[i] = qw;
    }
}
