#ifndef __ASM_H__
#define __ASM_H__

#include <stdint.h>
#include "kernel.h"

#define pushq(val) __asm__ ( "pushq %0\n\t" :: "m" (val) );
#define pushqi(val) __asm__ ( "pushq %0\n\t" :: "i" (val) );
#define pushfq() __asm__ ( "pushfq" );

void outb(uint16_t port, uint8_t b);
uint8_t inb(uint16_t port);

#define BOCHS_BRK() __asm__("xchg %bx, %bx\n\t")

// sti lags one instruction, so there is no interrupt race
// condition with the sti/hlt combo
#define HALT() \
    __asm__ __volatile__ ( \
        "sti\n\t" \
        "hlt\n\t" \
        "cli\n\t" \
    );

#define GET_FLAGS(dst) \
    __asm__ __volatile__ ( \
        "pushfq\n\t" \
        "popq %0\n\t" \
        : "=m" (dst) \
        : \
        : \
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

#define EOI() \
    __asm__ __volatile__ ( \
        "movb $0x20, %al\n\t" \
        "outb %al, $0x20\n\t" \
    )

#define PUSHA_SIZE (15*8)

#endif
