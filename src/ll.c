#include "ll.h"
#include "memory.h"

node_t *ll_alloc_a(allocator alloc) {
    node_t *head = alloc(sizeof(node_t));
    head->next = head;
    return head;
}

static inline node_t *_ll_next_rr(node_t *ll, node_t *node) {
    node_t *next = node->next;

    if (next == ll) {
        return ll->next;
    }

    return next;
}

node_t *ll_next_rr(node_t *ll, node_t *node) {
    if (node == NULL) {
        return ll->next;
    }
    return _ll_next_rr(ll, node);
}

static inline node_t *_ll_find_rr_p(node_t *ll, node_t *start, ll_predicate p) {
    if (p(start)) return start;

    for (node_t *node = _ll_next_rr(ll, start); node != start; node = _ll_next_rr(ll, node)) {
        if (p(node)) return node;
    };

    return NULL;
}

node_t *ll_find_rr_p(node_t *ll, node_t *start, ll_predicate p) {
    if (start == NULL) start = ll->next;
    return _ll_find_rr_p(ll, start, p);
}

static inline node_t *_ll_next_rr_p(node_t *ll, node_t *start, ll_predicate p) {
    node_t *found = _ll_find_rr_p(ll, _ll_next_rr(ll, start), p);
    if (found) return found;
    if (p(start)) return start;
    return NULL;
}

node_t *ll_next_rr_p(node_t *ll, node_t *start, ll_predicate p) {
    if (start == NULL) start = ll->next;
    return _ll_next_rr_p(ll, start, p);
}

node_t *ll_first(node_t *ll) {
    if (ll->next == ll) return NULL;
    return ll->next;
}

static inline node_t *_ll_last(node_t *ll) {

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

int ll_size(node_t *ll) {
    int size = 0;
    for (node_t *node = ll->next; node != ll; node = node->next) {
        size++;
    }
    return size;
}

void ll_remove(node_t *ll, node_t *target) {
    for (node_t *node = ll; node->next != ll; node = node->next) {
        if (node->next == target) {
            node->next = node->next->next;
            return;
        }
    }
}

void ll_foreach(node_t *ll, ll_effect effect) {
    for (node_t *node = ll->next; node != ll; node = node->next) {
        effect(node);
    }
}
