#ifndef __INTEL_8042_H__
#define __INTEL_8042_H__

/*
 * Intel 8042
 *
 * This is the PC keyboard interface present x86 systems.
 *
 * The interface is overloaded with functionality for setting
 *  A20 address pin and system reset. 8042 not standalone
 *  in modern systems but interface remains the same
 *
 * Datasheet for an original 8042
 * http://www.diakom.ru/el/elfirms/datashts/Smsc/42w11.pdf
 *
 * Scancode listing
 * http://www.win.tue.nl/~aeb/linux/kbd/scancodes-1.html#ss1.1
 */

#include <stdint.h>
#include <stdbool.h>

#define I8042_PORT_DATA 0x60      // IO
#define I8042_PORT_STATUS 0x64    // I
#define I8042_PORT_COMMAND 0x64   // O

#define I8042_RELEASE_BIT 0x80

typedef enum {
    I8042_STATUS_OUTPUT_FULL = 1 << 0,
    I8042_STATUS_INPUT_FULL = 1 << 1,
    I8042_STATUS_SYSTEM_POST_OK = 1 << 2,
    I8042_STATUS_WRITE_WILL_TARGET_DEVICE_NOT_CONTROLLER = 1 << 3,
    I8042_STATUS_UNDEFINED1 = 1 << 4,
    I8042_STATUS_UNDEFINED2 = 1 << 5,
    I8042_STATUS_ERROR_TIMEOUT = 1 << 6,  // Device not responding, => data = 0xFF
    I8042_STATUS_ERROR_PARITY = 1 << 7,   // Scancode parity incorrect (hardware tries 2X), => data = 0xFF
} i8042_status_e;

uint8_t read_data();
uint8_t read_status();

void intel_8042_irq1(bool *got_scancode, uint64_t *scancode_value, int *scancode_length);

#endif
