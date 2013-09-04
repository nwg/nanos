#include <sys/wait.h>
#include "syscall.h"

pid_t wait(int *stat_loc) {
    pid_t result;
    sys_wait(stat_loc, result);
    return result;
}
