#include "acutest.h"			// Απλή βιβλιοθήκη για unit testing

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "entry_list.h"
#include "BK_tree.h"
#include "core.h"

// #define _DEBUG_ 1

// this function reads a text of words and put them into an entry_list
static entry_list* read_words() {
    FILE* fp;
    char* word = NULL;
    size_t len = 0;
    ssize_t read;

    ErrorCode return_code;

    #ifdef _DEBUG_
    printf("\n---In test..\n");
    #endif

    // Open test file
    fp = fopen("./test.txt", "r");
    TEST_ASSERT(fp != NULL);

    // Create entry list
    entry_list* EntryList;
    return_code = create_entry_list(&EntryList, (DestroyFunc)destroy_entry);
    TEST_ASSERT(return_code == EC_SUCCESS);

    #ifdef _DEBUG_
    printf("---Entry list created successfully\n\n");
    #endif

    // Read line by line the file. Each line has a word
    while ((read = getline(&word, &len, fp)) != -1) {
        //printf("Retrieved line of length %zu:\n", read);

        // Remove newline character if exists in word
        if (word[strlen(word)-1] == '\n'){
            word[strlen(word)-1] = '\0';
        }

        // Create new entry with given word
        entry* new_entry;
        return_code = create_entry(word, &new_entry);
        TEST_ASSERT(return_code == EC_SUCCESS);

        #ifdef _DEBUG_
        printf("---New entry created with word=%s\n", get_entry_word(new_entry));
        #endif

        // Add entry to entry_list
        return_code = add_entry(EntryList, new_entry);
        TEST_ASSERT(return_code == EC_SUCCESS);

        #ifdef _DEBUG_
        printf("---Entry added to entry_list\n");
        printf("---Current number of entries in entry_list: %d\n\n", get_number_entries(EntryList));
        #endif
    }

    fclose(fp);
    if (word) {
        free(word);
    }

    return EntryList;
}

void test_leaks() {
    entry_list* EntryList;
    create_entry_list(&EntryList, (DestroyFunc)destroy_entry);
    destroy_entry_list(EntryList);
}

void test_entry_list() {
    entry_list* EntryList = read_words();

    #ifdef _DEBUG_
    printf("---Total number of entries: %d\n\n", get_number_entries(EntryList));
    #endif
    // check number of words
    TEST_ASSERT(get_number_entries(EntryList) == 7);
    
    // check first word
    entry_list_node* node = get_first(EntryList);
    entry* entr = entry_list_node_value(node);
    word w = get_entry_word(entr);
    TEST_ASSERT(strcmp(w, "hell") == 0);

    // check second
    node = get_next(EntryList, node);
    entr = entry_list_node_value(node);
    w = get_entry_word(entr);
    TEST_ASSERT(strcmp(w, "help") == 0);

    // check third
    node = get_next(EntryList, node);
    entr = entry_list_node_value(node);
    w = get_entry_word(entr);
    TEST_ASSERT(strcmp(w, "fall") == 0);

    ErrorCode return_code = destroy_entry_list(EntryList);
    TEST_ASSERT(return_code == EC_SUCCESS);

    #ifdef _DEBUG_
    printf("---Entry list destroyed\n");
    printf("---Program terminated successfully\n");
    #endif
}

void test_entry_index() {
    entry_list* EntryList = read_words();
    Index* indx;
    ErrorCode err = build_entry_index(EntryList, MT_EXACT_MATCH, &indx, NULL);      // the old hamming match is the new exact match
    TEST_ASSERT(err == EC_SUCCESS);
    // printf("\n");
    // BK_tree_print(indx);
    err = destroy_entry_index(indx);
    TEST_ASSERT(err == EC_SUCCESS);
    destroy_entry_list(EntryList);
}

void test_lookup_entry_index() {
    entry_list* EntryList = read_words();
    Index* indx;
    ErrorCode err = build_entry_index(EntryList, MT_EXACT_MATCH, &indx, NULL);      // the old hamming match is the new exact match
    TEST_ASSERT(err == EC_SUCCESS);

    entry_list* result;
    create_entry_list(&result, NULL);
    lookup_entry_index("henn", indx, 2, &result);

    // Tests
    TEST_ASSERT(get_number_entries(result) == 2);   // check that the result list has 2 elements
    
    entry_list_node* node = get_first(result);
    entry* entr = entry_list_node_value(node);
    TEST_ASSERT(strcmp(get_entry_word(entr), "hell") == 0);     // check first word
    
    node = get_next(result, node);
    entr = entry_list_node_value(node);
    TEST_ASSERT(strcmp(get_entry_word(entr), "help") == 0);     // check second word

    destroy_entry_index(indx);
    destroy_entry_list(result);
    destroy_entry_list(EntryList);
}

// Λίστα με όλα τα tests προς εκτέλεση
TEST_LIST = {
    // { "test_leaks", test_leaks },
	{ "test_entry_list", test_entry_list },
	{ "test_entry_index", test_entry_index },
    { "test_lookup_entry_index", test_lookup_entry_index },

	{ NULL, NULL } // τερματίζουμε τη λίστα με NULL
};