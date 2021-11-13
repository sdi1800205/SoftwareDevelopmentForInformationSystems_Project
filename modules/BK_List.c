#include <stdlib.h>

#include "BK_List.h"

// extra functions

static BK_Listnode create_node() {
    BK_Listnode node = malloc(sizeof(struct BK_list_node));
    node->node = NULL;
    node->next = BK_LIST_EOF;
    return node;
}

// BK_List.h functions

BK_List BK_list_create() {
    // memory allocation
    BK_List list = malloc(sizeof(struct BK_list));
    list->dummy = create_node();

    // initialize rest of the list
    list->size = 0;

    return list;
}

ErrorCode BK_list_destroy(BK_List list) {
    if (list == NULL)
        return EC_FAIL;

    BK_Listnode node = list->dummy;
    while (node != BK_LIST_EOF) {
        BK_Listnode next = node->next;
        free(node);
        node = next;
    }
    free(list);
    return EC_SUCCESS;
}

BK_Listnode BK_list_first(BK_List list) {
    if (list->size == 0)
        return BK_LIST_EOF;
    return list->dummy->next;
}

BK_Listnode BK_list_next(BK_Listnode node) {
    if (node == BK_LIST_EOF)
        return BK_LIST_EOF;
    return node->next;
}

void BK_list_insert_next(BK_List list, BK_Listnode node, BK_node* new_tree_node) {
    if (node == BK_LIST_BOF)    // if we want to put a node at the beginning of the list
        node = list->dummy;
    BK_Listnode new_list_node = create_node();  // create the new list_node
    new_list_node->node = new_tree_node;        // the new list_node points to the BK_node that was sent
    new_list_node->next = node->next;           // the new list_node points to the next list_node of current list_node
    node->next = new_list_node;                 // the current list_node points to the new list_node that has been created

    (list->size)++;
}

void BK_list_remove_node(BK_List list, BK_Listnode previous, BK_Listnode node) {
    if (previous == BK_LIST_BOF)    // case which removes the first node
        previous = list->dummy;
    previous->next = node->next;
    free(node);
}