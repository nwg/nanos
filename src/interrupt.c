#include "interrupt.h"
#include "kernel.h"
#include "timer.h"
#include "schedule.h"
#include "syscall.h"
#include "asm.h"
#include "intel_8259.h"
#include "ata_nanos.h"
#include <string.h>

irq_handler irq_handlers[MAX_IRQ];

void interrupt_init() {
    memset(irq_handlers, 0, sizeof(irq_handlers));
}

void enable_irq(int irqno) {
    intel_8259_enable_irq(irqno);
}

void disable_irq(int irqno) {
    intel_8259_disable_irq(irqno);
}

void handle_irq(system_state_t *state, irq_e code) {

    // IRQ from kernel mode => was HLT
    bool is_halt = IS_KERNEL_STATE(state);

    if (!is_halt) {
        // HLT => not in schedule
        return_from_schedule(state);

        if (code == IRQ0) schedule();
    }

    irq_handler handler = irq_handlers[code];
    if (handler) {
        handler(state);
    }

    EOI();
}

void handle_cpu_exception(system_state_t *state, cpu_exception_e code) {
    kprintf("CPU Exception %d received\n", code);

    while (true) {}
}
