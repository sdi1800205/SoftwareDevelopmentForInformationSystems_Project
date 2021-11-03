#include "acutest.h"			// Απλή βιβλιοθήκη για unit testing
#include <stdio.h>

#include "interface.h"
#include "BK_tree.h"

static int find_distance_entries_HAM(entry* a, entry* b) {
	char a_char = a->word;
	char b_char = b->word;
	int diff = 0;	// number of differences

	// check all the characters until one or both words end
	while (a_char != '\0' && b_char !='\0') {
		if (a_char != b_char)	// if the characters are different, add one more to the difference number value
			diff++;
		a_char++;
		b_char++;
	}
	// if word a ends first, add the number of the rest characters of word b in total differences
	if (a_char == '\0' && b_char != '\0') {
		while (b_char != '\0') {
			diff++;
			b_char++;
		}
	}
	// if word b ends first, add the number of the rest characters of word a in total differences
	else if (b_char == '\0' && a_char != '\0') {
		while (a_char != '\0') {
			diff++;
			a_char++;
		}
	}
	return diff;
}

void test_build_entry_index() {
    entry* entr1, * entr2;
    create_entry('add', entr1);
    create_entry('ads', entr2);
    TEST_ASSERT(find_distance_entries_HAM(entr1, entr2) == 1);
}

// Λίστα με όλα τα tests προς εκτέλεση
TEST_LIST = {
	{ "test_build_entry_index", test_build_entry_index },

	{ NULL, NULL } // τερματίζουμε τη λίστα με NULL
};