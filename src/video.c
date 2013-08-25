
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

void video_clear() {
    memset(video, 0, VIDEO_SIZE);
}

