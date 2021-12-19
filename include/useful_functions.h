#pragma once

#include "common_types.h"

// allocates memory for a new integer
int* create_int(int value);


// compare function for set-payload
int compare_ints(Pointer a, Pointer b);

// compares 2 strings
int compare_strings(Pointer a, Pointer b);