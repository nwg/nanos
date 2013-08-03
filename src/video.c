
#include "video.h"
#include "types.h"

static u16_t *video = (u16_t*)USER_VIDEO;

void print(int row, int col, int color, const char *s) {
	(void)video;
	// video[0] = 0x0169;

	for (int i = 0; *s; i++, s++) {
		video[row*80 + col + i] = *s | color;
	}
}