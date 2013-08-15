#ifndef __MEMORY_H__
#define __MEMORY_H__

#include <memory.h>
#include <stdint.h>
#include <stddef.h>

void	*memcpy(void *dst, const void *src, size_t count);
void	*memset(void *, int, size_t);
void memsetqw(void *mem, uint64_t qw, size_t count);

#endif
