#ifndef __ENTRY_H_
#define __ENTRY_H_

#include "core.h"
// Virtual entries: Beginning Of File, EOF: End Of File
#define LIST_BOF (entry*)0
#define LIST_EOF (entry*)0

typedef struct entry_list entry_list;
typedef struct entry entry;
typedef char* word;

struct entry
{
    entry* next;

    word word;
    void* payload;    
};

struct entry_list
{
    entry* dummy;
    entry* last;
    
    int size;
};

enum error_code create_entry(const word* w, entry* e);
enum error_code destroy_entry(entry *e);
enum error_code create_entry_list(entry_list* el);
unsigned int get_number_entries(const entry_list* el);
enum error_code add_entry(entry_list* el, const entry* e);
entry* get_first(const entry_list* el);
entry* get_next(const entry_list* el, const entry* e);


#endif //__ENTRY_H_