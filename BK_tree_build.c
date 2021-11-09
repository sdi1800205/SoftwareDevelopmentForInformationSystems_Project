#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "BK_tree_build.h"
#include "interface.h"


static index* create_index(enum match_type type){
	index* ix = malloc(sizeof(index));
	ix->root = NULL;
	ix->match_type = type;
	return ix;
}

enum error_code build_entry_index(const entry_list* el, enum match_type type, index* ix){
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
		if (err != EC_SUCCESS)
			return err;
	}
}

enum error_code BK_insert_entry(entry *input,BK_node *tree,enum match_type type){
	int dist;
	BK_child *tmp_child;

	if(tree == NULL){
		return EC_FAIL;
	}else if(tree->children == NULL){
		tree->children = (BK_child *)malloc(sizeof(BK_child));
		tree->children->node = (BK_node *)malloc(sizeof(BK_node));
		tree->children->node->children = NULL;
		tree->children->node->centry = input;
		tree->children->distance = get_distance(tree->centry,input,type);
		tree->children->next = NULL;
	}else{
		dist = get_distance(tree->centry,input,type);
		if(dist < tree->children->distance){
			tmp_child = (BK_child *)malloc(sizeof(BK_child));
			tmp_child->node = (BK_node *)malloc(sizeof(BK_node));
			tmp_child->node->children = NULL;
			tmp_child->node->centry = input;
			tmp_child->distance = dist;
			tmp_child->next = tree->children;
			tree->children = tmp_child;		
		}else if(dist == tree->children->distance){
			return BK_insert_entry(input,tree->children,type);
		}else{
			BK_child *new_node;
			tmp_child = tree->children;
			while(tmp_child->next !=NULL && dist > tmp_child->next->distance){
				tmp_child = tmp_child->next;
			}

			if(tmp_child->next == NULL && dist > tmp_child->distance){
				new_node = (BK_child *)malloc(sizeof(BK_child));
				new_node->node = (BK_node *)malloc(sizeof(BK_node));
				new_node->node->children = NULL;
				new_node->node->centry = input;
				new_node->distance = dist;
				new_node->next = NULL;

				tmp_child->next = new_node;
			}else if(dist == tmp_child->next->distance){
				return BK_insert_entry(input,tmp_child->next->node,type);
			}else if(dist < tmp_child->next->distance){
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

enum error_code destroy_entry_index(index* ix){
	enum error_code err;
	err = BK_destroy_entry(&(ix->root));
	free(ix);
	return err;
}

enum error_code BK_destroy_entry(BK_node **tree){
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