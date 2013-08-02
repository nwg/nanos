#ifndef __STACK_H__
#define __STACK_H__

#include "types.h"

typedef uintnative_t *stackptr_t;

#define STACK(addr, size) (stackptr_t)(((uintptr_t)addr + size))
#define STACK_PUSH(ptr, val) (*--ptr = val)
#define STACK_INC(ptr, val) (ptr -= val)

#endif