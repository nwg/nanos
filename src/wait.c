#include <sys/wait.h>
#include "syscall.h"

pid_t wait(int *stat_loc) {
    return sys_wait(stat_loc);
}
