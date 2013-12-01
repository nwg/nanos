#include "kernel.h"
#include "schedule.h"
#include "kmem.h"
#include "pages.h"
#include "asm.h"
#include "user_vga.h"
#include "timer.h"
#include "intel_8042_nanos.h"
#include "intel_8254_nanos.h"
#include "term.h"
#include "intel_8259_nanos.h"
#include "binary.h"
#include "pci_nanos.h"

termbuf_t *g_termbuf = NULL;

/**
 * Configure first 2MB for initial kernel use
 */
void init_kernel_pages() {

    uintptr_t ****pages = pt_alloc();
    page_flag_e kf = PAGE_PRESENT | PAGE_WRITEABLE;
    pt_map(pages, 0, 0, 2*M, kf, kf, kf, kf);
    pt_map(pages, USER_VIDEO, 0xb8000, 8*K, kf, kf, kf, kf);

	SET_CR3(pages);
}

void spawn_test_programs() {
    // static char *argv1[] = {
    //         "user1", "4", "11",
    // };
    // static int argc1 = sizeof(argv1) / sizeof(char*);

    // spawn((void*)TEST_PROG_PMA + 16*K, argc1, argv1);

    // static char *argv2[] = {
    // "user2", "1", "12",
    // };
    // static int argc2 = sizeof(argv2) / sizeof(char*);

    // spawn((void*)TEST_PROG_PMA + 16*K, argc2, argv2);

    static char *argv3[] = {
        "sh"
    };
    static int argc3 = sizeof(argv3) / sizeof(char*);
    spawn((void*)TEST_PROG_PMA, argc3, argv3);


}

void kernel_init() {
	init_kernel_pages();
    g_termbuf = termbuf_alloc();
    term_init();
    pci_init();
    intel_8259_nanos_init();
    intel_8042_nanos_init();
    intel_8254_nanos_init();
	schedule_init();
	spawn_test_programs();
	schedule();
}

static char print_buf[256];

int kvprintf(const char * restrict format, va_list ap) {
    int len = vsnprintf(print_buf, 256, format, ap);
    term_write(g_term, print_buf, len);
    return 0;
}

int kprintf(const char * restrict format, ...) {
    va_list ap;
    va_start(ap, format);

    int result = kvprintf(format, ap);

    va_end(ap);

    return result;
}
