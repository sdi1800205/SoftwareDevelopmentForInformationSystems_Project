#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/distances.h"
#include "../include/BK_tree.h"

#define HAMMING_LENGTH 28 // Length 28 => from 4 to 31

struct hammingIndex {
    struct BK_tree *BK_trees[HAMMING_LENGTH];   // array of pointers to BK_trees
};


hammingIndex* create_hamming_index() {
    hammingIndex* new;

    // initialize all pointers to NULL
    for (int i=0; i<HAMMING_LENGTH; i++) {
        new->BK_trees[i] = NULL; 
    }

    return new;
}

ErrorCode destroy_hamming_index(hammingIndex* h) {
    ErrorCode e;
    // destroy all of 28 BK_trees
    for (int i=0; i<HAMMING_LENGTH; i++) {
        e = destroy_entry_index(h->BK_trees[i]);

        if (e != EC_SUCCESS) return EC_FAIL;
    }

    return EC_SUCCESS;
}

ErrorCode insert_to_hamming_index(hammingIndex* h, word w) {
    int word_length = strlen(w);
    ErrorCode e;

    if (h->BK_trees[word_length - 4] == NULL) { // word_lengths from 4 to 28, array starts at 0 so word_length - 4 is the right index of the array
        // create new BK_tree if there isn't any allocated for this word_length
        e = build_entry_index( entry_list, MatchType, &(h->BK_trees[word_length - 4]) );
    }
    else {
        // insert into BK_tree. Exists for this word_length
        BK_insert_entry();
    }

    return EC_SUCCESS;
}

unsigned int hamming_distance(char *a, int length_a, char *b, int length_b) {
	char* a_char = a;
	char* b_char = b;
	int diff = 0;	// number of differences

    // Exit if lenght of words a,b are different
    if (length_a != length_b) {
        printf("Error in hamming distance, different word lengths compared\n");
        exit(EXIT_FAILURE);
    }

    // check all the characters until one or both words end
    while (*a_char != '\0' && *b_char !='\0') {
        if (*a_char != *b_char)	// if the characters are different, add one more to the difference number value
            diff++;

        a_char++;
        b_char++;
    }

    return diff;
}