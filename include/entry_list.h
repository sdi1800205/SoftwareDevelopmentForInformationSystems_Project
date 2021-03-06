#ifndef __ENTRY_H_
#define __ENTRY_H_

#include "common_types.h"
#include "core.h"
#include "ADTSet.h"

// Virtual entries: Beginning Of File, EOF: End Of File
#define LIST_BOF (entry_list_node*)0
#define LIST_EOF (entry_list_node*)0

typedef struct entry entry;
typedef struct entry_list entry_list;
typedef struct entry_list_node entry_list_node;
typedef char* word;

// entry
ErrorCode create_entry(const word w, entry** e);
ErrorCode destroy_entry(entry* e);

// entry_list
ErrorCode create_entry_list(entry_list** el, DestroyFunc destroy);  // we use a destroy function for the entries of the list
unsigned int get_number_entries(const entry_list* el);
ErrorCode add_entry(entry_list* el, const entry* e);
entry_list_node* get_first(const entry_list* el);
entry_list_node* get_next(const entry_list* el, const entry_list_node*);
ErrorCode destroy_entry_list(entry_list* el);

// Extra functions
entry* entry_list_node_value(const entry_list_node*);
void entry_add_thread(entry*, pthread_t);
void entry_remove_thread(entry*, pthread_t);

// set
void set_entry_dist(entry*, pthread_t, int);

// get
word get_entry_word(entry* e);
int get_entry_dist(entry*, pthread_t);
pthread_t* get_entry_pthread_t(entry*, pthread_t);


#endif //__ENTRY_H_