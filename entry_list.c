#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interface.h"
#include "core.h"

ErrorCode create_entry(const char* w, entry* e) {
    if (w == NULL) {
        printf("Given word is empty\n");
        return EC_FAIL;
    }

    // allocate memory for the new entry
    entry newEntry = malloc(sizeof(struct entry));
    if (newEntry == NULL) {
        return EC_NO_AVAIL_RES;
    }

    //allocate memory for the word
    newEntry->word = malloc((strlen(w)+1)*sizeof(*w));
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
    if (*e == NULL) {
        printf("Given entry is already null");
        return EC_FAIL;
    }

    free((*e)->word);
    free((*e)->payload);
    free((*e));
    if ((*e) != NULL) {
        printf("Given entry was not successully destroyed\n");
        return EC_FAIL;
    }

    return EC_SUCCESS;
}

ErrorCode create_entry_list(entry_list* el) {
    // Allocate entry_list memory
    entry_list newEntryList = malloc(sizeof(struct entry_list));
    if(newEntryList == NULL) {
        printf("Error while allocating entry list memory\n");
        return EC_NO_AVAIL_RES;
    }

    // Create dummy entry (virtual entry that will point to the first entry)
    create_entry("dummy", &(newEntryList->dummy));
    newEntryList->last = newEntryList->dummy;

    // Initialize entry_list members
    // newEntryList->dummy = LIST_BOF;
    newEntryList->last = LIST_EOF;
    // newEntryList->dummy->next = newEntryList->last;
    newEntryList->size = 0;

    *el = newEntryList;

    return EC_SUCCESS;
}

unsigned int get_number_entries(const entry_list el) {
    if (el == NULL) {
        printf("Entry is list not initialized\n");
        return 0;
    }

    return el->size;
}

ErrorCode add_entry(entry_list* el, const entry e) {
    if (*el == NULL || e == NULL) {
        printf("Given parameters are null\n");
        return EC_FAIL;
    }

    // Allocate memory for the new entry
    entry new_entry = malloc(sizeof(struct entry));
    if (new_entry == NULL) {
        printf("Error while allocating memory\n");
        return EC_FAIL;
    }

    // Copy the values of given entry to the new entry

    new_entry->word = malloc(strlen((e->word)+1)*sizeof(char));
    if (new_entry->word == NULL) {
        printf("Error while allocating memory\n");
        return EC_FAIL;
    }
    strcpy(new_entry->word, e->word);
    // new_entry->payload = e->payload;

    // this entry will be added to the end of list, so it doesn't have next entry
    // new_entry->next = LIST_EOF;

    // Check if this is the first entry of entry_list
    if((*el)->size == 0) {
        (*el)->dummy->next = new_entry;
        (*el)->last = new_entry;
    }
    else {
        // set the last entry of entry list as this node
        (*el)->last->next = new_entry;
        (*el)->last = new_entry;
    }
    
    (*el)->size++;

    return EC_SUCCESS;
}

entry get_first(const entry_list* el) {
    if (*el == NULL) {
        printf("Entry list is not initialized\n");
        return NULL;
    }

    if ((*el)->size == 0) {
        printf("Entry list is empty\n");
        return NULL;
    }
    return (*el)->dummy->next;
}

entry get_next(const entry_list* el, const entry e) {
    if (*el == NULL || e == NULL) {
        printf("Given arguments must not be null\n");
        return NULL;
    }

    if ((*el)->size == 0) {
        printf("Entry list is empty\n");
        return (*el)->last;
    }

    entry temp = (*el)->dummy->next;

    int found = 0;
    while (!found && temp->next != LIST_EOF) {
        if (temp == e) {
            found = 1;
            return temp->next;
        }
        else {
            temp = temp->next;
        }
    }

    return NULL;
}

ErrorCode destroy_entry_list(entry_list* el) {
    if (*el == NULL) {
        printf("Entry list is not initialized\n");
        return EC_FAIL;
    }

    entry next;
    entry temp = get_first(el);
    while (temp != LIST_EOF) {
        next = get_next(el, temp);
        destroy_entry(&temp);
        (*el)->size--;
        printf("Current number of entries in entry list:%d\n", (*el)->size);
        temp = next;
    }
    
    free((*el));

    return EC_SUCCESS;
}