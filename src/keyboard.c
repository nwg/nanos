#include "keyboard.h"
#include <stdio.h>

void keyboard_press(keycode_e code) {
    printf("Press %x", code);
}

void keyboard_release(keycode_e code) {
    printf("Release %x", code);
}
