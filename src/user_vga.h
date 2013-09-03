#ifndef __USER_VIDEO_H__
#define __USER_VIDEO_H__

#include <stdint.h>
#include "user.h"

#define VIDEO_START_PHYS 0xb8000
#define VIDEO_SIZE (80*25*2)
#define VIDEO_CLEAR_CHAR (' ')
#define COLOR_BLUE (0x01 << 8)
#define COLOR_GREEN (0x02 << 8)
#define COLOR_RED (0x04 << 8)

void printc(int row, int col, int color, char c);
void print(int row, int col, int color, const char *s);
void video_clear(uint16_t clear_color);

#endif
