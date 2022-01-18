#include "acutest.h"			// Απλή βιβλιοθήκη για unit testing

#include "Queue.h"
#include "useful_functions.h"

void test_create(void) {
    Queue queue = queue_create();

    TEST_ASSERT(queue_size(queue) == 0);

    queue_destroy(queue);
}

void test_push(void) {
    Queue queue = queue_create();

    int N = 1000;
	int* array = malloc(N * sizeof(*array));		// Στο queue θα προσθέσουμε pointers προς τα στοιχεία αυτού του πίνακα

	// insert 1000 στοιχεία ώστε να συμβούν πολλαπλά resizes
	for (int i = 0; i < 1000; i++) {
		queue_push(queue, &array[i]);
		TEST_ASSERT(queue_size(queue) == i+1);		// Το size ενημερώθηκε;
	}

    for (int i = 999; i >= 0; i--) {
        TEST_ASSERT(queue_size(queue) == i+1);
        queue_pop(queue);
    }

    queue_destroy(queue);
}

void test_pop(void) {
    Queue queue = queue_create();

    int N = 1000;
	int* array = malloc(N * sizeof(*array));		// Στο queue θα προσθέσουμε pointers προς τα στοιχεία αυτού του πίνακα

	// insert 1000 στοιχεία ώστε να συμβούν πολλαπλά resizes
	for (int i = 0; i < 1000; i++) {
		queue_push(queue, &array[i]);
		TEST_ASSERT(queue_size(queue) == i+1);		// Το size ενημερώθηκε;
	}

    for (int i = 999; i >= 0; i--) {
        TEST_ASSERT(queue_size(queue) == i+1);
        queue_pop(queue);
    }

    queue_destroy(queue);
}

void test_destroy(void) {
    Queue queue = queue_create();

    for (int i=0; i<1000; i++)
        queue_push(queue, malloc(1));

    TEST_ASSERT(queue_size(queue) == 1000);

    for (int i=0; i<1000; i++)   
        queue_pop(queue);

    TEST_ASSERT(queue_size(queue) == 0);

    queue_destroy(queue);
}


// Λίστα με όλα τα tests προς εκτέλεση
TEST_LIST = {
	{ "queue_create", test_create },
	{ "queue_push", test_push },
	{ "queue_pop", test_pop },
	{ "queue_destroy", test_destroy },
	{ NULL, NULL } // τερματίζουμε τη λίστα με NULL
};