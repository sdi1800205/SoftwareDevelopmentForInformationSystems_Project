#pragma once

#include "core.h"

typedef struct BK_tree Index;
typedef struct BK_node BK_node;

enum error_code build_entry_index(const entry_list* , enum match_type , Index* );
enum error_code lookup_entry_index(const word, Index*, int, entry_list*)
enum error_code BK_destroy_entry(BK_node **);

// extra functions
enum error_code BK_insert_entry(entry *,BK_node *,enum match_type );