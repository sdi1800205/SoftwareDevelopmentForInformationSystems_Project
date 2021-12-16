#include <stdio.h>
#include <stdlib.h>
#include <string.h>

unsigned int hamming_distance(char *a, int length_a, char *b, int length_b) {
	char* a_char = a;
	char* b_char = b;
	int diff = 0;	// number of differences

    // Exit if lenght of words a,b are different
    if (length_a != length_b) {
        printf("Error in hamming distance, different word lengths compared\n");
        exit(EXIT_FAILURE);
    }

    // check all the characters until one or both words end
    while (*a_char != '\0' && *b_char !='\0') {
        if (*a_char != *b_char)	// if the characters are different, add one more to the difference number value
            diff++;

        a_char++;
        b_char++;
    }

    return diff;
}