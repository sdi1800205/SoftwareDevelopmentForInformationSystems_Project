#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interface.h"
#include "BK_tree.h"

// pointer to function that finds the distance of 2 words of 2 entries
typedef int (*DistanceFunc)(entry* a, entry* b);

struct  BK_tree {
    BK_node* root;		// root of tree/index
	MatchType match_type;		// type that matches words
};

struct BK_node {
	entry *centry;
	struct BK_node **children;
};


/////////// functions that help with implementation ///////////

// creates a BK_tree/index and initialize with no root and the Matchtype of the words
static Index* create_index(MatchType type) {
	Index* indx = malloc(sizeof(Index));
	indx->root = NULL;
	indx->match_type = type;
	return indx;
}

// creates a BK_node and initialized with an entry and an array for the children with size: MAX_WORD_LENGTH
// because this is the max size of a word, therefore the max possible number of differences between the words
static BK_node* create_BK_node(entry* entr) {
	BK_node* node = malloc(sizeof(BK_node));
	node->centry = entr;
	node->children = malloc(MAX_WORD_LENGTH * sizeof(BK_node*));
	return node;
}

// function that finds the MT_HAMMING_DIST distance between 2 words of 2 entries
static int find_distance_entries_HAM(entry* a, entry* b) {
	char a_char = a->word;
	char b_char = b->word;
	int diff = 0;	// number of differences

	// check all the characters until one or both words end
	while (a_char != '\0' && b_char !='\0') {
		if (a_char != b_char)	// if the characters are different, add one more to the difference number value
			diff++;
		a_char++;
		b_char++;
	}
	// if word a ends first, add the number of the rest characters of word b in total differences
	if (a_char == '\0' && b_char != '\0') {
		while (b_char != '\0') {
			diff++;
			b_char++;
		}
	}
	// if word b ends first, add the number of the rest characters of word a in total differences
	else if (b_char == '\0' && a_char != '\0') {
		while (a_char != '\0') {
			diff++;
			a_char++;
		}
	}
	return diff;
}

/*
static int find_distance_words(word a, word b) {

}
*/

////////// functions of BK_tree.h //////////////


enum error_code build_entry_index(const entry_list* el, enum match_type type, Index* indx){
	unsigned int el_count = get_number_entries(el);		// get size of entry list
	if (el_count <= 0) {		// etries do not exists
		printf("Error in build_entry_index: Input entry_list is empty\n");
		return 1;
	}

	enum error_code err;
	entry* entr = get_first(el);	// get first entry

	indx = create_index(type);	// create the BK_tree
	indx->root = create_BK_node(entr);		// create node of root

	for(unsigned int i = 1; i < el_count; i++){
		entr = get_next(el, entr);		
		err = BK_insert_entry(entr, indx, type);
		if(err != EC_SUCCESS ) return err;
	}
}


//////////////////////////////////////////////////
// Απο δω και κάτω δεν έχω κοιτάξει ακόμα
//////////////////////////////////////////////////
/*
enum error_code BK_insert_entry(entry *input, BK_node *tree, enum match_type type){
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

*/