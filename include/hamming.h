#pragma once

#include <pthread.h>

#include "core.h"
#include "BK_tree.h"
#include "entry_list.h"

typedef struct hamIndex hamIndex;

hamIndex* create_hamming_index(DestroyFunc destroy);
ErrorCode destroy_hamming_index(hamIndex* hamindx);
entry* insert_hamming_index(hamIndex* hamindx, entry* entr);
ErrorCode lookup_hamming_index(const word w, hamIndex* hamindx, int threshold, entry_list** result, pthread_t target_thread);