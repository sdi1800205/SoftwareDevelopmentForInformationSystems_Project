#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "BK_tree.h"
#include "BK_List.h"
#include "distances.h"
#include "ADTSet.h"
#include "ADTMap.h"
#include "useful_functions.h"

//////////// structs of module ////////////////

struct  BK_tree {
    BK_node* root;		// root of tree/index
	MatchType match_type;		// type that matches words
	DestroyFunc destroy;		// function called when an entry is going to be deleted from the tree
	// pthread_mutex_t lock_tree;	//mutex for safe access on tree nodes' contents
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
	// pthread_mutex_init(&(indx->lock_tree),0);
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
	int res;

	if (type == MT_EDIT_DIST)
		res = edit_distance(a, strlen(a), b, strlen(b));
	else if (type == MT_HAMMING_DIST)
		res = hamming_distance(a, strlen(a), b, strlen(b));
	else
		res = exact_distance(a, b);
	return res;
}

// function that finds distance between 2 entries
static int find_distance_entries(entry* a, entry* b, MatchType type) {
	return find_distance_word(get_entry_word(a), get_entry_word(b), type);
}

// this function inserts a new entry in the tree recursively
// and returns the node that has been inserted . In case of the entry's word already exists in the tree
// it appends the new entry's payload to the old one and returns the old entry instead as the entry that has been updated
static entry* BK_insert_entry(entry *input, BK_node* tree_node, MatchType type) {
	if (tree_node == NULL || input == NULL)
		return NULL;
	
	int dist = find_distance_entries(tree_node->centry, input, type);		// find the distance of 2 entries
	// we remove duplicates, so if we have the same word to insert,
	// we just ignore new entry and keep old one
	if (dist == 0)
		return tree_node->centry;		// return old entry

	// get the first node of the children's list
	BK_Listnode listnode = BK_list_first(tree_node->children);

	// case which the list has no list_nodes
	if (listnode == BK_LIST_EOF) {
		BK_list_insert_next(tree_node->children, BK_LIST_BOF, create_BK_node(input, dist));		// create the first list_node and create the new tree_node over there
		return input;		// return the inserted entry
	}

	BK_Listnode previous = BK_LIST_BOF;		//initializing previous with the beginning of the list 
	// search until the node's distance is equal or higher than the current distance, or if you reach the end of the list
	while (listnode != BK_LIST_EOF && listnode->node->dist < dist) {
		previous = listnode;		// store previous list_node
		listnode = BK_list_next(listnode);	// move to the next list_node
	}
	// case of reaching the end of the list and still not found equal or higher distance OR just reached a higher distance
	if (listnode == BK_LIST_EOF || listnode->node->dist > dist) {
		BK_list_insert_next(tree_node->children, previous, create_BK_node(input, dist));		// create a new position after previous and create the new tree_node over there
		return input;
	}
	// case of being equal
	else
		return BK_insert_entry(input, listnode->node, type);	// run again for the next tree_node
}

// this function checks recursively which nodes/words are valid for the requested word
static entry_list* lookup_tree(const word w, BK_node* tree_node, int threshold, entry_list* result, MatchType type, pthread_t target_thread) {
	int dist = find_distance_word(w, get_entry_word(tree_node->centry), type);		// find distance between requested word and current node
	if (dist <= threshold) {
		// pthread_mutex_lock(&lock_tree);
		// check if the entry haw already get into results
		if (get_entry_pthread_t(tree_node->centry, target_thread) != NULL) {		// case of this entry already been in results of current thread
			if (dist < get_entry_dist(tree_node->centry, target_thread))		// we keep the minimum distance of this entry of this thread in the document
				set_entry_dist(tree_node->centry, target_thread, dist);		// we keep the distance that made this entry matched
		}
		else {		
			entry_add_thread(tree_node->centry, target_thread);		// we set true the value of entry that shows it has been matched with the document
			set_entry_dist(tree_node->centry, target_thread, dist);				// we keep the distance that made this entry matched										// case of this entry first results-appearance
			add_entry(result, tree_node->centry);					// we add the entry of the node to the result list
		}
		// pthread_mutex_unlock(&lock_tree);
	}

	// get the first node of the children's list
	BK_Listnode listnode = BK_list_first(tree_node->children);
	// checking all the children until the distance is higher than d+n
	while (listnode != BK_LIST_EOF && listnode->node->dist <= dist + threshold) {
		if (listnode->node->dist >= dist - threshold) {		// we also keep the nodes that their distance is lower than d-n
			result = lookup_tree(w, listnode->node, threshold, result, type, target_thread);	// we call the function for the valid node
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

	entry* entr = entry_list_node_value(get_first(el));	// get first entry

	*indx = create_index(type, destroy);	// create the BK_tree
	(*indx)->root = create_BK_node(entr, -1);		// create node of root(-1 because its the root)

	for (entry_list_node* node = get_first(el); node != LIST_EOF; node = get_next(el, node)) {
		entry* value = entry_list_node_value(node);
		entry* inserted = BK_insert_entry(value, (*indx)->root, type);
		if(inserted == NULL )		// the returned entry should have a value, if its NULL then something went wrong
			return EC_FAIL;
	}
	return EC_SUCCESS;
}

void create_entry_index(Index** indx, MatchType match_type, DestroyFunc destroy) {
	*indx = create_index(match_type, destroy);		// allocate index
}

entry* insert_entry_index(Index* indx, entry* entr) {
	if (indx == NULL || entr == NULL) {
		printf("Error in insert_entry_index\n");
		return NULL;
	}
	if (indx->root == NULL) {
		indx->root = create_BK_node(entr, -1);		// create root node
		return entr;
	}
	else
		return BK_insert_entry(entr, indx->root, indx->match_type);
}

ErrorCode lookup_entry_index(const word w, Index* ix, int threshold, entry_list** result, pthread_t target_thread) {
	if (ix == NULL || threshold < 0 || *result == NULL)
		return EC_FAIL;
	else if (ix->root == NULL)	// case of tree being empty
		return EC_SUCCESS;		// this is no mistake, its a possible outcome so continue
	else
		*result = lookup_tree(w, ix->root, threshold, *result, ix->match_type, target_thread);		// we call a recursive function for this job
	
	return EC_SUCCESS;
}


ErrorCode destroy_entry_index(Index* indx) {
	ErrorCode err = destroy_tree(indx, indx->root);
	if (err != EC_SUCCESS)
		return EC_FAIL;
	// pthread_mutex_destroy(&(indx->lock_tree));
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