#ifndef __TERM_H__
#define __TERM_H__

#include <stdbool.h>
#include <stddef.h>
#include "file.h"

extern file_t *g_term_file;

void term_init();
void term_write(const char *str, size_t len);
void term_write_c(char c);
void term_clear();
void term_clear_stdin();
void term_write_c_stdin(char c);
int term_read_stdin(char *dst, size_t maxlen);
file_t *term_get_global_file();

int kprintf(const char * restrict format, ...);

#endif
