#ifndef __PTR_H__
#define __PTR_H__

#include <stdint.h>
#include <stddef.h>

uintptr_t align(uintptr_t num, size_t bounds);
uintptr_t alignr(uintptr_t num, size_t bounds);

#define HIGHBITS 0xffff800000000000
#define CANON(x) ( (x) & HIGHBITS ? ((x) | HIGHBITS) : (x))

#endif
