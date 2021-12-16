#include "distances.h"
#include "BK_tree.h"
#include "entry_list.h"


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

        if (e != EC_SUCCESS) 
            return EC_FAIL;
    }

    return EC_SUCCESS;
}

ErrorCode insert_to_hamming_index(hammingIndex* h, entry entr) {
    int word_length = strlen(w);
    ErrorCode e;

    if (h->BK_trees[word_length - 4] == NULL) { // word_lengths from 4 to 28, array starts at 0 so word_length - 4 is the right index of the array
        // create new BK_tree if there isn't any allocated for this word_length
        e = build_entry_index( entry_list, MatchType, &(h->BK_trees[word_length - 4]), (DestroyFunc)destroy_entry );
        e = create_entry_index(&(h->BK_trees[word_length - 4]), )
    }
    else {
        // insert into BK_tree. Exists for this word_length
        BK_insert_entry();
    }

    return EC_SUCCESS;
}
