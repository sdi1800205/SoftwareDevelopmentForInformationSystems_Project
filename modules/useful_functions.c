#include "useful_functions.h"
#include <stdlib.h>

// allocates memory for a new integer
int* create_int(int value) {
	int* pointer = malloc(sizeof(int));		// memory allocation
	*pointer = value;						// copy value at the new integer
	return pointer;
}

// compare function for set-payload
int compare_ints(Pointer a, Pointer b) {
    return *(int*)a - *(int*)b;
}