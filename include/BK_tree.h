#pragma once

#include "core.h"
#include "interface.h"

typedef struct BK_tree Index;
typedef struct BK_node BK_node;


ErrorCode build_entry_index(const entry_list* , MatchType , Index** );
// enum error_code lookup_entry_index(const word, Index*, int, entry_list*);
ErrorCode destroy_entry_index(Index* );

// extra functions
// ErrorCode BK_insert_entry(entry *,BK_node *, MatchType);

void BK_tree_print(Index *);