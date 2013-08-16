#ifndef __INTEL_8254_H__
#define __INTEL_8254_H__

/*
 * Intel 8254 Programmable Interval Timer
 *
 * http://download.intel.com/design/archives/periphrl/docs/23124406.pdf
 * http://wiki.osdev.org/Programmable_Interval_Timer
 * http://www.intel.com/design/archives/periphrl/docs/7203.htm
 * http://www.intel.com/design/archives/periphrl/docs/7178.htm
 */

#include "binary.h"
#include <stdint.h>

#define INTEL_8254_HZ 1193182
#define TIMER_PORT_CHAN0 0x40
#define TIMER_PORT_COMMAND 0x43

typedef enum {
    TIMER_CHAN0,
    TIMER_CHAN1,
    TIMER_CHAN2,
    TIMER_READBACK,
} timer_channel_e;

typedef enum {
    TIMER_LATCH_COUNT,
    TIMER_ACCESS_LO,
    TIMER_ACCESS_HI,
    TIMER_ACCESS_LOHI,
} timer_access_e;

typedef enum {
    TIMER_MODE_ONESHOT,         // Mode 0 - Interrupt On Terminal Count
    TIMER_MODE_ONESHOT_GATE,    // Mode 1 - Hardware Re-triggerable One-shot
    TIMER_MODE_BLIP_GEN,        // Mode 2 - Rate Generator
    TIMER_MODE_CLK_GEN,         // Mode 3 - Square Wave Generator
    TIMER_MODE_ONEBLIP,         // Mode 4 - Software Triggered Strobe
    TIMER_MODE_ONEBLIP_GATE,    // Mode 5 - Hardware Triggered Strobe
} timer_mode_e;

typedef enum {
    TIMER_FORMAT_BINARY,
    TIMER_FORMAT_BCD
} timer_format_e;

#define TIMER_COMMAND_BYTE(channel, access, mode, format) \
    ( (channel & b11) << 6 | (access & b11) << 4 | \
        (mode & b111) << 1 | (format & 1) )


void intel_8254_config(
    timer_channel_e channel,
    timer_access_e access,
    timer_mode_e mode,
    timer_format_e format,
    uint16_t count
);


#endif
