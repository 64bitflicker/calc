#include "dlist.h"
#include <stdlib.h>

// 增
Node *dlist_inster(Node **head) {
    Node * new = (Node *)malloc(sizeof(Node));
    if (*head == NULL) {
        new->id = 0;
        new->last = new;
        new->next = new;
        *head = new;
    } else {
        new->id = (*head)->id + 1;

        new->last = (*head)->last;
        new->next = (*head);

        (*head)->last->next = new;
        (*head)->last = new;

        // new->last = head;
        // new->next = head->next;
        // new->next->last = new;
        // head->next = new;
        *head = new;
    }

    return new;
}

// 删
bool dlist_delete(Node *head, int id) {
    Node *tmp = head;
    Node *delete = NULL;
    while(tmp != NULL) {
        if (tmp->id == id) {
            delete = tmp;
            break;
        }
        tmp = tmp->next;
        if (tmp == head) {
            break;
        }
    }

    if (delete) {
        delete->last->next = delete->next;
        delete->next->last = delete->last;
        free(delete);
        return true;
    }

    return false;
}
// 改
Node *dlist_update(Node *head, int id) {
    Node *tmp = head;
    Node *update = NULL;
    while(tmp != NULL) {
        if (tmp->id == id) {
            update = tmp;
            break;
        }
        tmp = tmp->next;
        if (tmp == head) {
            break;
        }
    }

    return update;
}
// 查
const Node *dlist_select(Node *head, int id) {
    return dlist_update(head, id);
}

// 清除
void dlist_clean(Node **head) {
    Node *tmp = *head;
    Node *next = NULL; 
    while(tmp != NULL) {
        if (tmp->expr != NULL) {
            free(tmp->expr);
        }
        next = tmp->next;
        free(tmp);
        tmp = next;
        if (tmp == *head) {
            break;
        }
    }
    *head = NULL;


    // Node *next = NULL; 
    // while((*head) != NULL) {
    //     if ((*head)->expr != NULL) {
    //         free((*head)->expr);
    //     }
    //     next = (*head)->next;
    //     free(*head);
    //     (*head) = next;
    // }

    return;
}

// Node head_g;

