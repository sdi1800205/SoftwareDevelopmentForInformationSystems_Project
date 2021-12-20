#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "useful_functions.h"
#include "entry_list.h"
#include "core.h"

// #define _DEBUG_ 1

struct entry
{
    word word;
    Set payload;        // we declare payload as a set

    int dist;               // variable that shows the distance between a word and this word
    bool matched;           // variable that shows if this entry has made a match with a word
};

struct entry_list
{
    entry_list_node* dummy;    // virtual entry that points to the first entry
    entry_list_node* last;
    
    int size;
    DestroyFunc destroy;    // function called when an entry is going to be deleted from the list
};

struct entry_list_node {
    entry* value;
    entry_list_node* next;
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

    // initialize values
    strcpy(newEntry->word, w);
    newEntry->dist = MAX_INT;
    newEntry->matched = false;
    newEntry->payload = set_create(compare_ints, NULL);         // in the payload are going to pass the pointers of ids of struct query, so the queries are obliged to destroy the ids

    // assign the created entry to the pointer so as to be return (pass by reference)
    *e = newEntry;

    return EC_SUCCESS;
}

ErrorCode destroy_entry(entry *e) {
    if (e == NULL) {
        printf("Given entry is already null");
        return EC_FAIL;
    }

    // deallocate memory
    free(e->word);
    set_destroy(e->payload);
    free(e);

    return EC_SUCCESS;
}

ErrorCode create_entry_list(entry_list** el, DestroyFunc destroy) {
    // Allocate entry_list memory
    entry_list* newEntryList = malloc(sizeof(entry_list));
    if(newEntryList == NULL) {
        printf("Error while allocating entry list memory\n");
        return EC_NO_AVAIL_RES;
    }

    // Allocate memory for entry_list_node for dummy
    newEntryList->dummy = malloc(sizeof(entry_list_node));
    newEntryList->dummy->value = NULL;
    newEntryList->dummy->next = LIST_EOF;

    // Initialize entry_list members
    newEntryList->last = newEntryList->dummy;
    newEntryList->size = 0;
    newEntryList->destroy = destroy;

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

    // allocate memory for new entry_list_node
    entry_list_node* new_node = malloc(sizeof(entry_list_node));
    new_node->value = (entry*)e;
    new_node->next = LIST_EOF;

    // set the last entry of entry list as this node
    el->last->next = new_node;
    el->last = new_node;
    el->size++;

    return EC_SUCCESS;
}

entry_list_node* get_first(const entry_list* el) {
    if (el == NULL) {
        printf("Entry list is not initialized\n");
        return NULL;
    }
    return el->dummy->next;
}

entry_list_node* get_next(const entry_list* el, const entry_list_node* node) {
    if (el == NULL || node == NULL) {
        printf("Given arguments must not be null\n");
        return NULL;
    }

    return node->next;
}

ErrorCode destroy_entry_list(entry_list* el) {
    if (el == NULL) {
        printf("Entry list is not initialized\n");
        return EC_FAIL;
    }

    entry_list_node* next;
    entry_list_node* temp = get_first(el);
    ErrorCode return_code;

    while (temp != LIST_EOF) {
        #ifdef _DEBUG_
        printf("---Trying to destroy entry with word:%s\n",temp->word);
        #endif
        next = get_next(el, temp);
        if (el->destroy != NULL) {
            entry* value = entry_list_node_value(temp);
            return_code = el->destroy(value);
            if (return_code != EC_SUCCESS) {
                return EC_FAIL;
            }
        }
        free(temp);
            
        #ifdef _DEBUG_
        printf("---Entry destroyed\n");
        #endif

        el->size--;
        #ifdef _DEBUG_
        printf("---Current number of entries in entry list:%d\n\n", el->size);
        #endif
        temp = next;
    }

    free(el->dummy);
    free(el);

    return EC_SUCCESS;
}

// Extra functions
entry* entry_list_node_value(const entry_list_node* node) {
    return node->value;
}

// set

void insert_entry_payload(entry* entr, QueryID* query_id) {
    if (entr == NULL || query_id == NULL) {
        fprintf(stderr, "Fail in insert_entry_payload\n");
        exit(EXIT_FAILURE);
    }
    set_insert(entr->payload, query_id);
}

void set_entry_matched(entry* entr, bool matched) {
    if (entr == NULL) {
        fprintf(stderr, "Fail in set_entry_matched\n");
        exit(EXIT_FAILURE);
    }
    entr->matched = matched;
}

void set_entry_dist(entry* entr, int dist) {
    if (entr == NULL) {
        fprintf(stderr, "Fail in set_entry_matchdist\n");
        exit(EXIT_FAILURE);
    }
    entr->dist = dist;    
}

// get

word get_entry_word(entry* e) {
    if (e == NULL)
        return NULL;
        
    return e->word;
}

Set get_entry_payload(entry* entr) {
    if (entr == NULL) {
        fprintf(stderr, "Fail in get_entry_payload\n");
        exit(EXIT_FAILURE);
    }
    return entr->payload;
}

bool get_entry_matched(entry* entr) {
    if (entr == NULL) {
        fprintf(stderr, "Fail in set_entry_matched\n");
        exit(EXIT_FAILURE);
    }
    return entr->matched;    
}

int get_entry_dist(entry* entr) {
    if (entr == NULL) {
        fprintf(stderr, "Fail in set_entry_matchdist\n");
        exit(EXIT_FAILURE);
    }
    return entr->dist;
}