#ifndef __LL_H__
#define __LL_H__

#include "types.h"
typedef struct node_s {
    struct node_s *next;
    void *data;
} node_t;

typedef BOOL (*ll_predicate)(node_t *node);
typedef void * (*allocator)(size_t size);

node_t *ll_alloc_a(allocator alloc);
node_t *ll_next_rr(node_t *ll, node_t *node);
node_t *ll_first(node_t *ll);
node_t *ll_last(node_t *ll);
void ll_append(node_t *ll, node_t *node);
void ll_append_data_a(allocator alloc, node_t *ll, void *data);
node_t *ll_find(node_t *ll, void *data);
#endif
