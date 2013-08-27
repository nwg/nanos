#include "interrupt.h"
#include "kernel.h"
#include "timer.h"
#include "schedule.h"
#include "intel_8042_nanos.h"
#include "intel_8254_nanos.h"
#include "syscall.h"
#include "asm.h"

void handle_irq(system_state_t *state, irq_e code) {
    if (IS_USER_STATE(state)) {
        return_from_schedule(state);
    }

    switch (code) {
        case IRQ0:

            intel_8254_nanos_irq0();

            if (IS_USER_STATE(state)) {
                schedule();
            }

            break;

        case IRQ1:
            intel_8042_nanos_handle_irq1();
            break;

        default:
            break;
    }

    EOI();
}

void handle_cpu_exception(system_state_t *state, cpu_exception_e code) {
    kprintf("CPU Exception %d received\n", code);

    while (true) {}
}
