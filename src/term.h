#ifndef __TERM_H__
#define __TERM_H__

#include <stdbool.h>
#include <stddef.h>
#include "file.h"
#include "ring.h"

typedef struct {
    int row;
    int col;
    ring_t *ring;
    file_t *file;
} term_t;

extern term_t *g_term;

void term_init();

term_t *term_alloc();
void term_write(term_t *term, const char *str, size_t len);
void term_write_c(term_t *term, char c);
void term_clear(term_t *term);
void term_clear_stdin(term_t *term);
void term_write_c_stdin(term_t *term, char c);
int term_read_stdin(term_t *term, char *dst, size_t maxlen);
file_t *term_get_global_file();

int kprintf(const char * restrict format, ...);

#endif
