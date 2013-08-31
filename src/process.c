#include "memory.h"
#include "process.h"
#include "kmem.h"
#include "pages.h"
#include "asm.h"
#include "stack.h"
#include "kernel.h"
#include <stdio.h>
#include "timer.h"
#include <string.h>
#include "user_vga.h"
#include "syscall.h"

uintptr_t *process_page_dirent_alloc(uintptr_t stack_u, uintptr_t text);
void *process_page_table_alloc(uintptr_t stack_u, uintptr_t text);
stackptr_t push_argv(void *vstart, void *pstart, stackptr_t s, int argc, char **argv);
stackptr_t push_system_state(stackptr_t k, void *stack_u, int argc, char **argv);
bool process_done_sleep(process_t *this);

process_t *process_alloc(void *text, int argc, char **argv) {
    process_t *process = (process_t*)kalloc(sizeof(process_t));
    memset(process, 0, sizeof(process));

    process->text = text;
    process->stack_k = kalloc(K_STACK_SIZE);
    process->stack_u = kalloc_aligned(U_STACK_SIZE, 4096);
    process->pages = process_page_table_alloc((uintptr_t)process->stack_u, (uintptr_t)text);
    process->num_pages = 0;
    process->argc = argc;
    process->runstate = PROCESS_RUNNABLE;
    process->current = false;
    process->next_switch_is_kernel = false;

    process->files = kalloc(sizeof(file_t) * PROCESS_MAX_FILES);
    process->files[0] = g_termbuf->file;
    process->files[1] = g_term->file;

    stackptr_t u = STACK(process->stack_u, U_STACK_SIZE);
    u = push_argv((void*)USER_STACK_VMA, process->stack_u, u, argc, argv);
    process->argv = (char**)u;

    stackptr_t k = STACK(process->stack_k, K_STACK_SIZE);
    process->state = (system_state_t*)push_system_state(k, process->stack_u, process->argc, process->argv);
    process->kstate = NULL;

    return process;
}

void process_add_pages(process_t *process, uint64_t num) {
    uintptr_t added_pages = (uintptr_t)kalloc_aligned(num * 4096, 4096);
    page_flag_e uf = PAGE_PRESENT | PAGE_WRITEABLE | PAGE_USER;
    pt_map(process->pages, USER_HEAP + 4096*process->num_pages, added_pages, 4096*num, uf, uf, uf, uf);

    process->num_pages += num;
}

void *process_page_table_alloc(uintptr_t stack_u, uintptr_t text) {

    uintptr_t ****pages = pt_alloc();
    page_flag_e kf = PAGE_PRESENT | PAGE_WRITEABLE;
    page_flag_e uf = PAGE_PRESENT | PAGE_WRITEABLE | PAGE_USER;
    pt_map(pages, 0, 0, 2*M, uf, uf, uf, kf);
    pt_map(pages, USER_STACK_VMA, stack_u, U_STACK_SIZE, uf, uf, uf, uf);
    pt_map(pages, USER_TEXT_VMA, text, 16*K, uf, uf, uf, uf);
    pt_map(pages, USER_VIDEO, 0xb8000, 8*K, uf, uf, uf, uf);

    return pages;
}

stackptr_t push_system_state(stackptr_t k, void *stack_u, int argc, char **argv) {
    system_state_t *state = STACK_ALLOC(k, sizeof(system_state_t));

    uint64_t curflags;
    GET_FLAGS(curflags);
    uintptr_t u_vma = GET_USER_STACK_VMA(stack_u, argv);
    uintptr_t argv_vma = u_vma;

    state->iretq.data_selector = 0x20 | PRIV_RING3;
    state->iretq.rsp = u_vma;
    state->iretq.rflags = curflags | 0x200;
    state->iretq.code_selector = 0x18 | PRIV_RING3;
    state->iretq.rip = USER_TEXT_VMA;

    memset(&(state->registers), 0, sizeof(state->registers));
    state->registers.rdi = argc;        // First argument for program main
    state->registers.rsi = argv_vma;    // Second argument for program main

    return k;
}

void process_description(char *buf, int n, process_t *p) {
    char *name = (char*)GET_USER_STACK_PMA(p->stack_u, p->argv[0]);
    snprintf(buf, n, "%s @ %d", name, (int)(uintptr_t)p->text);
}

