#include "memory.h"
#include "types.h"

void	*memset(void *m, int value, size_t count) {
	u8_t *bytes = (u8_t*)m;
	for (int i = 0; i < count; i++) {
		bytes[i] = value;
	}

	return m;
}

void *memcpy(void *dst, const void *src, size_t count) {
	u8_t *src8 = (u8_t*)src;
	u8_t *dst8 = (u8_t*)dst;

	for (int i = 0; i < count; i++) {
		dst8[i] = src8[i];
	}

	return dst;
}

void memsetqw(void *mem, u64_t qw, size_t count) {
    for (int i = 0; i < count; i++) {
        ((u64_t*)mem)[i] = qw;
    }
}
