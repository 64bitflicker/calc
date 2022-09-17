#ifndef __DLIST_H__
#define __DLIST_H__

#include <stdint.h>

#include "common.h"

typedef struct Node_ {
    int id;
    int32_t type;
    char *expr;
    struct Node_ *last;
    struct Node_ *next;
} Node;

// 增
Node *dlist_inster(Node **head);
// 删
bool dlist_delete(Node *head, int id);
// 改
Node *dlist_update(Node *head, int id);
// 查
const Node *dlist_select(Node *head, int id);
// 清除
void dlist_clean(Node **head);

#endif // __DLIST_H__
