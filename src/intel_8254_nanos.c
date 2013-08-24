#include "intel_8254_nanos.h"
#include "kernel.h"
#include "timer.h"
#include "intel_8254.h"

void intel_8254_nanos_init() {
    intel_8254_config(
        TIMER_CHAN0,
        TIMER_ACCESS_LOHI,
        TIMER_MODE_CLK_GEN,
        TIMER_FORMAT_BINARY,
        INTEL_100HZ_TICK_DIVISOR
    );

    g_timer_tps = INTEL_8254_HZ;
    g_timer_tps_divisor = INTEL_100HZ_TICK_DIVISOR;
}

void intel_8254_nanos_irq0(system_state_t *state) {
    timer_tick();
}
