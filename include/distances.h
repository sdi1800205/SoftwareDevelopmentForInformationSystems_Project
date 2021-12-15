#pragma once

#include "../include/core.h"
#include "../include/BK_tree.h"

typedef struct hammingIndex hammingIndex;

hammingIndex* create_hamming_index();
ErrorCode destroy_hamming_index(hammingIndex* h);
ErrorCode insert_hamming_index(hammingIndex* h, word w);