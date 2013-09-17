#ifndef __SYSCALL_H__
#define __SYSCALL_H__

#include "kernel.h"

#define SYSCALL_ID 0x30

typedef enum {
    SYSCALL_EXIT = 0, // SYSCALL_EXIT must be 0 (crt0.asm)
    SYSCALL_TEST,
    SYSCALL_GET_TICKS,
    SYSCALL_SBRK,
    SYSCALL_SLEEP,
    SYSCALL_READ,
    SYSCALL_WRITE,
    SYSCALL_YIELD,
    SYSCALL_SPAWN,
    SYSCALL_WAIT,
    SYSCALL_GETPID,
} syscall_code_e;

static inline uint64_t _syscall0(uint64_t code) {
    uint64_t ret;

    __asm__ __volatile__ (
        "movq %1, %%rdi\n\t"
        "int %2\n\t"
        "movq %%rax, %0\n\t"
        :  "=m" (ret)
        : "m" (code), "i" (SYSCALL_ID)
        : "rdi", "rax"
    );

    return ret;
}

static inline uint64_t _syscall1(uint64_t code, uint64_t arg1) {
    uint64_t ret;

    __asm__ __volatile__ (
        "movq %1, %%rdi\n\t"
        "movq %2, %%rsi\n\t"
        "int %3\n\t"
        "movq %%rax, %0\n\t"
        :  "=m" (ret)
        : "m" (code), "m" (arg1), "i" (SYSCALL_ID)
        : "rdi", "rsi", "rax"
    );

    return ret;
}

static inline uint64_t _syscall2(uint64_t code, uint64_t arg1, uint64_t arg2) {
    uint64_t ret;

    __asm__ __volatile__ (
        "movq %1, %%rdi\n\t"
        "movq %2, %%rsi\n\t"
        "movq %3, %%rdx\n\t"
        "int %4\n\t"
        "movq %%rax, %0\n\t"
        :  "=m" (ret)
        : "m" (code), "m" (arg1), "m" (arg2), "i" (SYSCALL_ID)
        : "rdi", "rsi", "rdx", "rax"
    );

    return ret;
}

static inline uint64_t _syscall3(uint64_t code, uint64_t arg1, uint64_t arg2, uint64_t arg3) {
    uint64_t ret;

    __asm__ __volatile__ (
        "movq %1, %%rdi\n\t"
        "movq %2, %%rsi\n\t"
        "movq %3, %%rdx\n\t"
        "movq %4, %%rcx\n\t"
        "int %5\n\t"
        "movq %%rax, %0\n\t"
        :  "=m" (ret)
        : "m" (code), "m" (arg1), "m" (arg2), "m" (arg3), "i" (SYSCALL_ID)
        : "rdi", "rsi", "rdx", "rcx", "rax"
    );

    return ret;
}

#define syscall0 _syscall0
#define syscall1(code, arg1) _syscall1((uint64_t)code, (uint64_t)arg1)
#define syscall2(code, arg1, arg2) _syscall2((uint64_t)code, (uint64_t)arg1, (uint64_t)arg2)
#define syscall3(code, arg1, arg2, arg3) _syscall3((uint64_t)code, (uint64_t)arg1, (uint64_t)arg2, (uint64_t)arg3)

#define sys_test() syscall0(SYSCALL_TEST)
#define sys_getpid() syscall0(SYSCALL_GETPID)
#define sys_get_ticks() syscall0(SYSCALL_GET_TICKS)
#define sys_yield() syscall0(SYSCALL_YIELD)
#define sys_exit() syscall0(SYSCALL_EXIT)
#define sys_sbrk(incr) syscall1(SYSCALL_SBRK, incr)
#define sys_sleep(useconds) syscall1(SYSCALL_SLEEP, useconds)
#define sys_read(filedes, buf, nbyte) syscall3(SYSCALL_READ, filedes, buf, nbyte)
#define sys_write(filedes, buf, nbyte) syscall3(SYSCALL_WRITE, fildes, buf, nbyte)
#define sys_spawn(text, argc, argv) syscall3(SYSCALL_SPAWN, text, argc, argv)
#define sys_wait(stat_loc) syscall1(SYSCALL_WAIT, stat_loc)

#define YIELD() sys_yield()

#define SYS_CALLNUM(r) r.rdi
#define SYS_P1(r) r.rsi
#define SYS_P2(r) r.rdx
#define SYS_P3(r) r.rcx
#define SYS_P4(r) r.r8
#define SYS_P5(r) r.r9
#define SYS_RET(r) r.rax

void handle_syscall(system_state_t *state);


#endif
