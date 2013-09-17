
#include "user.h"
#include "syscall.h"

int spawn(void *text, int argc, char **argv) {
    return sys_spawn(text, argc, argv);
}

uint64_t get_ticks() {
    return sys_get_ticks();
}
