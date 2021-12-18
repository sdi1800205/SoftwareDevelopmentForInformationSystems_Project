#include <stdlib.h>
#include <string.h>

#include "hamming.h"
#include "BK_tree.h"
#include "entry_list.h"


#define HAMMING_LENGTH 28 // Length 28 => from 4 to 31

struct hamIndex {
    struct BK_tree *BK_trees[HAMMING_LENGTH];   // array of pointers to BK_trees
    DestroyFunc destroy;                        // function that pass through the trees
};


hamIndex* create_hamming_index(DestroyFunc destroy) {
    hamIndex* new_indx = malloc(sizeof(*new_indx));
    new_indx->destroy = destroy;

    // initialize all pointers to NULL
    for (int i=0; i<HAMMING_LENGTH; i++) {
        new_indx->BK_trees[i] = NULL; 
    }

    return new_indx;
}

ErrorCode destroy_hamming_index(hamIndex* h) {
    ErrorCode e;
    // destroy all of 28 BK_trees
    for (int i=0; i<HAMMING_LENGTH; i++) {
        if (h->BK_trees[i] != NULL) {       // check if tree exists
            e = destroy_entry_index(h->BK_trees[i]);
            if (e != EC_SUCCESS) 
                return EC_FAIL;
        }
    }

    return EC_SUCCESS;
}

entry* insert_hamming_index(hamIndex* h, entry *e) {
    int word_length = strlen(get_entry_word(e));

    if (h->BK_trees[word_length - 4] == NULL) { // word_lengths from 4 to 28, array starts at 0 so word_length - 4 is the right index of the array
        // create new BK_tree if there isn't any allocated for this word_length
        create_entry_index(&(h->BK_trees[word_length - 4]), MT_HAMMING_DIST, h->destroy);
    }

    // insert into BK_tree. Exists for this word_length
    return insert_entry_index(h->BK_trees[word_length - 4], e);
}

ErrorCode lookup_hamming_index(const word w, hamIndex* hamindx, int threshold, entry_list** result) {
    if (hamindx == NULL || threshold < 0 || *result == NULL)
    	return EC_FAIL;
    
    // we use lookup for the correct tree, with words' lenght equal to target word's lenght
    // since the hamming distance is used only for words with the same lenght
	
    if (hamindx->BK_trees[strlen(w) - 4] == NULL)       // maybe the tree for the wanted word's lenght doesn't exists
        return EC_SUCCESS;
    return lookup_entry_index(w, hamindx->BK_trees[strlen(w) - 4], threshold, result);
}