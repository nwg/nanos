#include "schedule.h"
#include "kmem.h"
#include "process.h"
#include "ll.h"
#include "memory.h"

static node_t *processes = NULL;
static node_t *current = NULL;

#define ll_alloc() ll_alloc_a(kalloc)
#define ll_append_data(ll, data) ll_append_data_a(kalloc, ll, data)
#define P(node) ((process_t*)node->data)

void schedule_init() {
    processes = ll_alloc();
}

void schedule() {

    node_t *next;
    if (current == NULL) {
        next = ll_first(processes);
    } else {
        next = ll_next_rr(processes, current);
    }

    current = next;
    switch_to_process(P(next));
}

void return_from_schedule(void *sp) {
    return_from_process(P(current), sp);
}

void add_process(process_t *process) {
    ll_append_data(processes, process);
}

void spawn(void *text) {
    process_t *process = process_alloc(text);
    add_process(process);
}

void somefunc() {

}