void dump_process(process_t *p) {
    char buf[256];
    process_description(buf, 256, p);
    kprintf("%s\n", buf);
}

/*
 * Copy argv and its contents to stack at s
 * Returns new pointer to argv
 *
 * This will only work for backwards-growing stacks
 */
stackptr_t push_argv(void *vstart, void *pstart, stackptr_t s, int argc, char **argv) {

    uintptr_t tmp_argv[argc];

    for (int i = 0; i < argc; i++) {
        s = push_string(s, argv[i]);
        tmp_argv[i] = (uintptr_t)s - (uintptr_t)pstart + (uintptr_t)vstart;
    }
    STACK_ALIGN(s, NATIVE_WORD_SIZE);

    for (int i = argc - 1; i >= 0; i--) {
        STACK_PUSH(s, (uintptr_t)tmp_argv[i]);
    }

    return s;
}

void process_set_file(process_t *this, int fileno, file_t *file) {
    if (fileno > PROCESS_MAX_FILES) {
        PANIC("Max files");
    }

    this->files[fileno] = file;
}

/* Perform context switch and enter user mode to run given process */
void switch_to_process(process_t *process) {
    BOCHS_BRK();
    SET_CR3(process->pages);

    // Kernel stack base address (for interrupts)
    stackptr_t k = STACK(process->stack_k, K_STACK_SIZE);
    SET_TSS_RSP(k);

    // Set up segments, including data segment (fourth GDT entry in kernel.asm)
    SET_ALL_SEGMENTS(0x20 | PRIV_RING3);

    // Flag process as running
    process->current = true;

    // Set rsp kernel will use when switching back to user process
    if (process->next_switch_is_kernel) {
        k_replace_system_state = process->kstate;
        process->next_switch_is_kernel = false;
    } else {
        k_replace_system_state = process->state;
    }

    // dump_process(process);
}

void switch_from_process(process_t *process) {
    process->current = false;
}

/*
 * All interrupt events should stash state using this function
 */
void process_stash_state(process_t *process, system_state_t *state) {
    if (!process->current) {
        PANIC("return_from_process on noncurrent process");
    }

    if (IS_USER_STATE(state)) {
        process->state = state;
    } else {
        process->next_switch_is_kernel = true;
        process->kstate = state;
    }
}

void process_sleep(process_t *this, useconds_t useconds) {
    uint64_t sleep_until = g_timer_ticks + TIMER_GET_TICKS_US(useconds);
    this->runstate = PROCESS_SLEEPING;
    this->runinfo.sleep_until_tick = sleep_until;
    YIELD();
    this->runstate = PROCESS_RUNNABLE;
}

bool process_runnable(process_t *this) {
    switch (this->runstate) {
        case PROCESS_RUNNABLE:
            return true;

        case PROCESS_SLEEPING:
            return g_timer_ticks >= this->runinfo.sleep_until_tick;

        case PROCESS_WAIT_READ:
            return file_can_read(this->runinfo.fileinfo.file);

        case PROCESS_WAIT_WRITE:
            return file_can_write(this->runinfo.fileinfo.file);
    }

    return false;
}

ssize_t process_read_file(process_t *this, int fileno, char *buf, size_t len) {

    file_t *file = this->files[fileno];
    if (!file) return -1;

    this->runstate = PROCESS_WAIT_READ;
    this->runinfo.fileinfo.fileno = fileno;
    this->runinfo.fileinfo.file = file;

    ssize_t result;
    while ( (result = file_read(file, buf, len)) < 0) {
        YIELD();
    }

    this->runstate = PROCESS_RUNNABLE;
    return result;
}

ssize_t process_write_file(process_t *this, int fileno, const char *buf, size_t len) {

    file_t *file = this->files[fileno];
    if (!file) return -1;

    this->runstate = PROCESS_WAIT_WRITE;
    this->runinfo.fileinfo.fileno = fileno;
    this->runinfo.fileinfo.file = file;

    ssize_t result;
    while ( (result = file_write(file, buf, len)) < 0) {
        YIELD();
    }

    this->runstate = PROCESS_RUNNABLE;
    return result;
}
