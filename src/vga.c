#include "vga.h"
#include "asm.h"

void vga_set_cursor(int row, int col) {
    uint16_t position = row*80 + col;

    outb(VGA_CRT_INDEX, VGA_PORT_CURSOR_LO);
    outb(VGA_CRT_DATA, (uint8_t)(position & 0xFF));

    outb(VGA_CRT_INDEX, VGA_PORT_CURSOR_HI);
    outb(VGA_CRT_DATA, (uint8_t)((position >> 8) & 0xFF));

 }

