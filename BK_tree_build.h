#ifndef BK_TREE
#define BK_TREE

typedef struct BK_node
{
	entry *centry;
	struct BK_node **children;
}BK_node; 

enum error_code build_entry_index(const entry_ist* el, enum match_type type, index* ix);

#endif