#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "entry_list.h"
#include "core.h"

ErrorCode create_entry(const char* w, entry* e) {
    if (w == NULL) {
        printf("Given word is empty\n");
        return EC_FAIL;
    }
    if (*e != NULL) {
        printf("Entry to be created must be null\n");
        return EC_FAIL;
    }

    // allocate memory for the new entry
    entry newEntry = malloc(sizeof(struct entry));
    if (newEntry == NULL) {
        return EC_NO_AVAIL_RES;
    }

    //allocate memory for the word
    newEntry->word = malloc((strlen(w))*sizeof(*w));
    if (newEntry->word == NULL){
        printf("Error while allocating memory\n");
        return EC_NO_AVAIL_RES;
    }

    // copy the given word into newEntry->word
    strcpy(newEntry->word, w);

    // set everything else to null
    newEntry->payload = NULL;
    newEntry->next = NULL;

    // assign the created entry to the pointer so as to be return (pass by reference)
    *e = newEntry;

    return EC_SUCCESS;
}

ErrorCode destroy_entry(entry *e) {
    free(*e);
    if (*e != NULL) {
        printf("Given entry was not successully destroyed\n");
        return EC_FAIL;
    }

    return EC_SUCCESS;
}

ErrorCode create_entry_list(entry_list* el) {
    if (*el != NULL) {
        printf("Entry list to be created must be null\n");
        return EC_FAIL;
    }

    // Allocate entry_list memory
    entry_list newEntryList = malloc(sizeof(struct entry_list));
    if(newEntryList == NULL) {
        printf("Error while allocating entry list memory\n");
        return EC_NO_AVAIL_RES;
    }

    // Initialize entry_list members
    newEntryList->dummy = LIST_BOF;
    newEntryList->last = LIST_EOF;
    newEntryList->size = 0;

    return EC_SUCCESS;
}