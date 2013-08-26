#ifndef __KERNEL_H__
#define __KERNEL_H__

#include <stdint.h>
#include <stdio.h>
#include <stdarg.h>

#define KERNEL_HEAP 0x100000
#define KERNEL_STACK 0x8000

extern uintptr_t *kernel_pml4;
extern uintptr_t *kernel_pdpt;
extern uintptr_t *kernel_pdt;
extern uintptr_t *kernel_pt;

typedef struct {
    uintptr_t rip;
    uintptr_t code_selector;
    uint64_t rflags;
    uintptr_t rsp;
    uintptr_t data_selector;
} iretq_state_t;

typedef struct {
    uint64_t rax;
    uint64_t rbx;
    uint64_t rcx;
    uint64_t rdx;
    uint64_t rbp;
    uint64_t rsi;
    uint64_t rdi;
    uint64_t r8;
    uint64_t r9;
    uint64_t r10;
    uint64_t r11;
    uint64_t r12;
    uint64_t r13;
    uint64_t r14;
    uint64_t r15;
} register_state_t;

typedef struct {
    register_state_t registers;
    iretq_state_t iretq;
} system_state_t;

typedef enum {
    PRIV_RING0 = 0,
    PRIV_RING3 = 3
} privilege_level_e;

extern void *k_tss64_sp;
extern system_state_t *k_replace_system_state;

int kvprintf(const char * restrict format, va_list ap);
int kprintf(const char * restrict format, ...);

#define STATE_IS_PRIV(state, priv) ((state->iretq.code_selector & 3) == priv)
#define IS_USER_STATE(state) STATE_IS_PRIV(state, PRIV_RING3)
#define IS_KERNEL_STATE(state) STATE_IS_PRIV(state, PRIV_RING0)

#define PANIC(s) ({ printf("%s", s); while (true) {} })

#endif
