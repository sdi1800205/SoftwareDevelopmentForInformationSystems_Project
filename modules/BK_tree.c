#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "BK_tree.h"
#include "BK_List.h"

//////////// structs of module ////////////////

struct  BK_tree {
    BK_node* root;		// root of tree/index
	MatchType match_type;		// type that matches words
	DestroyFunc destroy;		// function called when an entry is going to be deleted from the tree
};

struct BK_node {
	entry *centry;
	BK_List children;

	int dist;			// αριθμός απόστασης(διαφορετικών χαρακτήρων) του κόμβου/λέξη από τον πατέρα κόμβο/λέξη
};


/////////// functions that help with implementation ///////////

// creates a BK_tree/index and initialize with no root and the Matchtype of the words
static Index* create_index(MatchType type, DestroyFunc destroy) {
	Index* indx = malloc(sizeof(Index));
	indx->root = NULL;
	indx->match_type = type;
	indx->destroy = destroy;
	return indx;
}

// creates a BK_node and initialized with an entry and the distance of this entry and the parent's node
static BK_node* create_BK_node(entry* entr, int dist) {
	BK_node* node = malloc(sizeof(BK_node));
	node->centry = entr;
	node->children = BK_list_create();
	node->dist = dist;
	return node;
}

// function that finds distance between 2 words
static int find_distance_word(word a, word b, MatchType type) {
	char* a_char = a;
	char* b_char = b;
	int diff = 0;	// number of differences

	if (type == MT_HAMMING_DIST) {		// find MT_Hamming distance
		// check all the characters until one or both words end
		while (*a_char != '\0' && *b_char !='\0') {
			if (*a_char != *b_char)	// if the characters are different, add one more to the difference number value
				diff++;
			a_char++;
			b_char++;
		}
		// if word a ends first, add the number of the rest characters of word b in total differences
		if (*a_char == '\0' && *b_char != '\0') {
			while (*b_char != '\0') {
				diff++;
				b_char++;
			}
		}
		// if word b ends first, add the number of the rest characters of word a in total differences
		else if (*b_char == '\0' && *a_char != '\0') {
			while (*a_char != '\0') {
				diff++;
				a_char++;
			}
		}
	}
	return diff;
}

// function that finds distance between 2 entries
static int find_distance_entries(entry* a, entry* b, MatchType type) {
	return find_distance_word(get_entry_word(a), get_entry_word(b), type);
}

// this function inserts a new entry in the tree recursively
static ErrorCode BK_insert_entry(entry *input, BK_node* tree_node, MatchType type) {
	if (tree_node == NULL || input == NULL)
		return EC_FAIL;
	
	int dist = find_distance_entries(tree_node->centry, input, type);		// find the distance of 2 entries
	// we remove duplicates, so if we have the same word to insert, we swap the node's entry with the new one and we end the function with success errorcode
	if (dist == 0) {
		tree_node->centry = input;
		return EC_SUCCESS;
	}

	// get the first node of the children's list
	BK_Listnode listnode = BK_list_first(tree_node->children);

	// case which the list has no list_nodes
	if (listnode == BK_LIST_EOF) {
		BK_list_insert_next(tree_node->children, BK_LIST_BOF, create_BK_node(input, dist));		// create the first list_node and create the new tree_node over there
		return EC_SUCCESS;		// end function with success errorcode
	}

	BK_Listnode previous = BK_LIST_BOF;		//initializing previous with the beginning of the list 
	// search until the node's distance is equal or higher than the current distance, or if you reach the end of the list
	while (listnode != BK_LIST_EOF && listnode->node->dist < dist) {
		previous = listnode;		// store previous list_node
		listnode = BK_list_next(listnode);	// move to the next list_node
	}
	// case of reaching the end of the list and still not found equal or higher distance OR just reached a higher distance
	if (listnode == BK_LIST_EOF || listnode->node->dist > dist)
		BK_list_insert_next(tree_node->children, previous, create_BK_node(input, dist));		// create a new position after previous and create the new tree_node over there
	// case of being equal
	else {
		ErrorCode err = BK_insert_entry(input, listnode->node, type);	// run again for the next tree_node
		if (err != EC_SUCCESS)
			return EC_FAIL;
	}

	return EC_SUCCESS;
}

