#pragma once

#include "core.h"

// Χρήση του τύπου "bool" για μεταβλητές που παίρνουν μόνο τιμές true / false
#include <stdbool.h> 

// pointer to an object of any type
typedef void* Pointer;

// pointer to a function that compares 2 objects a and b and returns:
// < 0  if a < b
//   0  if a and b have equal value (_not_ necessarily the same object)
// > 0  if a > b
typedef int (*CompareFunc)(Pointer a, Pointer b);

// Pointer to function that destroys an object
typedef ErrorCode (*DestroyFunc)(Pointer value);

#define MAX_INT 0x7FFFFFFF