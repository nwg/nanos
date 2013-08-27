#ifndef __INTERUPT_H__
#define __INTERUPT_H__

#include "kernel.h"

typedef enum {
    CPU_EXC0,
    CPU_EXC1,
    CPU_EXC2,
    CPU_EXC3,
    CPU_EXC4,
    CPU_EXC5,
    CPU_EXC6,
    CPU_EXC7,
    CPU_EXC8,
    CPU_EXC9,
    CPU_EXC10,
    CPU_EXC11,
    CPU_EXC12,
    CPU_EXC13,
    CPU_EXC14,
    CPU_EXC15,
    CPU_EXC16,
    CPU_EXC17,
    CPU_EXC18,
    CPU_EXC19,
    CPU_EXC20,
    CPU_EXC21,
    CPU_EXC22,
    CPU_EXC23,
    CPU_EXC24,
    CPU_EXC25,
    CPU_EXC26,
    CPU_EXC27,
    CPU_EXC28,
    CPU_EXC29,
    CPU_EXC30,
    CPU_EXC31,
} cpu_exception_e;

typedef enum {
    IRQ0,
    IRQ1,
    IRQ2,
    IRQ3,
    IRQ4,
    IRQ5,
    IRQ6,
    IRQ7,
    IRQ8,
    IRQ9,
    IRQ10,
    IRQ11,
    IRQ12,
    IRQ13,
    IRQ14,
    IRQ15,

} irq_e;

void handle_irq(system_state_t *state, irq_e code);
void handle_cpu_exception(system_state_t *state, cpu_exception_e code);

#endif
