#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interface.h"
#include "BK_tree.h"

struct  BK_tree {
    BK_node* root;
};

struct BK_node {
	entry *centry;
	struct BK_node **children;
};

// creates a BK_node and initialized with an entry and no children
static BK_node* create_BK_node(entry* entr) {
	BK_node* node = malloc(sizeof(BK_node));
	node->centry = entr;
	node->children = NULL;
	return node;
}

static int find_distance_entries(entry* a, entry* b) {
	// Προς υλοποίηση
}

/*
static int find_distance_words(word a, word b) {

}
*/

enum error_code build_entry_index(const entry_ist* el, enum match_type type, index* indx){
	unsigned int el_count = get_number_entries(el);		// get size of entry list
	if (el_count <= 0) {		// etries do not exists
		printf("Error in build_entry_index: Input entry_ist is empty\n");
		return 1;
	}

	enum error_code err;
	entry* entr = get_first(el);	// get first entry

	indx = malloc(sizeof(index));		// create the BK_tree
	indx->root = create_BK_node(entr);	// create node of root

	for(unsigned int i = 1; i < el_count; i++){
		entr = get_next(el, entr);		
		err = BK_insert_entry(entr, indx, MT_HAMMING_DIST);
		if(err != EC_SUCCESS ) return err;
	}
}


//////////////////////////////////////////////////
// Απο δω και κάτω δεν έχω κοιτάξει ακόμα
//////////////////////////////////////////////////

enum error_code BK_insert_entry(entry *input, BK_node *tree,enum match_type type){
	unsigned int dist = 0;

	if(tree == NULL){
		return EC_FAIL;
	}else if(tree->children == NULL){
		tree->children = (BK_node **)malloc(MAX_WORD_LENGTH*sizeof(BK_node *))
		for(int i=0; i < MAX_WORD_LENGTH; i++){
			(tree->children)[i] = NULL;
		}
		dist = get_distance(input,tree->centry,type) - 1;
		(tree->children)[dist] = (BK_node *)malloc(sizeof(BK_node));
		(tree->children)[dist]->centry = input;
		(tree->children)[dist]->children = NULL;
	}else{
		dist = get_distance(input,tree->centry,type) - 1;
		if((tree->children)[dist] != NULL){
			return BK_insert_entry(input,(tree->children)[dist],type);
		}else{
			(tree->children)[dist] = (BK_node *)malloc(sizeof(BK_node));
			(tree->children)[dist]->centry = input;
			(tree->children)[dist]->children = NULL;
		}
	}
	return EC_SUCCESS;
}

enum error_code BK_destroy_entry(BK_node **tree){
	if(*tree == NULL){
		return EC_SUCCESS;
	}else if((*tree)->children == NULL){
		free(*tree);
		*tree = NULL;
	}else{
		for(int i = 0; i<MAX_WORD_LENGTH; i++){
			BK_destroy_entry(&(((*tree)->children)[i]));
		}
		free((*tree)->children);
		free(*tree);
	}
	return EC_SUCCESS;

}