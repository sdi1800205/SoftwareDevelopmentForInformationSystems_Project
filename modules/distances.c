#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../include/BK_tree.h"

struct hammingIndex {
    struct BK_tree *BK_trees[28];   // array of pointers to BK_trees. Length = 28 => from 4 to 31
};


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


// ----------- EDIT DISTANCE ----------- //
// Copied from https://www.geeksforgeeks.org/edit-distance-dp-5/

// define min function between two numbers
#define MIN(a,b) ((a) < (b) ? (a) : (b))

// Utility function to find minimum of three numbers
int min(int x, int y, int z) { return MIN(MIN(x, y), z); }
  
// m = length of str1, 
// n = length of str2
int editDist(char *str1, char *str2, int m, int n)
{
    // If first string is empty, the only option is to
    // insert all characters of second string into first
    if (m == 0)
        return n;
  
    // If second string is empty, the only option is to
    // remove all characters of first string
    if (n == 0)
        return m;
  
    // If last characters of two strings are same, nothing
    // much to do. Ignore last characters and get count for
    // remaining strings.
    if (str1[m - 1] == str2[n - 1])
        return editDist(str1, str2, m - 1, n - 1);
  
    // If last characters are not same, consider all three
    // operations on last character of first string,
    // recursively compute minimum cost for all three
    // operations and take minimum of three values.
    return 1
           + min(editDist(str1, str2, m, n - 1), // Insert
                 editDist(str1, str2, m - 1, n), // Remove
                 editDist(str1, str2, m - 1,
                          n - 1) // Replace
             );
}