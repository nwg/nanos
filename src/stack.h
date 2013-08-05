#ifndef __STACK_H__
#define __STACK_H__

#include "types.h"
#include "ptr.h"

typedef uintnative_t *stackptr_t;

#define STACK(addr, size) (stackptr_t)(((uintptr_t)addr + size))
#define STACK_PUSH(ptr, val) (*--ptr = val)
#define STACK_INC(ptr, val) (ptr = (stackptr_t)((uintptr_t)ptr - (val)))
#define STACK_ALIGN(ptr, bounds) (ptr = (stackptr_t)alignr((uintptr_t)ptr, bounds))

stackptr_t push_string(stackptr_t s, const char *str);

#endif