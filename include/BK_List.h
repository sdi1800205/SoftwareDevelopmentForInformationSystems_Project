#pragma once

#include "BK_tree.h"
#include "common_types.h"

// δομή λίστας για τα παιδιά της κάθε λέξης του BK_tree
typedef struct BK_list_node {
	BK_node* node;		// δείκτης προς το παιδί
	struct BK_list_node* next;	// δείκτης προς τον κόμβο λίστας με το επόμενο επόμενο παιδί
}*BK_Listnode;

typedef struct BK_list {
	BK_Listnode dummy;
	int size;
}*BK_List;

// Virtual entries: BOF: Begin Of File, EOF: End Of File
#define BK_LIST_BOF (BK_Listnode)0
#define BK_LIST_EOF (BK_Listnode)0

// functions of BK_List

BK_List BK_list_create();       // creates a BK_List
ErrorCode BK_list_destroy(BK_List list);  // destroys the list

BK_Listnode BK_list_first(BK_List list);             // returns the first node
BK_Listnode BK_list_next(BK_Listnode node); // returns the next Listnode of the current one
void BK_list_insert_next(BK_List list, BK_Listnode node, BK_node* new_tree_node);     // creates a new Listnode after the current one, with the new BK_node inside
void BK_list_remove_node(BK_List list, BK_Listnode previous, BK_Listnode node);		// connects previous node with the next of the current and removes the node