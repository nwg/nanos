#include "schedule.h"
#include "kmem.h"
#include "process.h"
#include "ll.h"
#include "memory.h"
#include <stdio.h>

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

void schedule() {
    if (ll_empty(processes)) {
        printf("No more processes to schedule. Spinning.");
        while (true) {}
    }

    if (current == NULL) {
        current = ll_first(processes);
    } else {
        current = ll_next_rr(processes, current);
    }

    char buf[256];
    process_description(buf, 256, P(current));
    switch_to_process(P(current));
}

void return_from_schedule(void *sp) {
    return_from_process(P(current), sp);
}

void add_process(process_t *process) {
    ll_append_data(processes, process);
}

void dump_processes() {
    int len = ll_size(processes);
    printf("%d Processes", len);
    printf("---------");
    for (node_t *node = processes->next; node != processes; node = node->next) {
        dump_process(P(node));
    }
}

void remove_process_node(node_t *node) {

    char buf[256];
    process_description(buf, 256, P(current));
    printf("Removing process %s", buf);

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
}

void spawn(void *text, int argc, char **argv) {
    process_t *process = process_alloc(text, argc, argv);
    add_process(process);
}
