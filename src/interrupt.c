#include "interrupt.h"
#include "kernel.h"
#include "timer.h"
#include "schedule.h"
#include "intel_8042_nanos.h"
#include "intel_8254_nanos.h"
#include "syscall.h"

void handle_interrupt(interrupt_e code, system_state_t *state) {
    if (IS_USER_STATE(state)) {
        return_from_schedule(state);
    }

    if (IS_CPU_INTERRUPT(code)) {
        kprintf("CPU Exception %d received\n", code);
        return;
    }

    switch (code) {
        case INTERRUPT_IRQ0:

            intel_8254_nanos_irq0();

            if (IS_USER_STATE(state)) {
                schedule();
            }

            break;
        case INTERRUPT_IRQ1:
            intel_8042_nanos_handle_irq1();
            break;
        case INTERRUPT_SOFTWARE:
            handle_syscall(state);
            break;
        default:
            break;
    }
}
