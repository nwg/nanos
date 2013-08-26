#include "schedule.h"
#include "kmem.h"
#include "process.h"
#include "ll.h"
#include "memory.h"
#include <stdio.h>
#include "timer.h"
#include "kernel.h"
#include "asm.h"

static node_t *processes = NULL;
static node_t *current = NULL;

#define ll_alloc() ll_alloc_a(kalloc)
#define ll_append_data(ll, data) ll_append_data_a(kalloc, ll, data)
#define P(node) ((process_t*)node->data)

process_t *current_process() {
    return P(current);
}

void schedule_init() {
    processes = ll_alloc();
}

static bool process_node_runnable(node_t *node) {
    return P(node)->runstate == PROCESS_RUNNABLE;
}

static void schedule_wake_sleeper(node_t *node) {
    process_wake(P(node));
}

void schedule_wake_sleepers() {
    ll_foreach(processes, schedule_wake_sleeper);
}

void schedule() {
    while (ll_empty(processes)) {
        HALT();
    }

    if (current) P(current)->current = false;

    current = ll_next_rr_p(processes, current, process_node_runnable);

    while (current == NULL) {
        HALT();
        current = ll_next_rr_p(processes, current, process_node_runnable);
    }

    switch_to_process(P(current));
}

bool process_running() {
    return current != NULL && P(current)->current;
}

void return_from_schedule(system_state_t *state) {
    if (P(current) == NULL) {
        PANIC("return_from_schedule with no current process");
    }

    return_from_process(P(current), state);
}

void add_process(process_t *process) {
    ll_append_data(processes, process);
}

void dump_processes() {
    int len = ll_size(processes);
    kprintf("%d Processes\n", len);
    kprintf("---------\n");
    for (node_t *node = processes->next; node != processes; node = node->next) {
        dump_process(P(node));
    }
}

void remove_process_node(node_t *node) {

    char buf[256];
    process_description(buf, 256, P(current));
    kprintf("Removing process %s\n", buf);

    if (node == current) {
        current = ll_next_rr(processes, node);

        // Last process?
        if (current == node) {
            current = NULL;
        }
    }

    ll_remove(processes, node);
    kfree(P(node));
    kfree(node);
}

void remove_process(process_t *process) {
    node_t *node = ll_find(processes, process);
    remove_process_node(node);
    // dump_processes();
}

void spawn(void *text, int argc, char **argv) {
    process_t *process = process_alloc(text, argc, argv);
    add_process(process);
}
