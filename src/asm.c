#include "asm.h"

void outl(uint16_t port, uint32_t l) {
    __asm__ __volatile__ (
        "movw %0, %%dx\n\t"
        "movl %1, %%eax\n\t"
        "outl %%eax, %%dx\n\t"
        :
        : "m" (port), "m" (l)
        : "dx", "eax"
    );
}

uint32_t inl(uint16_t port) {
    int result;
    __asm__ __volatile__ (
        "movw %0, %%dx\n\t"
        "inl %%dx, %%eax\n\t"
        "movl %%eax, %1\n\t"
        :
        : "m" (port), "m" (result)
        : "dx", "eax"
    );

    return result;
}

void outb(uint16_t port, uint8_t b) {
    __asm__ __volatile__ (
        "movw %0, %%dx\n\t"
        "movb %1, %%al\n\t"
        "outb %%al, %%dx\n\t"
        :
        : "m" (port), "m" (b)
        : "dx", "al"
    );
}

uint8_t inb(uint16_t port) {
    int result;
    __asm__ __volatile__ (
        "movw %0, %%dx\n\t"
        "inb %%dx, %%al\n\t"
        "movb %%al, %1\n\t"
        :
        : "m" (port), "m" (result)
        : "dx", "al"
    );

    return result;
}
