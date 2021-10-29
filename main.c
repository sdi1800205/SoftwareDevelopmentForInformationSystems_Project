#include <stdio.h>
#include "entry_list.h"
#include "core.h"

int main(void) {
    printf("In main...\n");
    entry new;
    char str[] = "Greece";

    if(create_entry(str, &new) == EC_NO_AVAIL_RES)
        printf("Error\n");

    printf("%s\n", new->word);

    return 0;
}