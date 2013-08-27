#include "keyboard.h"
#include <stdio.h>
#include "term.h"

void keyboard_press(keycode_e code) {
    if (code == KEY_ESC) {
        term_clear();
    } else {
        term_write_c_stdin(keychars(code)[0]);
    }
}

void keyboard_release(keycode_e code) {
    // kprintf("Release %x\n", code);
}

const char* keychars(keycode_e code) {
    switch (code) {
        case KEY_ESC: return "\e\e";

        case KEY_BACKTICK: return "`~";
        case KEY_1: return "1!";
        case KEY_2: return "2@";
        case KEY_3: return "3#";
        case KEY_4: return "4$";
        case KEY_5: return "5%";
        case KEY_6: return "6^";
        case KEY_7: return "7&";
        case KEY_8: return "8*";
        case KEY_9: return "9(";
        case KEY_0: return "0)";
        case KEY_MINUS: return "-_";
        case KEY_EQUALS: return "=_";
        case KEY_BACKSPACE: return "\b\b";

        case KEY_Q: return "qQ";
        case KEY_W: return "wW";
        case KEY_E: return "eE";
        case KEY_R: return "rR";
        case KEY_T: return "tT";
        case KEY_Y: return "yY";
        case KEY_U: return "uU";
        case KEY_I: return "iI";
        case KEY_O: return "oO";
        case KEY_P: return "pP";
        case KEY_BRACKET_LEFT: return "[{";
        case KEY_BRACKET_RIGHT: return "]}";
        case KEY_BACKSLASH: return "\\|";

        case KEY_A: return "aA";
        case KEY_S: return "sS";
        case KEY_D: return "dD";
        case KEY_F: return "fF";
        case KEY_G: return "gG";
        case KEY_H: return "hH";
        case KEY_J: return "jJ";
        case KEY_K: return "kK";
        case KEY_L: return "lL";
        case KEY_SEMICOLON: return ";:";
        case KEY_SINGLE_QUOTE: return "'\"";
        case KEY_ENTER: return "\n\n";

        case KEY_Z: return "zZ";
        case KEY_X: return "xX";
        case KEY_C: return "xX";
        case KEY_V: return "xX";
        case KEY_B: return "xX";
        case KEY_N: return "xX";
        case KEY_M: return "xX";
        case KEY_COMMA: return ",<";
        case KEY_PERIOD: return ".>";
        case KEY_SLASH: return "/?";
        case KEY_SPACE: return "  ";

        default: return "\0\0";
    }
}
