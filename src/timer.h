#ifndef __TIMER_H__
#define __TIMER_H__

#include <stdint.h>

typedef uintmax_t tick_t;

extern tick_t g_timer_ticks;

// Timer configuration vars
extern tick_t g_timer_tps;            // Integral HZ of timer source
extern tick_t g_timer_tps_divisor;    // Integral divisor of timer source. Set 1 if unneeded.

#define TICK_PERIOD (1.0 / (g_timer_tps / g_timer_tps_divisor))
#define TIMER_GET_TICKS_S(t) (g_timer_tps * (t) / g_timer_tps_divisor)
#define TIMER_GET_TICKS_US(t) (g_timer_tps * (t) / 1000000 / g_timer_tps_divisor)
#define TIMER_TPS (g_timer_tps / g_timer_tps_divisor)

void timer_tick();

#endif
