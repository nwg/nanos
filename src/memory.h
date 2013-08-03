#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "types.h"
#include <memory.h>

void	*memcpy(void *dst, const void *src, size_t count);
void	*memset(void *, int, size_t);
void memsetqw(void *mem, u64_t qw, size_t count);

#endif
