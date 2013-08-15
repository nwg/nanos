
#include "video.h"
#include "native.h"

static uint16_t *video = (uint16_t*)USER_VIDEO;

void print(int row, int col, int color, const char *s) {

	for (int i = 0; *s != '\0'; i++, s++) {
		video[row*80 + col + i] = *s | color;
	}
}
