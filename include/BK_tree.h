#pragma once

#include "core.h"
#include "interface.h"

typedef struct BK_tree Index;
typedef struct BK_node BK_node;


ErrorCode build_entry_index(const entry_list* , MatchType , Index** );
ErrorCode lookup_entry_index(const word w, Index* ix, int threshold, entry_list** result);
ErrorCode destroy_entry_index(Index* );

// extra functions
void BK_tree_print(Index *);