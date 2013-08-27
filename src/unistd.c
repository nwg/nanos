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
    ssize_t len;
    sys_read(filedes, buf, nbyte, len);
    return len;
}

ssize_t write(int fildes, const void *buf, size_t nbyte) {
    ssize_t len;
    sys_write(filedes, buf, nbyte, len);
    return len;
}
