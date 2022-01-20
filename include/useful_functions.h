#pragma once

#include <pthread.h>

#include "common_types.h"

// allocates memory for a new integer
int* create_int(int value);

// allocates memory for a new pthread_t
pthread_t* create_pthread_t(pthread_t value);

// deallocates pthread_t*
ErrorCode destroy_pthread_t(Pointer value);

// compare function for set-payload
int compare_ints(Pointer a, Pointer b);

// compares 2 strings
int compare_strings(Pointer a, Pointer b);

// compares 2 pthreads_t
int compare_pthread_t(Pointer a, Pointer b);