#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

/* Currently just PC set1 scancodes as right-justified 64-bit integrals  */
typedef enum {
    KEY_UNDEFINED = 0,
    KEY_Q = 0x10,
    KEY_W = 0x11,
    KEY_E = 0x12,
    KEY_R = 0x13,
    KEY_T = 0x14,
    KEY_Y = 0x15,
} keycode_e;

void keyboard_press(keycode_e code);
void keyboard_release(keycode_e code);

#endif
