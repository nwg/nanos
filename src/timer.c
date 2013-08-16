#include "timer.h"
#include "intel_8254.h"

uint64_t g_timer_ticks = 0;

void init_timer() {

    intel_8254_config(
        TIMER_CHAN0,
        TIMER_ACCESS_LOHI,
        TIMER_MODE_CLK_GEN,
        TIMER_FORMAT_BINARY,
        INTEL_100HZ_TICK_DIVISOR
    );

}

void timer_tick() {
    g_timer_ticks += 1;
}
