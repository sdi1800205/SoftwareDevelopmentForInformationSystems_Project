#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interface.h"
#include "core.h"

#define _DEBUG_ 1

int main(void) {
    FILE* fp;
    char* word = NULL;
    size_t len = 0;
    ssize_t read;

    ErrorCode return_code;

    #ifdef _DEBUG_
    printf("---In main..\n");
    #endif

    // Open test file
    fp = fopen("./tests/test.txt", "r");
    if (fp == NULL) {
        return -1;
    }

    // Create entry list
    entry_list* EntryList;
    return_code = create_entry_list(&EntryList);
    if (return_code != EC_SUCCESS) {
        printf("Error while creating entry list\n");
        return -1;
    }

    #ifdef _DEBUG_
    printf("---Entry list created successfully\n\n");
    #endif

    // Read line by line the file. Each line has a word
    while ((read = getline(&word, &len, fp)) != -1) {
        // printf("Retrieved line of length %zu:\n", read);

        // Remove newline character if exists in word
        if (word[strlen(word)-1] == '\n') {
            word[strlen(word)-1] = '\0';
        }

        // Create new entry with given word
        entry* new_entry;
        return_code = create_entry(word, &new_entry);
        if(return_code != EC_SUCCESS){
            printf("Error while creating new entry\n");
            return -1;
        }

        #ifdef _DEBUG_
        printf("---New entry created with word=%s\n", get_entry_word(new_entry));
        #endif

        // Add entry to entry_list
        return_code = add_entry(EntryList, new_entry);
        if (return_code != EC_SUCCESS) {
            printf("Error while adding new entry to entry_list");
            return -1;
        }

        #ifdef _DEBUG_
        printf("---Entry added to entry_list\n");
        printf("---Current number of entries in entry_list: %d\n\n", get_number_entries(EntryList));
        #endif
    }

    #ifdef _DEBUG_
    printf("---Total number of entries: %d\n\n", get_number_entries(EntryList));
    #endif

    return_code = destroy_entry_list(EntryList);
    if (return_code != EC_SUCCESS) {
        printf("Error while destroying entry list\n");
        return -1;
    }

    #ifdef _DEBUG_
    printf("---Entry list destroyed\n");
    #endif

    fclose(fp);
    if (word) {
        free(word);
    }

    printf("---Program terminated successfully\n");

    return 0;
}