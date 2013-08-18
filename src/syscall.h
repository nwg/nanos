#ifndef __SYSCALL_H__
#define __SYSCALL_H__

typedef enum {
	SYSCALL_TEST,
    SYSCALL_EXIT,
    SYSCALL_ADD_PAGES,
    SYSCALL_SLEEP,
} syscall_code_t;

#define syscall0(code) \
    __asm__ __volatile__ ( \
        "movq %0, %%rax\n\t" \
        "int $48\n\t" \
        :  \
        : "i" (code) \
        : "rax" \
    )

#define syscall1i(code, arg1) \
    __asm__ __volatile__ ( \
        "movq %0, %%rax\n\t" \
        "movq %1, %%rbx\n\t" \
        "int $48\n\t" \
        :  \
        : "i" (code), "i" (arg1) \
        : "rax", "rbx" \
    )

#define syscall1m(code, arg1) \
    __asm__ __volatile__ ( \
        "movq %0, %%rax\n\t" \
        "movq %1, %%rbx\n\t" \
        "int $48\n\t" \
        :  \
        : "i" (code), "m" (arg1) \
        : "rax", "rbx" \
    )

#define sys_test() syscall0(SYSCALL_TEST)
#define sys_exit() syscall0(SYSCALL_EXIT)
#define sys_add_pages(count) syscall1i(SYSCALL_ADD_PAGES, count)
#define sys_sleep(useconds) syscall1m(SYSCALL_SLEEP, useconds)

#endif
