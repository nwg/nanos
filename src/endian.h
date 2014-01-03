#include <stdint.h>
#include <stddef.h>

#define LITTLE_ENDIAN (((union { unsigned x; unsigned char c; }){1}).c)
#define BIG_ENDIAN (!LITTLE_ENDIAN)

void swap(void *tgt, size_t count);
uint64_t swap64(uint64_t i);
uint32_t tofromlittle32(uint32_t i);
void inplace_swap64(uint64_t *i);
void inplace_swap32(uint32_t *i);
void inplace_swap16(uint16_t *i);
uint32_t tofromlittle16(uint16_t i);
