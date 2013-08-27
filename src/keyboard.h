#ifndef __KEYBOARD_H__
#define __KEYBOARD_H__

/* Currently just PC set1 scancodes as right-justified 64-bit integrals  */
typedef enum {
    KEY_UNDEFINED = 0,
    KEY_BACKTICK = 0x29,
    KEY_ESC = 0x01,
    KEY_1 = 0x02,
    KEY_2,
    KEY_3,
    KEY_4,
    KEY_5,
    KEY_6,
    KEY_7,
    KEY_8,
    KEY_9,
    KEY_0,
    KEY_MINUS,
    KEY_EQUALS,
    KEY_BACKSPACE,
    KEY_Q = 0x10,
    KEY_W,
    KEY_E,
    KEY_R,
    KEY_T,
    KEY_Y,
    KEY_U,
    KEY_I,
    KEY_O,
    KEY_P,
    KEY_BRACKET_LEFT,
    KEY_BRACKET_RIGHT,
    KEY_BACKSLASH = 0x2b,
    KEY_A = 0x1e,
    KEY_S,
    KEY_D,
    KEY_F,
    KEY_G,
    KEY_H,
    KEY_J,
    KEY_K,
    KEY_L,
    KEY_SEMICOLON,
    KEY_SINGLE_QUOTE,
    KEY_Z = 0x2c,
    KEY_X,
    KEY_C,
    KEY_V,
    KEY_B,
    KEY_N,
    KEY_M,
    KEY_COMMA,
    KEY_PERIOD,
    KEY_SLASH,
    KEY_RSHIFT,

    KEY_SPACE = 0x39,

    KEY_ENTER = 0x1C,
} keycode_e;

const char *keychars(keycode_e code);

void keyboard_press(keycode_e code);
void keyboard_release(keycode_e code);

#endif
