#include "timer.h"
#include "intel_8254.h"
#include "schedule.h"

tick_t g_timer_ticks = 0;
tick_t g_timer_tps;
tick_t g_timer_tps_divisor = 1;

void timer_tick() {
    g_timer_ticks += 1;
}
