#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BK_tree_build.h"

typedef struct BK_node BK_node;
typedef struct BK_child BK_child;

struct BK_node
{
	entry *centry;
	BK_child *children;
};

struct BK_child
{
	BK_node *node;
	int distance;
	BK_child *next;
};

struct  BK_tree {
    BK_node* root;		// root of tree/index
	MatchType match_type;		// type that matches words
};


static Index* create_index(MatchType type){
	Index* ix = malloc(sizeof(Index));
	ix->root = NULL;
	ix->match_type = type;
	return ix;
}

static int find_distance_entries(entry* a, entry* b, MatchType type) {
	char* a_char = get_entry_word(a);
	char* b_char = get_entry_word(b);
	int diff = 0;	// number of differences

	if (type == MT_HAMMING_DIST) {
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

static ErrorCode BK_insert_entry(entry *input,BK_node *tree,MatchType type){
	int dist;
	BK_child *tmp_child;

	if(tree == NULL){		//non existent tree
		return EC_FAIL;
	}else if(tree->children == NULL){		//current tree node has no child nodes
		tree->children = (BK_child *)malloc(sizeof(BK_child));
		tree->children->node = (BK_node *)malloc(sizeof(BK_node));
		tree->children->node->children = NULL;
		tree->children->node->centry = input;
		tree->children->distance = find_distance_entries(tree->centry,input,type);
		tree->children->next = NULL;
	}else{
		dist = find_distance_entries(tree->centry,input,type);	//get the distance between the input word and the word of the current node
		if(dist < tree->children->distance){		//if this distance is smaller than the distance between the first child node and the current node
			tmp_child = (BK_child *)malloc(sizeof(BK_child));		//insert child in the start of the list
			tmp_child->node = (BK_node *)malloc(sizeof(BK_node));
			tmp_child->node->children = NULL;
			tmp_child->node->centry = input;
			tmp_child->distance = dist;
			tmp_child->next = tree->children;
			tree->children = tmp_child;		
		}else if(dist == tree->children->distance){		//if it's equal the latter distance move deeper into the tree
			return BK_insert_entry(input,tree->children,type);
		}else{
			BK_child *new_node;
			tmp_child = tree->children;
			while(tmp_child->next !=NULL && dist > tmp_child->next->distance){		//find the right position in the list to insert the new child node (sorted list)
				tmp_child = tmp_child->next;
			}

			if(tmp_child->next == NULL && dist > tmp_child->distance){		//if we've reached the end of the list and the input word distance is greater than the word of the last child node
				new_node = (BK_child *)malloc(sizeof(BK_child));
				new_node->node = (BK_node *)malloc(sizeof(BK_node));
				new_node->node->children = NULL;
				new_node->node->centry = input;
				new_node->distance = dist;
				new_node->next = NULL;

				tmp_child->next = new_node;
			}else if(dist == tmp_child->next->distance){			//if it's equal to the child node move deeper
				return BK_insert_entry(input,tmp_child->next->node,type);
			}else if(dist < tmp_child->next->distance){			//if we've reached the right position (current child node distance < input word distance < next child node distance)
				new_node = (BK_child *)malloc(sizeof(BK_child));
				new_node->node = (BK_node *)malloc(sizeof(BK_node));
				new_node->node->children = NULL;
				new_node->node->centry = input;
				new_node->distance = dist;
				new_node->next = tmp_child->next;

				tmp_child->next = new_node;
			}
		}
	}
	return EC_SUCCESS;
}

static ErrorCode BK_destroy_entry(BK_node **tree){
	BK_child *tmp_child;
	if(*tree == NULL){
		return EC_SUCCESS;
	}else{
		if((*tree)->children == NULL){
			free(*tree);
			*tree = NULL;
		}else{
			while((*tree)->children != NULL){
				tmp_child = (*tree)->children->next;
				err = BK_destroy_entry(&((*tree)->children->node));
				if(err != EC_SUCCESS){
					return err;
				}
				free((*tree)->children);
				(*tree)->children = tmp_child;
			}
			free(*tree);
			*tree = NULL;
		}
	}
}

ErrorCode build_entry_index(const entry_list* el, MatchType type, Index* ix){
	unsigned int el_count = get_number_entries(el);		// get size of entry list
	if (el_count <= 0) {		// etries do not exists
		printf("Error in build_entry_index: Input entry_list is empty\n");
		return EC_FAIL;
	}

	ErrorCode err;
	entry* entr = get_first(el);	// get first entry

	ix = create_index(type);	// create the BK_tree
	ix->root = create_BK_node(entr);		// create node of root

	for(unsigned int i = 1; i < el_count; i++){
		entr = get_next(el, entr);		
		err = BK_insert_entry(entr, ix->root, type);
		if(err != EC_SUCCESS)
			return err;
	}
}

ErrorCode destroy_entry_index(Index* ix){
	ErrorCode err;
	err = BK_destroy_entry(&(ix->root));
	free(ix);
	return err;
}