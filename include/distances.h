#pragma once

#include "common_types.h"
#include "entry_list.h"

int exact_distance(word a, word b);
int edit_distance(word a,int na,word b,int nb);
int hamming_distance(word a, int length_a, word b, int length_b);