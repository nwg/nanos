#include "ll.h"

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

static inline node_t *_ll_find_rr_p(node_t *ll, node_t *start, ll_predicate p, void *ctx) {
    if (p(start, ctx)) return start;

    for (node_t *node = _ll_next_rr(ll, start); node != start; node = _ll_next_rr(ll, node)) {
        if (p(node, ctx)) return node;
    };

    return NULL;
}

node_t *ll_find_rr_p(node_t *ll, node_t *start, ll_predicate p, void *ctx) {
    if (start == NULL) start = ll->next;
    return _ll_find_rr_p(ll, start, p, ctx);
}

static inline node_t *_ll_next_rr_p(node_t *ll, node_t *start, ll_predicate p, void *ctx) {
    node_t *found = _ll_find_rr_p(ll, _ll_next_rr(ll, start), p, ctx);
    if (found) return found;
    if (p(start, ctx)) return start;
    return NULL;
}

node_t *ll_next_rr_p(node_t *ll, node_t *start, ll_predicate p, void *ctx) {
    if (start == NULL) start = ll->next;
    return _ll_next_rr_p(ll, start, p, ctx);
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

void ll_delete_a(deallocator dealloc, node_t *ll, node_t *node) {
    ll_remove(ll, node);
    dealloc(node);
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

node_t *ll_find_p(node_t *ll, ll_predicate p, void *ctx) {
    for (node_t *node = ll->next; node != ll; node = node->next) {
        if (p(node, ctx)) return node;
    }

    return NULL;
}

void *ll_find_data_p(node_t *ll, ll_data_predicate p, void *ctx) {
    for (node_t *node = ll->next; node != ll; node = node->next) {
        if (p(node->data, ctx)) return node->data;
    }

    return NULL;
}

bool ll_all(node_t *ll, ll_predicate p, void *ctx) {
    for (node_t *node = ll->next; node != ll; node = node->next) {
        if (!p(node, ctx)) return false;
    }

    return true;
}
