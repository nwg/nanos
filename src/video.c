
#include "video.h"
#include "native.h"
#include <memory.h>

static uint16_t *video = (uint16_t*)USER_VIDEO;

void printc(int row, int col, int color, char c) {
    video[row*80 + col] = c | color;
}

void print(int row, int col, int color, const char *s) {

	for (int i = 0; *s != '\0'; i++, s++) {
		video[row*80 + col + i] = *s | color;
	}
}

void video_clear(uint16_t clear_color) {
    uint16_t clear_word = ' ' | clear_color;
    for (int row = 0; row < 25; row++) {
        for (int col = 0; col < 80; col++) {
            video[row*80+col] = clear_word;
        }
    }
}

