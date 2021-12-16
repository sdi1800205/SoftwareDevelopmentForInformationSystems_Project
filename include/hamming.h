#pragma once

#include "core.h"
#include "BK_tree.h"
#include "entry_list.h"

typedef struct hamIndex hamIndex;

hamIndex* create_hamming_index();
ErrorCode destroy_hamming_index(hamIndex* h);
ErrorCode insert_hamming_index(hamIndex* h, entry* entr);