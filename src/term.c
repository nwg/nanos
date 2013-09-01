#include "term.h"
#include "user_vga.h"
#include "num.h"
#include <memory.h>
#include <stdbool.h>
#include <stdarg.h>
#include <stdio.h>
#include "kmem.h"
#include <stdint.h>
#include <string.h>
#include "vga.h"
#include "kernel.h"
#include "file.h"

#define TERM_COLOR COLOR_GREEN
#define TERM_WIDTH 80
#define TERM_HEIGHT 25
#define TERM_BACK_BUFFER 4096

term_t *g_term = NULL;

ssize_t term_write_handler(term_t *this, const char *str, size_t len);

void term_init() {
    g_term = term_alloc();
}

term_t *term_alloc() {
    term_t *this = kalloc(sizeof(term_t));
    memset(this, 0, sizeof(term_t));

    file_t *file = file_alloc();
    file->ctx = this;
    file->write_handler = (file_write_handler)term_write_handler;
    this->file = file;

    ring_t *ring = ring_alloc(kalloc, TERM_BACK_BUFFER, TERM_WIDTH);
    ring_add_row(ring);
    this->ring = ring;

    term_clear(this);


    return this;
}

void update_cursor(term_t *this) {
    vga_set_cursor(min(this->row, TERM_HEIGHT - 1), this->col);
}

void term_clear(term_t *this) {
    this->row = 0; this->col = 0;

    for (int i = 0; i < this->ring->current_length; i++) {
        void *row = ring_get_row(this->ring, i);
        if (row) {
            memset(row, VIDEO_CLEAR_CHAR, TERM_WIDTH);
        }
    }
    video_clear(TERM_COLOR);
    update_cursor(this);
}

void term_redraw(term_t *this) {
    for (int row = 0; row < TERM_HEIGHT; row++) {
        int back_idx = -TERM_HEIGHT + row;
        char *rowbuf = ring_get_row(this->ring, back_idx);
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

void term_add_rows(term_t *this, int count) {
    if (count <= 0) return;

    for (int i = 0; i < count; i++) {
        void *row = ring_add_row(this->ring);
        memset(row, VIDEO_CLEAR_CHAR, TERM_WIDTH);
    }
    this->row += count;

    if (this->row >= TERM_HEIGHT) {
        term_redraw(this);
    }

    update_cursor(this);
}

void term_add_cols(term_t *this, int count) {
    this->col = (this->col + count) % TERM_WIDTH;
    term_add_rows(this, (this->col + count) / TERM_WIDTH);

    update_cursor(this);
}

static bool is_printable(char c) {
    return true;
}

int term_current_screen_row(term_t *this) {
    return min(this->ring->current_length - 1, TERM_HEIGHT - 1);
}

char *term_last_row(term_t *this) {
    return ring_get_row(this->ring, -1);
}

void term_write_printable(term_t *this, char c) {
    if (is_printable(c)) {
        char *row = term_last_row(this);
        if (!row) {
            PANIC("Write with no row");
        }

        row[this->col] = c;
        printc(term_current_screen_row(this), this->col, TERM_COLOR, c);
        term_add_cols(this, 1);
    }
}

void term_backspace(term_t *this, int count) {
    char *row = term_last_row(this);
    while (this->col > 0 && count > 0) {
        this->col--; count--;
        row[this->col] = VIDEO_CLEAR_CHAR;
        printc(term_current_screen_row(this), this->col, TERM_COLOR, VIDEO_CLEAR_CHAR);
    }
    update_cursor(this);
}

void term_write_c(term_t *this, char c) {
    switch (c) {
        case '\n':
            this->col = 0; term_add_rows(this, 1);
            break;
        case '\r':
            this->col = 0;
            break;
        case '\t':
            term_add_cols(this, 8);
            break;
        case '\b':
            term_backspace(this, 1);
            break;
        default:
            term_write_printable(this, c);
            break;

    }
}

void term_write(term_t *this, const char *str, size_t len) {
    int i = 0;
    for (const char *p = str; *p && i < len; p++, i++) {
        term_write_c(this, *p);
    }
}

ssize_t term_write_handler(term_t *this, const char *str, size_t len) {
    term_write(this, str, len);
    return len;
}
