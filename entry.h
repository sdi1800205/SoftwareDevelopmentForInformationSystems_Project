// Virtual entries: Beginning Of File, EOF: End Of File
#define LIST_BOF (struct entry*)0
#define LIST_EOF (struct entry*)0

struct entry
{
    struct entry* next;

    char* word;
    void* payload;    
};

struct entry_list
{
    struct entry* dummy;
    
    int size;
};