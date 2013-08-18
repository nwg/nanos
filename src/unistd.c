#include <unistd.h>
#include "syscall.h"
#include <stdint.h>

int usleep(useconds_t useconds) {
    sys_sleep(useconds);
    return 0;
}

unsigned int
sleep(unsigned int seconds) {
    uint64_t s = seconds * 1000000;
    sys_sleep(s);
    return 0;
}
