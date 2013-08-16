#include "intel_8254.h"
#include "asm.h"
#include <stdint.h>

void intel_8254_config(
    timer_channel_e channel,
    timer_access_e access,
    timer_mode_e mode,
    timer_format_e format,
    uint16_t count
) {

    uint8_t cmd = TIMER_COMMAND_BYTE(channel, access, mode, format);

    outb(TIMER_PORT_COMMAND, cmd);
    outb(TIMER_PORT_CHAN0, count & 0xFF);
    outb(TIMER_PORT_CHAN0, (count >> 8) & 0xFF);
}
