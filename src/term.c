#include "term.h"
#include "user_vga.h"
#include "num.h"
#include <memory.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include "ring.h"
#include "kmem.h"
#include <stdint.h>
#include <string.h>

term_newline_handler g_term_newline_handler = NULL;

static int term_row = 0;
static int term_col = 0;

#define TERM_COLOR COLOR_GREEN
#define TERM_WIDTH 80
#define TERM_HEIGHT 25
#define TERM_BACK_BUFFER 4096
#define TERM_STDIN_BUFSIZE 1024
#define TERM_STDIN_LINESIZE 512

static ring_t *ring;
static char stdin_buf[TERM_STDIN_BUFSIZE];
static int stdin_buf_offset = 0;
static int stdin_buf_last_newline = 0;

void term_init() {
    ring = ring_alloc(kalloc, TERM_BACK_BUFFER, TERM_WIDTH);
    ring_add_row(ring);
    term_clear();
    term_clear_stdin();
}

void term_clear_stdin() {
    memset(stdin_buf, 0, sizeof(stdin_buf));
    stdin_buf_offset = 0;
    stdin_buf_last_newline = 0;
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
    video_set_cursor(min(term_row, TERM_HEIGHT - 1), term_col);
}

void term_clear() {
    term_row = 0; term_col = 0;

    for (int i = 0; i < ring->current_length; i++) {
        void *row = ring_get_row(ring, i);
        if (row) {
            memset(row, VIDEO_CLEAR_CHAR, TERM_WIDTH);
        }
    }
    video_clear(TERM_COLOR);
    update_cursor();
}

void term_redraw() {
    for (int row = 0; row < TERM_HEIGHT; row++) {
        int back_idx = -TERM_HEIGHT + row;
        char *rowbuf = ring_get_row(ring, back_idx);
        if (rowbuf) {
            for (int col = 0; col < TERM_WIDTH; col++) {
                printc(row, col, TERM_COLOR, rowbuf[col]);
            }
        } else {
            for (int col = 0; col < TERM_WIDTH; col++) {
                printc(row, col, TERM_COLOR, VIDEO_CLEAR_CHAR);
            }
        }
    }
}

void term_add_rows(int count) {
    if (count <= 0) return;

    for (int i = 0; i < count; i++) {
        void *row = ring_add_row(ring);
        memset(row, VIDEO_CLEAR_CHAR, sizeof(row));
    }
    term_row += count;

    if (term_row >= TERM_HEIGHT) {
        term_redraw();
    }

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

int term_current_screen_row() {
    return min(ring->current_length - 1, TERM_HEIGHT - 1);
}

char *term_last_row() {
    return ring_get_row(ring, -1);
}

void term_write_printable(char c) {
    if (is_printable(c)) {
        char *row = term_last_row();
        if (!row) {
            PANIC("Write with no row");
        }

        row[term_col] = c;
        printc(term_current_screen_row(), term_col, TERM_COLOR, c);
        term_add_cols(1);
    }
}

void term_backspace(int count) {
    char *row = term_last_row();
    while (term_col > 0 && count > 0) {
        term_col--; count--;
        row[term_col] = VIDEO_CLEAR_CHAR;
        printc(term_current_screen_row(), term_col, TERM_COLOR, VIDEO_CLEAR_CHAR);
    }
    update_cursor();
}

static inline void call_newline_handler() {
    if (g_term_newline_handler) {
        if (g_term_newline_handler(stdin_buf)) {
            term_clear_stdin();
        }
    }
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
        case '\b':
            term_backspace(1);
            break;
        default:
            term_write_printable(c);
            break;

    }
}

int term_read_stdin(char *dst, size_t maxlen) {
    int size = min(maxlen, stdin_buf_last_newline);
    strncpy(dst, stdin_buf, size);

    if (size < stdin_buf_offset) {
        memcpy(stdin_buf, &stdin_buf[stdin_buf_offset], stdin_buf_offset - stdin_buf_last_newline);
    } else {
        term_clear_stdin();
    }

    return size;
}

void term_write_c_stdin(char c) {
    if (stdin_buf_offset >= TERM_STDIN_BUFSIZE - 1)
        return;

    // save char for last newline
    if (stdin_buf_offset == TERM_STDIN_BUFSIZE - 2 && c != '\n')
        return;

    term_write_c(c);

    stdin_buf[stdin_buf_offset++] = c;

    if (c == '\n') {
        stdin_buf_last_newline = stdin_buf_offset;
        call_newline_handler();
    }
}

void term_write(const char *str, size_t len) {
    int i = 0;
    for (const char *p = str; *p && i < len; p++, i++) {
        term_write_c(*p);
    }
}
