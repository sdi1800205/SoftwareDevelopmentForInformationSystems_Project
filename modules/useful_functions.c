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

// allocates memory for a new pthread_t
pthread_t* create_pthread_t(pthread_t value) {
	pthread_t* thread = malloc(sizeof(pthread_t));
	*thread = value;
	return thread;
}

// deallocates pthread_t*
ErrorCode destroy_pthread_t(Pointer value) {
	free(value);
}


// compare function for set-payload
int compare_ints(Pointer a, Pointer b) {
    return *(int*)a - *(int*)b;
}

int compare_strings(Pointer a, Pointer b) {
	return exact_distance((char*)a, (char*)b);
}

// compares 2 pthreads_t
int compare_pthread_t(Pointer a, Pointer b) {
	if (*(pthread_t*)a > *(pthread_t*)b)
		return 1;
	else if (*(pthread_t*)a == *(pthread_t*)b)
		return 0;
	else
		return -1;
}