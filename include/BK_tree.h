#pragma once

#include "core.h"
#include "entry_list.h"
#include "common_types.h"

typedef struct BK_tree Index;
typedef struct BK_node BK_node;


ErrorCode build_entry_index(const entry_list* , MatchType , Index** , DestroyFunc);     // creates a bk tree from an entry_list
void create_entry_index(Index**, entry*, MatchType, DestroyFunc);                  // creates a bk tree with one entry(root)
ErrorCode insert_entry_index(Index*, entry*);                                           // inserts an entry in an already created bk tree
ErrorCode lookup_entry_index(const word w, Index* ix, int threshold, entry_list** result);
ErrorCode destroy_entry_index(Index* );

// extra functions
void BK_tree_print(Index *);