#ifndef BK_TREE
#define BK_TREE

#include "core.h"

typedef struct BK_node
{
	entry *centry;
	struct BK_node **children;
}BK_node; 

enum error_code build_entry_index(const entry_ist* , enum match_type , index* );

enum error_code BK_insert_entry(entry *,BK_node *,enum match_type );
enum error_code BK_destroy_entry(BK_node **);

#endif