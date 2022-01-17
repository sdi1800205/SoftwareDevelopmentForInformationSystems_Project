#include <stdlib.h>
#include <assert.h>

#include "Queue.h"
#include "ADTDeque.h"

Queue queue_create() {
    Queue queue = deque_create(0, NULL);
}

int queue_size(Queue queue) {
    return deque_size(queue);
}

void queue_push(Queue queue, Pointer value) {
    deque_insert_first(queue, value);
}

Pointer queue_pop(Queue queue) {
    Pointer last_node = deque_get_at(queue, deque_size(queue)-1);

    deque_remove_last(queue);

    return last_node;
}

void queue_destroy(Queue queue) {
    deque_destroy(queue);
}