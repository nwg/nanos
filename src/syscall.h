#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include "kernel.h"

typedef enum {
	SYSCALL_TEST,
    SYSCALL_EXIT,
    SYSCALL_ADD_PAGES,
    SYSCALL_SLEEP,
    SYSCALL_READ,
} syscall_code_t;

#define syscall0(code) \
    __asm__ __volatile__ ( \
        "movq %0, %%rax\n\t" \
        "int $48\n\t" \
        :  \
        : "i" (code) \
        : "rax" \
    )

#define syscall1i(code, arg1) \
    __asm__ __volatile__ ( \
        "movq %0, %%rax\n\t" \
        "movq %1, %%rbx\n\t" \
        "int $48\n\t" \
        :  \
        : "i" (code), "i" (arg1) \
        : "rax", "rbx" \
    )

#define syscall1m(code, arg1) \
    __asm__ __volatile__ ( \
        "movq %0, %%rax\n\t" \
        "movq %1, %%rbx\n\t" \
        "int $48\n\t" \
        :  \
        : "i" (code), "m" (arg1) \
        : "rax", "rbx" \
    )

#define syscall3m(code, arg1, arg2, arg3) \
    __asm__ __volatile__ ( \
        "movq %0, %%rax\n\t" \
        "movq %1, %%rbx\n\t" \
        "movq %2, %%rcx\n\t" \
        "movq %3, %%rdx\n\t" \
        "int $48\n\t" \
        :  \
        : "i" (code), "m" (arg1), "m" (arg2), "m" (arg3) \
        : "rax", "rbx" \
    )

#define sys_test() syscall0(SYSCALL_TEST)
#define sys_exit() syscall0(SYSCALL_EXIT)
#define sys_add_pages(count) syscall1i(SYSCALL_ADD_PAGES, count)
#define sys_sleep(useconds) syscall1m(SYSCALL_SLEEP, useconds)
#define sys_read(filedes, buf, len) syscall3m(SYSCALL_READ, filedes, buf, len)

void handle_syscall(system_state_t *state);


#endif
