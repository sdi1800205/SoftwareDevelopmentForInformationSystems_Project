#include <stdio.h>
#include <stdlib.h>
#include "interface.h"
#include "core.h"

int main(void) {
    FILE* fp;
    char* word = NULL;
    size_t len = 0;
    ssize_t read;

    ErrorCode return_code;

    printf("In main..\n");

    // Open test file
    fp = fopen("./tests/test.txt", "r");
    if (fp == NULL) {
        return -1;
    }

    // Create entry list
    entry_list EntryList;
    return_code = create_entry_list(&EntryList);
    if (return_code != EC_SUCCESS) {
        printf("Error while creating entry list\n");
        return -1;
    }

    // Read line by line the file. Each line has a word
    while ((read = getline(&word, &len, fp)) != -1) {
        printf("Retrieved line of length %zu:\n", read);

        // Create new entry with given word
        entry new_entry;
        return_code = create_entry(word, &new_entry);
        if(return_code != EC_SUCCESS){
            printf("Error while creating new entry\n");
            return -1;
        }

        printf("New word passed=%s\n", new_entry->word);

        printf("Trying to add new entry\n");

        // Add entry to entry_list
        return_code = add_entry(&EntryList, new_entry);
        if (return_code != EC_SUCCESS) {
            printf("Error while adding new entry to entry_list");
            return -1;
        }

        printf("Current number of entries: %d\n", get_number_entries(EntryList));
    }

    printf("Total number of entries: %d\n", get_number_entries(EntryList));

    return_code = destroy_entry_list(&EntryList);
    if (return_code != EC_SUCCESS) {
        printf("Error while destroying entry list\n");
        return -1;
    }

    fclose(fp);
    if (word) {
        free(word);
    }

    printf("Program terminated successfully\n");

    return 0;;
}