#ifndef __ENTRY_H_
#define __ENTRY_H_

#include "core.h"
// Virtual entries: Beginning Of File, EOF: End Of File
#define LIST_BOF (entry)0
#define LIST_EOF (entry)0

typedef struct entry_list entry_list;
typedef struct entry entry;
typedef char* word;

ErrorCode create_entry(const word w, entry** e);
ErrorCode destroy_entry(entry* e);
ErrorCode create_entry_list(entry_list** el);
unsigned int get_number_entries(const entry_list* el);
ErrorCode add_entry(entry_list* el, const entry* e);
entry* get_first(const entry_list* el);
entry* get_next(const entry_list* el, const entry* e);
ErrorCode destroy_entry_list(entry_list* el);

// Extra functions
word get_entry_word(entry* e);

#endif //__ENTRY_H_