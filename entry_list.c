#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "entry_list.h"
#include "core.h"

ErrorCode create_entry(const char* w, entry* e) {
    if (w == NULL){
        return EC_NO_AVAIL_RES;
    }

    // allocate memory for the new entry
    entry newEntry = malloc(sizeof(entry));
    if (newEntry == NULL) {
        return EC_NO_AVAIL_RES;
    }

    //allocate memory for the word
    newEntry->word = malloc((strlen(w))*sizeof(*w));
    if (newEntry->word == NULL){
        return EC_NO_AVAIL_RES;
    }

    // copy the given word into newEntry->word
    strcpy(newEntry->word, w);

    newEntry->payload = NULL;
    newEntry->next = NULL;

    *e = newEntry;

    return EC_SUCCESS;
}