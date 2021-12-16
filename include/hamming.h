#pragma once

#include "core.h"
#include "BK_tree.h"
#include "entry_list.h"

typedef struct hammingIndex hammingIndex;

hammingIndex* create_hamming_index();
ErrorCode destroy_hamming_index(hammingIndex* h);
ErrorCode insert_hamming_index(hammingIndex* h, entry* entr);