#ifndef BK_TREE
#define BK_TREE

#include "core.h"

typedef struct BK_tree index;
typedef struct BK_node BK_node;

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

struct  BK_tree {
    BK_node* root;		// root of tree/index
	enum match_type type;		// type that matches words
};

enum error_code build_entry_index(const entry_ist* , enum match_type , index* );
enum error_code destroy_entry_index(index* );

enum error_code BK_insert_entry(entry *,BK_node *,enum match_type );
enum error_code BK_destroy_entry(BK_node **);

#endif