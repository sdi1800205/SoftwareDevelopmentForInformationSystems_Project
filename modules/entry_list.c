#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "interface.h"
#include "core.h"

// #define _DEBUG_ 1

struct entry
{
    entry* next;

    word word;
    void* payload;  
};

struct entry_list
{
    entry* dummy;    // virtual entry that points to the first entry
    entry* last;
    
    int size;
};

ErrorCode create_entry(const word w, entry** e) {
    if (w == NULL) {
        printf("Given word is empty\n");
        return EC_FAIL;
    }

    // allocate memory for the new entry
    entry* newEntry = malloc(sizeof(entry));
    if (newEntry == NULL) {
        return EC_NO_AVAIL_RES;
    }

    //allocate memory for the word
    newEntry->word = malloc((strlen(w)+1)*sizeof(char));
    if (newEntry->word == NULL){
        printf("Error while allocating memory\n");
        return EC_NO_AVAIL_RES;
    }

    // copy the given word into newEntry->word
    // strcpy(newEntry->word, w);
    strncpy(newEntry->word, w, strlen(w));
	newEntry->word[strlen(w)]='\0';

    // set everything else to null
    newEntry->payload = NULL;
    newEntry->next = NULL;

    // assign the created entry to the pointer so as to be return (pass by reference)
    *e = newEntry;

    return EC_SUCCESS;
}

ErrorCode destroy_entry(entry *e) {
    if (e == NULL) {
        printf("Given entry is already null");
        return EC_FAIL;
    }

    free(e->word);
    // free(e->payload);
    free(e);

    return EC_SUCCESS;
}

ErrorCode create_entry_list(entry_list** el) {
    // Allocate entry_list memory
    entry_list* newEntryList = malloc(sizeof(entry_list));
    if(newEntryList == NULL) {
        printf("Error while allocating entry list memory\n");
        return EC_NO_AVAIL_RES;
    }

    // Create dummy entry (virtual entry that will point to the first entry)
    create_entry("dummy", &(newEntryList->dummy));

    // Initialize entry_list members
    newEntryList->last = newEntryList->dummy;
    newEntryList->size = 0;

    *el = newEntryList;

    return EC_SUCCESS;
}

unsigned int get_number_entries(const entry_list* el) {
    if (el == NULL) {
        printf("Entry is list not initialized\n");
        return 0;
    }

    return el->size;
}

ErrorCode add_entry(entry_list* el, const entry* e) {
    if (el == NULL || e == NULL) {
        printf("Given parameters are null\n");
        return EC_FAIL;
    }

    // set the last entry of entry list as this node
    el->last->next = (entry*)e;
    el->last = (entry*)e;
    el->size++;

    return EC_SUCCESS;
}

entry* get_first(const entry_list* el) {
    if (el == NULL) {
        printf("Entry list is not initialized\n");
        return NULL;
    }

    if (el->size == 0) {
        printf("Entry list is empty\n");
        return NULL;
    }
    return el->dummy->next;
}

entry* get_next(const entry_list* el, const entry* e) {
    if (el == NULL || e == NULL) {
        printf("Given arguments must not be null\n");
        return NULL;
    }

    return e->next;
}

ErrorCode destroy_entry_list(entry_list* el) {
    if (el == NULL) {
        printf("Entry list is not initialized\n");
        return EC_FAIL;
    }

    entry* next;
    entry* temp = get_first(el);
    if (temp == NULL) {
        printf("Error getting first entry\n");
        return EC_FAIL;
    }

    ErrorCode return_code;

    while (temp != NULL) {
        #ifdef _DEBUG_
        printf("---Trying to destroy entry with word:%s\n",temp->word);
        #endif
        next = get_next(el, temp);
        return_code = destroy_entry(temp);
        if (return_code != EC_SUCCESS) {
            return EC_FAIL;
        }
        #ifdef _DEBUG_
        printf("---Entry destroyed\n");
        #endif

        el->size--;
        #ifdef _DEBUG_
        printf("---Current number of entries in entry list:%d\n\n", el->size);
        #endif
        temp = next;
    }

    destroy_entry(el->dummy);
    free(el);

    return EC_SUCCESS;
}

// Extra functions
word get_entry_word(entry* e) {
    if (e == NULL)
        return NULL;
        
    return e->word;
}