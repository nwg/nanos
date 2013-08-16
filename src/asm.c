#include "asm.h"

void outb(uint16_t port, uint8_t b) {
    __asm__ __volatile__ (
        "movw %0, %%dx\n\t"
        "movb %1, %%al\n\t"
        "outb %%al, %%dx\n\t"
        :
        : "m" (port), "m" (b)
        : "al"
    );
}
