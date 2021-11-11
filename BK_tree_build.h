#ifndef BK_TREE
#define BK_TREE

#include "core.h"

typedef struct BK_tree Index;

ErrorCode build_entry_index(const entry_ist* , MatchType , Index* );
ErrorCode destroy_entry_index(Index* );

#endif