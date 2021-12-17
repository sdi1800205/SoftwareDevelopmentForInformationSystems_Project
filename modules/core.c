#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core.h"
#include "useful_functions.h"
#include "distances.h"
#include "ADTMap.h"
#include "ADTSet.h"
#include "entry_list.h"
#include "BK_tree.h"
#include "hamming.h"


//////////// extra functions ////////////////

// compares 2 integers (just like the declaration of CompareFunc says in common_types.h)
int compare_queries(Pointer a, Pointer b) {
    return ((Query*)a)->query_id - ((Query*)b)->query_id);
}

int compare_documents(Pointer a, Pointer b) {
    return ((Document*)a)->doc_id - ((Document*)b)->doc_id;
}

// destroys a query
ErrorCode destroy_query(Pointer value) {
	ErrorCode err = destroy_entry_list(((Query*)value)->entrylist);
	free(value);
	return err;
}

// destroys o document
ErrorCode destroy_document(Pointer value) {
	free(((Document*)value)->query_ids);
	free(value);
	return EC_SUCCESS;
}

///////////////////////

// Keeps all information related to an active query
typedef struct Query
{
	QueryID query_id;
	// char str[MAX_QUERY_LENGTH];
	entry_list* entrylist;			// keep the words of the query into entries
	MatchType match_type;
	unsigned int match_dist;
}Query;

// Keeps all query ID results associated with a dcoument
typedef struct Document
{
	DocID doc_id;
	unsigned int num_res;
	QueryID* query_ids;
}Document;

// Keeps all currently active queries
Set queries = set_create(compare_queries, destroy_query);

// Keeps all currently available results that has not been returned yet
Set docs = set_create(compare_documents, destroy_document);


//////////// structs for matchtypes ////////////
Map exact_dist;
Index* edit_dist;
hamIndex* ham_dist;

//////////// core.h functions ///////////////

ErrorCode InitializeIndex() {
	// create dictionary for exact distance
	// we use the same entry as a key and value in the dictionary, so only one of them should be deleted when its time
	exact_dist = map_create(exact_distance_entry, (DestroyFunc)destroy_entry, NULL);
	map_set_hash_function(exact_dist, hash_string);

	// create BK_tree for edit distance
	create_entry_index(&edit_dist, MT_EDIT_DIST);

	// create hamming tree for hamming distance
	ham_dist = create_hamming_index();
}

ErrorCode DestroyIndex() {
	map_destroy(exact_dist);
	destroy_entry_index(edit_dist);
	destroy_hamming_index(ham_dist);
}

ErrorCode StartQuery (QueryID query_id, const char * query_str, MatchType match_type, unsigned int match_dist ) {
	Query* query = malloc(sizeof(*query));	// allocate memory for the new query
	query->query_id = query_id;
	query->match_type = match_type;
	query->match_dist = match_dist;
	// strcpy(query->str, query_str);

	create_entry_list(&(query->entrylist), NULL);		// we don't want the entry_list to destroy the entries because the struct will do that


	// add query in the set of queries
	set_insert(queries, query);

	// add query's words into the correct struct

	char* token = strtok(query_str, " ");
	entry* entr;
	// check in which struct the query should be inserted
	switch (match_type)	{
	case MT_EXACT_MATCH:
		while (token != NULL) {
			create_entry((word)token, &entr);					// create a new entry for current word

			entry* old_entry = map_find(exact_dist, entr);		// check if the entry's word already exists in the map
			if (old_entry != NULL) {									// case in which the entry's word already exists in the map
				Set old_entry_set = get_entry_payload(old_entry);		// get old entry's payload
				set_insert(old_entry_set, create_int(query_id));		// append in payload the new query_id
				destroy_entry(entr);									// destroy new entry
				entr = old_entry;										// keep old entry in current entries value so the query can have access to it, since the new entry destroyed
			}
			else {												// case of entry's word first appearance
				insert_entry_payload(entr, create_int(query_id));		// append in entry's payload the query_id from which it came
				map_insert(exact_dist, entr, entr);						// we pass as key and value the same value, so the set_find returns the entry we search as a key
			}
			add_entry(query->entrylist, entr);						// we add entry in current query's entry_list

			token = strtok(NULL, " ");			// take the next word
		}
		break;
	case MT_EDIT_DIST:
		while (token != NULL) {
			create_entry((word)token, &entr);						// create entry
			insert_entry_payload(entr, create_int(query_id));		// append query_id in payload
			entry* inserted = insert_entry_index(edit_dist, entr);					// insert entry in tree
			if (inserted == NULL) {				// the insertion or updating went wrong
				fprintf(stderr, "Fail in StartQuery->MT_EDIT_DIST->insert_entry_index\n");
				exit(EXIT_FAILURE);
			}

			add_entry(query->entrylist, inserted);			// we add entry that has been inserted or updated in current query's entry_list

			token = strtok(NULL, " ");			// take the next word
		}
		break;		
	case MT_HAMMING_DIST:
		while (token != NULL) {
			// same as in MT_EDIT_DIST

			create_entry((word)token, &entr);
			insert_entry_payload(entr, create_int(query_id));
			entry* inserted = insert_hamming_index(ham_dist, entr);
			if (inserted == NULL) {				// the insertion or updating went wrong
				fprintf(stderr, "Fail in StartQuery->MT_HAMMING_DIST->insert_entry_index\n");
				exit(EXIT_FAILURE);
			}

			add_entry(query->entrylist, inserted);			// we add entry that has been inserted or updated in current query's entry_list

			token = strtok(NULL, " ");			// take the next word
		}
		break;		
	default:
		fprintf(stderr, "Wrong match_type\n");
		exit(EXIT_FAILURE);
		break;
	}

	return EC_SUCCESS;
}

ErrorCode EndQuery(QueryID query_id) {
	Query* query = set_find(queries, &query_id);		// get query from the set of queries
	if (query == NULL)
		return EC_FAIL;

	bool result = true;

	// for every entry of this query delete this query's id from their payload, in any struct they exist
	for (entry_list_node* lnode = get_first(query->entrylist); node != LIST_EOF; lnode = get_next(query->entrylist, lnode)) {
		entry* value = entry_list_node_value(lnode);			// get entry from list_node
		Set payload = get_entry_payload(value);					// get entry's payload
		result = set_remove(payload, &query_id);				// remove from payload the query_id
		if (!result)
			return EC_FAIL;
	}
	
	// search query's words into the correct struct to delete them, or just delete the 
	result = set_remove(queries, &query_id);
	if (!result)
		return EC_FAIL;

	return EC_SUCCESS;
}

ErrorCode MatchDocument (DocID doc_id, const char * doc_str) {

}

ErrorCode GetNextAvailRes (DocID * p_doc_id, unsigned int * p_num_res, QueryID ** p_query_ids) {

}