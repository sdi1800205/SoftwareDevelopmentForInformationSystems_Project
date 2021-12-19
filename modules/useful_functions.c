#include <stdlib.h>
#include <string.h>

#include "distances.h"
#include "useful_functions.h"

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

int compare_strings(Pointer a, Pointer b) {
	return exact_distance((char*)a, strlen((char*)a), (char*)b, strlen((char*)b));
}