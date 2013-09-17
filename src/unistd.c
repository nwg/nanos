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
    return sys_read(filedes, buf, nbyte);
}

ssize_t write(int fildes, const void *buf, size_t nbyte) {
    return sys_write(filedes, buf, nbyte);
}

void *sbrk(ptrdiff_t incr) {
    return (void*)sys_sbrk(incr);
}

pid_t getpid() {
    return sys_getpid();
}
