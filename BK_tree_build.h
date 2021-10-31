#ifndef BK_TREE
#define BK_TREE

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

enum error_code build_entry_index(const entry_ist* el, enum match_type type, index* ix);

#endif