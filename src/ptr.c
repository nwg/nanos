#include "ptr.h"

uintptr_t align(uintptr_t num, size_t bounds) {
    uintptr_t top = num & ~(bounds - 1);
    if (num != top) {
        num = top + bounds;
    }
    return num;
}

uintptr_t alignr(uintptr_t num, size_t bounds) {
    uintptr_t top = num & ~(bounds - 1);
    if (num != top) {
        num = top;
    }
    return num;
}
