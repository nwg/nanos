#ifndef __ASM_H__
#define __ASM_H__

extern void *tss64_sp;

#define pushq(val) __asm__ ( "pushq %0\n\t" :: "m" (val) );
#define pushqi(val) __asm__ ( "pushq %0\n\t" :: "i" (val) );
#define pushfq() __asm__ ( "pushfq" );

#define BOCHS_BRK() __asm__("xchg %bx, %bx\n\t")

#define GET_FLAGS(dst) \
    __asm__ __volatile__ ( \
        "pushfq\n\t" \
        "popq %0\n\t" \
        : "=m" (dst) \
        : \
        : \
    )


#define SET_TSS_RSP(src) \
    __asm__ __volatile__ ( \
        "movq %0, %%rax\n\t" \
        "movq %%rax, %1\n\t" \
        : \
        : "m" (src), "m" (tss64_sp) \
        : "rax" \
    )

#define GET_RSP(dst) \
    __asm__ __volatile__ ( \
        "movq %%rsp, %0\n\t" \
        : "=m" (dst) \
    )

#define SET_RSP(src) \
    __asm__ __volatile__ ( \
        "movq %0, %%rsp\n\t" \
        : \
        : "m" (src) \
    )

#define GET_CR3(dst) \
    __asm__ __volatile__ ( \
        "movq %%cr3, %%rax\n\t" \
        "movq %%rax, %0\n\t" \
        : "=m" (dst) \
        : \
        : "rax" \
    )

#define SET_CR3(src) \
    __asm__ __volatile__ ( \
        "movq %0, %%rax\n\t" \
        "movq %%rax, %%cr3\n\t" \
        : \
        : "m" (src) \
        : "rax" \
    )

#define SET_ALL_SEGMENTS(val) \
    __asm__ __volatile__ ( \
        "movq %0, %%rax\n\t" \
        "mov %%ax, %%ds\n\t" \
        "mov %%ax, %%es\n\t" \
        "mov %%ax, %%fs\n\t" \
        "mov %%ax, %%gs\n\t" \
        : \
        : "i" (val) \
        : "rax" \
    )

#define RETURN_TO_PROCESS() \
    __asm__ ("jmp return_to_process\n\t");

#define PUSHA_NUM (7)
#define PUSHA_SIZE (PUSHA_NUM*8)

#endif
