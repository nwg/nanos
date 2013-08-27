#ifndef __VGA_H__
#define __VGA_H__

/* http://www.osdever.net/documents/vga_ports.txt */

#define VGA_CRT_BASE_MONO (0x3b4)
#define VGA_CRT_BASE_COLOR (0x3d4)
#define VGA_CRT_BASE VGA_CRT_BASE_COLOR
#define VGA_CRT_INDEX VGA_CRT_BASE
#define VGA_CRT_DATA (VGA_CRT_BASE+1)

typedef enum {
    VGA_PORT_CURSOR_SCAN_START = 0xa,
    VGA_PORT_CURSOR_SCAN_END = 0xb,     // Cursor scanlines before and after (set shape)
    VGA_PORT_CURSOR_HI = 0xe,
    VGA_PORT_CURSOR_LO = 0xf,           // Cursor linear offset start position
} vga_port_index_e;

void vga_set_cursor(int row, int col);

#endif
