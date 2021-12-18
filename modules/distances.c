#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "distances.h"


//////////// functions for entries /////////////////
int exact_distance_entry(Pointer a, Pointer b) {
    word a_word = get_entry_word((entry*)a);
    word b_word = get_entry_word((entry*)b);
    return exact_distance(a_word, strlen(a_word), b_word, strlen(b_word));
}

int edit_distance_entry(entry* a, entry* b) {
    word a_word = get_entry_word(a);
    word b_word = get_entry_word(b);
    return edit_distance(a_word, strlen(a_word), b_word, strlen(b_word));
}
int hamming_distance_entry(entry* a, entry* b) {
    word a_word = get_entry_word(a);
    word b_word = get_entry_word(b);
    return hamming_distance(a_word, strlen(a_word), b_word, strlen(b_word));
}

//////////// functions for char*/word ////////////////////

int exact_distance(word a, int length_a, word b, int length_b) {
    int i = -1, j = -1, diff = 0;
    while(a[++i] != '\0' && b[++j] != '\0') {
        if (a[i] != b[j])
            diff++;
    }
    if (length_a > length_b)            // using this if because abs is slower
        diff += length_a - length_b;
    else
        diff += length_b - length_a;

    return diff;
}

int edit_distance(word a,int na,word b,int nb) {
    int oo=0x7FFFFFFF;

	static int T[2][MAX_WORD_LENGTH+1];

	int ia, ib;

	int cur=0;
	ia=0;

	for(ib=0;ib<=nb;ib++)
		T[cur][ib]=ib;

	cur=1-cur;

	for(ia=1;ia<=na;ia++)
	{

		T[cur][0]=ia;


		for(ib=1;ib<=nb;ib++)
		{
			int ret=oo;

			int d1=T[1-cur][ib]+1;
			int d2=T[cur][ib-1]+1;
			int d3=T[1-cur][ib-1]; if(a[ia-1]!=b[ib-1]) d3++;

			if(d1<ret) ret=d1;
			if(d2<ret) ret=d2;
			if(d3<ret) ret=d3;

			T[cur][ib]=ret;
		}

		cur=1-cur;
	}

	int ret=T[1-cur][nb];

	return ret;
}

int hamming_distance(word a, int length_a, word b, int length_b) {
	int diff = 0;	// number of differences

    // Exit if lenght of words a,b are different
    if (length_a != length_b) {
        printf("Error in hamming distance, different word lengths compared\n");
        exit(EXIT_FAILURE);
    }

    // check all the characters until one or both words end
    for (int i = 0; i < length_a; i++) {
        if (a[i] != b[i])	// if the characters are different, add one more to the difference number value
            diff++;
    }

    return diff;
}