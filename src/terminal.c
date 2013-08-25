#include "terminal.h"
#include "video.h"
#include "num.h"
#include <memory.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>

static int term_row = 0;
static int term_col = 0;
static int term_top = 0;

#define TERM_COLOR COLOR_GREEN
#define TERM_WIDTH 80
#define TERM_HEIGHT 25

static char term_buf[TERM_HEIGHT][TERM_WIDTH];

void term_init() {
    term_clear();
}

// temporary
#include "asm.h"
void video_set_cursor(int row, int col) {
    uint16_t position = row*80 + col;

    // cursor LOW port to vga INDEX register
    outb(0x3D4, 0x0F);
    outb(0x3D5, (uint8_t)(position & 0xFF));
    // cursor HIGH port to vga INDEX register
    outb(0x3D4, 0x0E);
    outb(0x3D5, (uint8_t)((position >> 8) & 0xFF));
 }

void update_cursor() {
    video_set_cursor(term_row, term_col);
}

void term_clear() {
    term_row = 0; term_col = 0; term_top = 0;

    memset(term_buf, 0, sizeof(term_buf));
    video_clear();
    update_cursor();
}

void term_redraw() {
    for (int i = 0; i < TERM_HEIGHT; i++) {
        int row = (i + term_top) % TERM_HEIGHT;
        char *rowbuf = term_buf[row];
        for (int col = 0; col < TERM_WIDTH; col++) {
            printc(i, col, TERM_COLOR, rowbuf[col]);
        }
    }
}

void term_add_rows(int count) {
    if (count <= 0) return;

    if (term_row + count >= TERM_HEIGHT) {
        for (int row = max(term_row + 1, TERM_HEIGHT); row < term_row + count + 1; row++) {
            char *new_row = term_buf[row % TERM_HEIGHT];
            memset(new_row, 0, TERM_WIDTH);
        }
        term_top = (term_row + count + 1) % TERM_HEIGHT;
        term_redraw();
    }
    term_row += count;

    update_cursor();
}

void term_add_cols(int count) {
    term_col = (term_col + count) % TERM_WIDTH;
    term_add_rows((term_col + count) / TERM_WIDTH);

    update_cursor();
}

static bool is_printable(char c) {
    return true;
}

void term_write_c(char c) {
    switch (c) {
        case '\n':
            term_col = 0; term_add_rows(1);
            break;
        case '\r':
            term_col = 0;
            break;
        case '\t':
            term_add_cols(8);
            break;
        default:
            if (is_printable(c)) {
                term_buf[term_row % TERM_HEIGHT][term_col] = c;
                printc(min(term_row, TERM_HEIGHT - 1), term_col, TERM_COLOR, c);
                term_add_cols(1);
            }

            break;

    }
}

void term_write(const char *str) {
    for (const char *p = str; *p; p++) {
        term_write_c(*p);
    }
}

static char print_buf[256];

int
kvprintf(const char * restrict format, va_list ap) {
    vsnprintf(print_buf, 256, format, ap);
    term_write(print_buf);
    return 0;
}

int
kprintf(const char * restrict format, ...) {
    va_list ap;
    va_start(ap, format);

    int result = kvprintf(format, ap);

    va_end(ap);

    return result;
}
