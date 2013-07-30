#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "types.h"

#define NULL ((void*)0x0)

// GCC builtins
void	*memchr(const void *, int, size_t);
int	 memcmp(const void *, const void *, size_t);
void	*memcpy(void *, const void *, size_t);
void	*memmove(void *, const void *, size_t);
void	*memset(void *, int, size_t);
void memsetqw(void *mem, u64_t qw, size_t count);

// Builtin memset only seems to be available for small sizes 8192 and lower
// #define memset __builtin_memset

#endif
