#ifndef __INTERUPT_H__
#define __INTERUPT_H__

typedef enum {
	INTERRUPT_TEST,
	INTERRUPT_ADD_PAGES,
} interrupt_code_t;

#define sys_test() \
    __asm__ __volatile__ ( \
        "movq %0, %%rax\n\t" \
        "int $48\n\t" \
        :  \
        : "i" (INTERRUPT_TEST) \
        : "rax" \
    )

#define sys_add_pages(count) \
    __asm__ __volatile__ ( \
        "movq %0, %%rax\n\t" \
        "movq %1, %%rbx\n\t" \
        "int $48\n\t" \
        :  \
        : "i" (INTERRUPT_ADD_PAGES), "i" (count) \
        : "rax", "rbx" \
    )

#endif