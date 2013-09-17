#ifndef __NANOS_USER_H__
#define __NANOS_USER_H__

#include "user.h"
#include <stdint.h>

#define SH_TEXT ((void*)TEST_PROG_PMA)
#define RACE_TEXT ((void*)(TEST_PROG_PMA + USER_TEXT_SIZE))
#define LINE_TEXT ((void*)(TEST_PROG_PMA + 2*USER_TEXT_SIZE))

int spawn(void *text, int argc, char **argv);
uint64_t get_ticks();

#endif
