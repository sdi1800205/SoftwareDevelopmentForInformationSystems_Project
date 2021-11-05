#pragma once

#include "core.h"

typedef struct BK_tree Index;
typedef struct BK_node BK_node;

typedef struct BK_list* BK_list;
typedef struct BK_list_node* BK_listnode;


ErrorCode build_entry_index(const entry_list* , MatchType , Index* );
// enum error_code lookup_entry_index(const word, Index*, int, entry_list*);
// ErrorCode BK_destroy_entry(BK_node **);

// extra functions
// ErrorCode BK_insert_entry(entry *,BK_node *, MatchType);