#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BK_tree_build.h"

enum error_code build_entry_index(const entry_ist* el, enum match_type type, index* ix){
	unsigned int el_count = get_number_entries(el);
	entry *tmp;
	BK_node *BK_tmp;
	ix = malloc(sizeof(BK_node));
	enum error_code err;
	if(el_count > 0){
		ix->centry = get_first(el);
		ix->children = NULL;
	}else{
		printf("Error in build_entry_index: Input entry_ist is empty\n");
		return 1;
	}

	for(unsigned int i = 1; i < el_count; i++){
		err = BK_insert_entry(get_next(el),ix,type);
		if(err != EC_SUCCESS )
			return err;
	}
}

enum error_code BK_insert_entry(entry *input,BK_node *tree,enum match_type type){
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
		free(*tree;)
	}
	return EC_SUCCESS;

}

int main(int argc,char *argv[]){

}