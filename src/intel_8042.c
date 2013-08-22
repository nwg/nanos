#include "intel_8042.h"
#include "asm.h"
#include <stdbool.h>

uint8_t read_data() {
    return inb(I8042_PORT_DATA);
}

uint8_t read_status() {
    return inb(I8042_PORT_STATUS);
}

bool is_prefix(uint64_t code) {

    switch (code) {
        case 0xE0:
        case 0xE02A:
        case 0xE02AE:
        case 0xE0B7:
        case 0xE0B7E0:
        case 0xE1:
        case 0xE11D:
        case 0xE11D45:
        case 0xE11D45E1:
        case 0xE11D45E19D:
            return true;
    }

    return false;
}

void intel_8042_irq1(bool *got_scancode, uint64_t *scancode_value, int *scancode_length) {
    static uint64_t scan;
    static int length = 0;

    uint8_t data = read_data();
    scan |= data;
    length++;
    if (scancode_value) *scancode_value = scan;
    if (scancode_length) *scancode_length = length;

    if (is_prefix(scan)) {
        if (got_scancode) *got_scancode = false;
        scan <<= 8;
    } else {
        if (got_scancode) *got_scancode = true;
        scan = 0;
        length = 0;
    }
}
