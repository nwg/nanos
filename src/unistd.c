#include <unistd.h>
#include "syscall.h"
#include <stdint.h>
#include "asm.h"

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

ssize_t read(int filedes, void *buf, size_t nbyte) {
    sys_read(filedes, buf, nbyte);
    uint64_t len;
    GET_RDI(len);
    return len;
}
