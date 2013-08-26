#ifndef __TERM_H__
#define __TERM_H__

void term_init();
void term_write(const char *str);
void term_write_c(char c);

void term_clear();

int kprintf(const char * restrict format, ...);

#endif
