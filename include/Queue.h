#pragma once // #include once

#include "ADTDeque.h"

// FIFO data type that pushes node at first and pop from end

typedef struct deque* Queue;

Queue queue_create();
int queue_size(Queue queue);
void queue_push(Queue queue, Pointer value);
Pointer queue_pop(Queue queue);