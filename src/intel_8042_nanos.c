#include "intel_8042_nanos.h"
#include "intel_8042.h"
#include "keyboard.h"
#include "interrupt.h"

static void convert_scancode(uint64_t scancode, int length, keycode_e *keycode, bool *is_release) {
    if (length > 2) {
        *keycode = KEY_UNDEFINED;
        *is_release = false;
        return;
    }

    *is_release = scancode & I8042_RELEASE_BIT;
    scancode &= ~I8042_RELEASE_BIT;
    *keycode = (keycode_e)scancode;
}

void intel_8042_nanos_init() {
    enable_irq(1);
    irq_handlers[IRQ1] = intel_8042_nanos_handle_irq1;
}

void intel_8042_nanos_handle_irq1() {
    bool got_scancode; uint64_t scancode; int scancode_length;
    intel_8042_irq1(&got_scancode, &scancode, &scancode_length);

    if (got_scancode) {
        keycode_e keycode; bool is_release;
        convert_scancode(scancode, scancode_length, &keycode, &is_release);

        if (keycode == KEY_UNDEFINED)
            return;

        if (is_release) {
            keyboard_release(keycode);
        } else {
            keyboard_press(keycode);
        }
    }
}
