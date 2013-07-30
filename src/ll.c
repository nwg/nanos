#include "ll.h"
#include "memory.h"

node_t *ll_alloc_a(allocator alloc) {
    node_t *head = alloc(sizeof(node_t));
    head->next = head;
    return head;
}

node_t *ll_next_rr(node_t *ll, node_t *node) {
    node_t *next = node->next;

    if (next == ll) {
        return ll->next;
    }

    return next;
}

node_t *ll_first(node_t *ll) {
    if (ll->next == ll) {
        return NULL;
    }

    return ll->next;
}

node_t *_ll_last(node_t *ll) {

    node_t *last = ll;

    while (last->next != ll) {
        last = last->next;
    }

    return last;
}

node_t *ll_last(node_t *ll) {
    if (ll->next == ll) {
        return NULL;
    }
    
    return _ll_last(ll);
}

void ll_append(node_t *ll, node_t *node) {
    node_t *last = _ll_last(ll);
    node->next = ll;
    last->next = node;
}

void ll_append_data_a(allocator alloc, node_t *ll, void *data) {
    node_t *node = alloc(sizeof(node_t));
    node->data = data;
    ll_append(ll, node);
}

node_t *ll_find(node_t *ll, void *data) {
    for (node_t *node = ll->next; node != ll; node = node->next) {
        if (node->data == data) return node;
    }

    return NULL;
}
