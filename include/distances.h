#pragma once

#include "common_types.h"
#include "core.h"
#include "entry_list.h"

int exact_distance_entry(Pointer a, Pointer b);
int edit_distance_entry(entry* a, entry* b);
int hamming_distance_entry(entry* a, entry* b);

int exact_distance(word a, int length_a, word b, int length_b);
int edit_distance(word a,int na,word b,int nb);
int hamming_distance(word a, int length_a, word b, int length_b);