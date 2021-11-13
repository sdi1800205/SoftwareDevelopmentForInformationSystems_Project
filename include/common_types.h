#pragma once

#include "core.h"

// Pointer to function that destroys an element/entry
typedef ErrorCode (*DestroyFunc)(void* value);