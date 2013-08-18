#ifndef __TIMER_H__
#define __TIMER_H__

#include "intel_8254.h"
#include <stdint.h>

uint64_t g_timer_ticks;

#define INTEL_100HZ_TICK_DIVISOR 11932
#define TICK_PERIOD (1.0 / (INTEL_8254_HZ / INTEL_100HZ_TICK_DIVISOR))
#define TIMER_GET_TICKS_S(t) (INTEL_8254_HZ * (t) / INTEL_100HZ_TICK_DIVISOR)
#define TIMER_GET_TICKS_US(t) (INTEL_8254_HZ * (t) / 1000000 / INTEL_100HZ_TICK_DIVISOR)

void init_timer();
void timer_tick();

#endif
