#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "useful_functions.h"
#include "entry_list.h"
#include "core.h"
#include "ADTMap.h"

// #define _DEBUG_ 1


// hash function for pthread_t
uint hash_pthread_t(Pointer value) {
    return *(pthread_t*)value%MAX_INT;
}


struct entry
{
    word word;

    Map dist;                       // map that holds for a matched thread the distance between a word and this word
    Set matched_threads;            // a set that keeps the thread ids that have made a match with this word
    
    pthread_mutex_t mtx_map;        //mutex to block threads that try to access the map of the entry simultaneously
    pthread_mutex_t mtx_set;        //mutex to block threads that try to access the set of the entry simultaneously
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
    newEntry->dist = map_create(compare_pthread_t, destroy_pthread_t, (DestroyFunc)free);
    map_set_hash_function(newEntry->dist, hash_pthread_t);
    newEntry->matched_threads = set_create(compare_pthread_t, destroy_pthread_t);

    //initialize mutexes
    pthread_mutex_init(&(newEntry->mtx_map),0);
    pthread_mutex_init(&(newEntry->mtx_set),0);

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
    set_destroy(e->matched_threads);
    map_destroy(e->dist);
    pthread_mutex_destroy(&(e->mtx_map));
    pthread_mutex_destroy(&(e->mtx_set));
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

//////////////////// Extra functions
entry* entry_list_node_value(const entry_list_node* node) {
    return node->value;
}

void entry_add_thread(entry* entr, pthread_t id) {
    if (entr == NULL) {
        fprintf(stderr, "Fail in entry_add_thread\n");
        exit(EXIT_FAILURE);
    }
    pthread_mutex_lock(&(entr->mtx_set));
    set_insert(entr->matched_threads, create_pthread_t(id));
    pthread_mutex_unlock(&(entr->mtx_set));
}

void entry_remove_thread(entry* entr, pthread_t id) {
    if (entr == NULL) {
        fprintf(stderr, "Fail in entry_remove_thread\n");
        exit(EXIT_FAILURE);
    }
    pthread_mutex_lock(&(entr->mtx_set));
    set_remove(entr->matched_threads, &id);
    pthread_mutex_unlock(&(entr->mtx_set));
}


///////////////// set

void set_entry_dist(entry* entr, pthread_t target_thread, int dist) {
    if (entr == NULL) {
        fprintf(stderr, "Fail in set_entry_matchdist\n");
        exit(EXIT_FAILURE);
    }
    pthread_mutex_lock(&(entr->mtx_map));
    map_insert(entr->dist, create_pthread_t(target_thread), create_int(dist));
    pthread_mutex_unlock(&(entr->mtx_map));
}

////////////////// get

word get_entry_word(entry* e) {
    if (e == NULL)
        return NULL;
        
    return e->word;
}

int get_entry_dist(entry* entr, pthread_t target_thread) {
    if (entr == NULL) {
        fprintf(stderr, "Fail in set_entry_matchdist\n");
        exit(EXIT_FAILURE);
    }
    pthread_mutex_lock(&(entr->mtx_map));
    int* dist = map_find(entr->dist, &target_thread);
    pthread_mutex_unlock(&(entr->mtx_map));
    if (dist == NULL) {
        fprintf(stderr, "Fail in get_entry_dist\n");
        exit(EXIT_FAILURE);
    }

    return *dist;
}

pthread_t* get_entry_pthread_t(entry* entr, pthread_t id) {
    pthread_mutex_lock(&(entr->mtx_set));
    pthread_t *ret_val = (pthread_t*)set_find(entr->matched_threads, &id);
    pthread_mutex_unlock(&(entr->mtx_set));
    return ret_val;
}