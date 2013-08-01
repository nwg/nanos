#include "memory.h"
#include "process.h"
#include "kmem.h"
#include "pages.h"
#include "asm.h"
#include "stack.h"
#include "video.h"
#include "kernel.h"

/* Our basic pagetable entry with identity from 0 to 0xc, 16k text + data, and video memory */
uintptr_t *process_page_dirent_alloc(uintptr_t stack_u, uintptr_t text) {

    uintptr_t *pt = kalloc_aligned(PAGE_DIRENT_SIZE, 4096);
    memset(pt, 0, PAGE_DIRENT_SIZE);

    // 64K stack
    for (int i = 0; i < (64*KB) / PAGE_DIRENT_SIZE; i++) {
        pt[i] = (USER_TEXT_VMA + 8*KB) | PAGE_USER | PAGE_PRESENT | PAGE_WRITEABLE;
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

void configure_initial_stack(process_t *process) {
    stackptr_t k = STACK(process->stack_k, K_STACK_SIZE);
    u64_t flags;

    STACK_PUSH(k, 0x20 | 3);
    STACK_PUSH(k, USER_STACK_START);
    GET_FLAGS(flags);
    flags |= 0x200; // Force reallow interrupts for the initial iret
    STACK_PUSH(k, flags);
    STACK_PUSH(k, 0x18 | 3);
    STACK_PUSH(k, (uintptr_t)USER_TEXT_VMA);

    STACK_INC(k, PUSHA_NUM);
    memset(k, 0, PUSHA_SIZE);

    process->saved_sp = k;
}

process_t *process_alloc(void *text) {
    process_t *process = (process_t*)kalloc(sizeof(process_t));
    process->text = text;
    process->stack_k = kalloc(K_STACK_SIZE);
    process->stack_u = kalloc(U_STACK_SIZE);
    process->pages = process_page_table_alloc((uintptr_t)process->stack_u, (uintptr_t)text);

    configure_initial_stack(process);

    return process;
}

/* Perform context switch and enter user mode to run given process */
void switch_to_process(process_t *process) {
    SET_CR3(process->pages);

    // Kernel stack base address (for interrupts)
    stackptr_t k = STACK(process->stack_k, K_STACK_SIZE);
    SET_TSS_RSP(k);

    // Set up segments, including data segment (fourth GDT entry in kernel.asm)
    SET_ALL_SEGMENTS(0x20 | 3);

    SET_RSP(process->saved_sp);
    RETURN_TO_PROCESS();

    while (1) {}
}

/*
 * This function must be called when returning to the kernel after
 * running a process, e.g. via a timer interrupt

 * sp is the IRETtable kernel-side stack pointer for the process
 */
void return_from_process(process_t *process, void *sp) {
    process->saved_sp = sp;
}