// this function checks recursively which nodes/words are valid for the requested word
static entry_list* lookup_tree(const word w, BK_node* tree_node, int threshold, entry_list* result, MatchType type) {
	int dist = find_distance_word(w, get_entry_word(tree_node->centry), type);		// find distance between requested word and current node
	if (dist <= threshold)
		add_entry(result, tree_node->centry);	// we add the entry of the node to the result list

	// get the first node of the children's list
	BK_Listnode listnode = BK_list_first(tree_node->children);
	// checking all the children until the distance is higher than d+n
	while (listnode != BK_LIST_EOF && listnode->node->dist <= dist + threshold) {
		if (listnode->node->dist >= dist - threshold) {		// we also keep the nodes that their distance is lower than d-n
			result = lookup_tree(w, listnode->node, threshold, result, type);	// we call the function for the valid node
		}
		listnode = BK_list_next(listnode);	// go to the next child
	}
	return result;	// return the result list
}

// this function deletes the tree recursively
static ErrorCode destroy_tree(Index* indx, BK_node* tree_node) {
	if (tree_node == NULL)
		return EC_FAIL;
	
	BK_Listnode listnode = BK_list_first(tree_node->children);
	if (listnode != BK_LIST_EOF) {
		// destroy all subtrees
		while(listnode != BK_LIST_EOF){
			ErrorCode err = destroy_tree(indx, listnode->node);	// destroy the tree below current list_node
			if (err == EC_FAIL)
				return EC_FAIL;
			BK_list_remove_node(tree_node->children, BK_LIST_BOF, listnode);		// remove the list_node
			listnode = BK_list_first(tree_node->children);				// move to next list_node
		}
	}

	BK_list_destroy(tree_node->children);	// destroy list
	if (indx->destroy != NULL) {
		ErrorCode err = indx->destroy(tree_node->centry);						// deallocate entry
		if (err != EC_SUCCESS)
			return EC_FAIL;
	}
	free(tree_node);	// deallocate BK_node

	return EC_SUCCESS;
}


////////// functions of BK_tree.h //////////////


ErrorCode build_entry_index(const entry_list* el, MatchType type, Index** indx, DestroyFunc destroy){
	unsigned int el_count = get_number_entries(el);		// get size of entry list
	if (el_count <= 0) {		// etries do not exists
		printf("Error in build_entry_index: Input entry_list is empty\n");
		return EC_FAIL;
	}

	ErrorCode err;
	entry* entr = entry_list_node_value(get_first(el));	// get first entry

	*indx = create_index(type, destroy);	// create the BK_tree
	(*indx)->root = create_BK_node(entr, -1);		// create node of root(-1 because its the root)

	for (entry_list_node* node = get_first(el); node != LIST_EOF; node = get_next(el, node)) {
		entry* value = entry_list_node_value(node);
		err = BK_insert_entry(value, (*indx)->root, type);
		if(err != EC_SUCCESS )
			return err;
	}
	return EC_SUCCESS;
}

void create_entry_index(Index** indx, entry* entr, MatchType match_type, DestroyFunc destroy) {
	*indx = create_index(match_type, destroy);		// allocate index
}

ErrorCode insert_entry_index(Index* indx, entry* entr) {
	if (indx == NULL || entr == NULL) {
		printf("Error in insert_entry_index\n");
		return EC_FAIL;
	}
	ErrorCode err = EC_SUCCESS;
	if (indx->root == NULL)
		indx->root = create_BK_node(entr, -1);		// create root node
	else
		err = BK_insert_entry(entr, indx->root, indx->match_type);

	return err;
}

ErrorCode lookup_entry_index(const word w, Index* ix, int threshold, entry_list** result) {
	if (ix == NULL || threshold < 0 || *result == NULL)
		return EC_FAIL;
	*result = lookup_tree(w, ix->root, threshold, *result, ix->match_type);		// we call a recursive function for this job
	return EC_SUCCESS;
}


ErrorCode destroy_entry_index(Index* indx) {
	ErrorCode err = destroy_tree(indx, indx->root);
	if (err != EC_SUCCESS)
		return EC_FAIL;
	free(indx);
	return EC_SUCCESS;
}


///////////////////// function for testing ////////////////////////////////////


static void print_tree(BK_node *node){
	printf("Word : %s\n",get_entry_word(node->centry));
	BK_Listnode tmp_list = BK_list_first(node->children);
	int i = 0;
	while(tmp_list != BK_LIST_EOF){
		printf("Word : %s ith child: %d\n",get_entry_word(node->centry),i);
		print_tree(tmp_list->node);
		tmp_list = BK_list_next(tmp_list);
		i++;
	}
}


void BK_tree_print(Index *tree){
	print_tree(tree->root);
}