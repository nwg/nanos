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
    uintptr_t *pt = get_pagedir(process->pages, 0, 0, 1);

    uintptr_t added_pages = (uintptr_t)kalloc_aligned(num * PAGE_SIZE, PAGE_SIZE);

    int heap_start = (USER_HEAP & PAGE_DIRENT_MASK) / PAGE_SIZE;

    for (int i = 0; i < num; i++) {
        int poffset = heap_start + process->num_pages + i;
        pt[poffset] = (added_pages + i*PAGE_SIZE) | PAGE_PRESENT | PAGE_WRITEABLE | PAGE_USER;
    }

    process->num_pages += num;
}

uintptr_t *process_page_dirent_alloc(uintptr_t stack_u, uintptr_t text) {

    uintptr_t *pt = kalloc_aligned(PAGE_DIRENT_SIZE, 4096);
    memset(pt, 0, PAGE_DIRENT_SIZE);

    // 64K stack
    for (int i = 0; i < U_STACK_SIZE / PAGE_DIRENT_SIZE; i++) {
        pt[i] = (stack_u + i*PAGE_DIRENT_SIZE) | PAGE_USER | PAGE_PRESENT | PAGE_WRITEABLE;
    }

    // Map 16k program
    for (int i = 0; i < USER_TEXT_SIZE/PAGE_SIZE; i++) {
        uintptr_t paddr = (uintptr_t)text + i*PAGE_SIZE;
        uintptr_t vaddr = (USER_TEXT_VMA + i*PAGE_SIZE);
        int offset = (vaddr & PAGE_DIRENT_MASK) / PAGE_SIZE;

        pt[offset] = paddr | PAGE_PRESENT | PAGE_WRITEABLE | PAGE_USER;
    }

    // Map user video to 0x1fe000
    int voffset = (USER_VIDEO & PAGE_DIRENT_MASK) / PAGE_DIRENT_SIZE;
    pt[voffset] = 0xb8000 | PAGE_PRESENT | PAGE_WRITEABLE | PAGE_USER;
    pt[voffset + 1] = 0xb9000 | PAGE_PRESENT | PAGE_WRITEABLE | PAGE_USER;

    return pt;
}

void *process_page_table_alloc(uintptr_t stack_u, uintptr_t text) {

    uintptr_t *descriptor = process_page_dirent_alloc(stack_u, text);

    uintptr_t *pdt = kalloc_aligned(4096, 4096);
    memset(pdt, 0, 4096);

    // First 2MB is protected kernel pages
    pdt[0] = (uintptr_t)kernel_pt | PAGE_PRESENT | PAGE_WRITEABLE;

    // Next 2MB is user pages
    pdt[1] = (uintptr_t)descriptor | PAGE_USER | PAGE_PRESENT | PAGE_WRITEABLE;

    // Upper 2 levels just points at our single pdt
    uintptr_t *pdpt = kalloc_aligned(4096, 4096);
    memsetqw(pdpt, (uintptr_t)pdt | PAGE_USER | PAGE_PRESENT | PAGE_WRITEABLE, 512);
    uintptr_t *pml4 = kalloc_aligned(4096, 4096);
    memsetqw(pml4, (uintptr_t)pdpt | PAGE_USER | PAGE_PRESENT | PAGE_WRITEABLE, 512);

    return pml4;
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

    this->runstate = PROCESS_WAIT_READ;
    file_t *file = this->files[fileno];
    if (!file) return -1;

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

    this->runstate = PROCESS_WAIT_WRITE;
    file_t *file = this->files[fileno];
    if (!file) return -1;

    this->runinfo.fileinfo.fileno = fileno;
    this->runinfo.fileinfo.file = file;

    ssize_t result;
    while ( (result = file_write(file, buf, len)) < 0) {
        YIELD();
    }

    this->runstate = PROCESS_RUNNABLE;
    return result;
}
