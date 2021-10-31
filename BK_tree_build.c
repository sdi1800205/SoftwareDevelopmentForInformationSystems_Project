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
		if(err != 0 )
			return err;
	}
}
///////////

typedef struct BK_child BK_child;

typedef struct BK_node
{
	entry *centry;
	BK_child *children;
}BK_node;

struct BK_child
{
	BK_node *node;
	int distance;
	BK_child *next;
};

///////////
enum error_code BK_insert_entry(entry *input,BK_node *tree,enum match_type type){
	int dist;
	BK_child *tmp_child;

	if(tree == NULL){
		return 1;
	}else if(tree->children == NULL){
		tree->children = (BK_child *)malloc(sizeof(BK_child));
		tree->children->node = (BK_node *)malloc(sizeof(BK_node));
		tree->children->node->children = NULL;
		tree->children->node->centry = input;
		tree->children->distance = get_distance(tree->centry,input,type);
		tree->children->next = NULL;
	}else{ 
		if(){
		
		}
	}
}



int main(int argc,char *argv[]){

}