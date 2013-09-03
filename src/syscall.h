#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include "kernel.h"

typedef enum {
	SYSCALL_TEST,
    SYSCALL_EXIT,
    SYSCALL_ADD_PAGES,
    SYSCALL_SLEEP,
    SYSCALL_READ,
    SYSCALL_WRITE,
    SYSCALL_YIELD,
    SYSCALL_SPAWN,
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

#define syscall2m(code, arg1, arg2, arg3) \
    __asm__ __volatile__ ( \
        "movq %0, %%rax\n\t" \
        "movq %1, %%rbx\n\t" \
        "movq %2, %%rcx\n\t" \
        "int $48\n\t" \
        :  \
        : "i" (code), "m" (arg1), "m" (arg2) \
        : "rax", "rbx", "rcx" \
    )

#define syscall3mo(code, arg1, arg2, arg3, r) \
    __asm__ __volatile__ ( \
        "movq %1, %%rax\n\t" \
        "movq %2, %%rbx\n\t" \
        "movq %3, %%rcx\n\t" \
        "movq %4, %%rdx\n\t" \
        "int $48\n\t" \
        "movq %%rdi, %0\n\t" \
        :  "=m" (r) \
        : "i" (code), "m" (arg1), "m" (arg2), "m" (arg3) \
        : "rax", "rbx", "rcx", "rdx", "rdi" \
    )

#define sys_test() syscall0(SYSCALL_TEST)
#define sys_yield() syscall0(SYSCALL_YIELD)
#define sys_exit() syscall0(SYSCALL_EXIT)
#define sys_add_pages(count) syscall1i(SYSCALL_ADD_PAGES, count)
#define sys_sleep(useconds) syscall1m(SYSCALL_SLEEP, useconds)
#define sys_read(filedes, buf, nbyte, ret) syscall3mo(SYSCALL_READ, filedes, buf, nbyte, ret)
#define sys_write(filedes, buf, nbyte, ret) syscall3mo(SYSCALL_WRITE, fildes, buf, nbyte, ret)
#define sys_spawn(text, argc, argv, ret) syscall3mo(SYSCALL_SPAWN, text, argc, argv, ret)

#define YIELD() sys_yield()

void handle_syscall(system_state_t *state);


#endif
