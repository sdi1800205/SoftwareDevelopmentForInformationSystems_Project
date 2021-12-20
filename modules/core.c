#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "core.h"
#include "useful_functions.h"
#include "distances.h"
#include "ADTMap.h"
#include "ADTSet.h"
#include "ADTDeque.h"
#include "entry_list.h"
#include "BK_tree.h"
#include "hamming.h"


// Keeps all information related to an active query
typedef struct Query
{
	QueryID query_id;
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


//////////// extra functions ////////////////

// compares 2 integers (just like the declaration of CompareFunc says in common_types.h)
int compare_queries(Pointer a, Pointer b) {
    return ((Query*)a)->query_id - ((Query*)b)->query_id;
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
	free(value);
	return EC_SUCCESS;
}

///////////////////////

// Keeps all currently active queries
Set queries;

// Keeps all currently available results that has not been returned yet
Set docs;


//////////// structs for matchtypes ////////////
// we create structs depend on the match_type and the match_distance
Map exact_dist;				// struct for exact distance
Index* edit_dist;			// struct for edit distance
hamIndex* ham_dist;			// struct for hamming distance

//////////// core.h functions ///////////////

ErrorCode InitializeIndex() {
	// initialize structs for queries and documents
	queries = set_create(compare_queries, destroy_query);
	docs = set_create(compare_documents, destroy_document);

	// create dictionary for exact distance
	// we use the entry->word as a key and entry as a value in the dictionary, so to delete the it only takes to delete entry
	exact_dist = map_create(compare_strings, NULL, (DestroyFunc)destroy_entry);		//(key, value) -> (word, entry), "word" = entry->word
	map_set_hash_function(exact_dist, hash_string);

	// create BK_tree for edit distance
	create_entry_index(&edit_dist, MT_EDIT_DIST, (DestroyFunc)destroy_entry);

	// create hamming tree for hamming distance
	ham_dist = create_hamming_index((DestroyFunc)destroy_entry);

	return EC_SUCCESS;
}

ErrorCode DestroyIndex() {
	map_destroy(exact_dist);
	destroy_entry_index(edit_dist);
	destroy_hamming_index(ham_dist);

	set_destroy(queries);
	set_destroy(docs);

	return EC_SUCCESS;
}

ErrorCode StartQuery (QueryID query_id, const char * query_str, MatchType match_type, unsigned int match_dist ) {
	Query* query = malloc(sizeof(*query));	// allocate memory for the new query
	query->query_id = query_id;
	query->match_type = match_type;
	query->match_dist = match_dist;
	create_entry_list(&(query->entrylist), NULL);		// we don't want the entry_list to destroy the entries because the structs will do that

	// add query in the set of queries
	set_insert(queries, query);

	// add query's words into the correct struct

	char* token = strtok((word)query_str, " ");
	entry* entr;
	// check in which struct the query should be inserted
	switch (match_type)	{
	case MT_EXACT_MATCH:
		while (token != NULL) {
			entry* old_entry = map_find(exact_dist, token);		// check if the entry's word already exists in the map

			if (old_entry != NULL)										// case in which the entry's word already exists in the map
				entr = old_entry;										// keep old entry in current entries value so the query can have access to it, since the new entry destroyed
			else {														// case of entry's word first appearance
				create_entry(token, &entr);								// create a new entry for current word
				map_insert(exact_dist, get_entry_word(entr), entr);						// we pass as key and value the same value, so the set_find returns the entry we search as a key
			}
			add_entry(query->entrylist, entr);						// we add entry in current query's entry_list

			token = strtok(NULL, " ");			// take the next word
		}
		break;
	case MT_EDIT_DIST:
		while (token != NULL) {
			create_entry(token, &entr);						// create entry
			entry* inserted = insert_entry_index(edit_dist, entr);		// insert entry in tree with correct match_dist
			if (inserted == NULL) {				// the insertion or updating went wrong
				fprintf(stderr, "Fail in StartQuery->MT_EDIT_DIST->insert_entry_index\n");
				exit(EXIT_FAILURE);
			}
			
			// check if entry was inserted or an old one just update its payload
			if (inserted != entr)			// another entry, equal to new, already existed("inserted" variable holds the entry that remains in the tree or the new one that inserted)
				destroy_entry(entr);		// destroy new entry because it didn't inserted

			add_entry(query->entrylist, inserted);			// we add entry that has been inserted or updated in current query's entry_list

			token = strtok(NULL, " ");			// take the next word
		}
		break;		
	case MT_HAMMING_DIST:
		while (token != NULL) {
			// same as in MT_EDIT_DIST

			create_entry(token, &entr);
			entry* inserted = insert_hamming_index(ham_dist, entr);			// insert entry in tree with correct match_dist
			if (inserted == NULL) {				// the insertion or updating went wrong
				fprintf(stderr, "Fail in StartQuery->MT_HAMMING_DIST->insert_entry_index\n");
				exit(EXIT_FAILURE);
			}

			// check if entry was inserted or an old one just update its payload
			if (inserted != entr)			// another entry, equal to new, already existed("inserted" variable holds the entry that remains in the tree or the new one that inserted)
				destroy_entry(entr);		// destroy new entry because it didn't inserted

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
	Query target_query = {query_id, NULL, -1, -1};
	
	// remove query from set of queries
	if (!(set_remove(queries, &target_query))) {
		printf("EndQuery: fail in set_remove\n");
		return EC_FAIL;
	}

	return EC_SUCCESS;
}

ErrorCode MatchDocument (DocID doc_id, const char * doc_str) {
	// initialize result list
	entry_list* result_list;
	create_entry_list(&result_list, NULL);			// we dont want result list to destroy entries, structs do that

	// initialize document
	Document* doc = malloc(sizeof(*doc));
	doc->doc_id = doc_id;
	doc->num_res = 0;

	// we create a map for doc's words, so that we won't check the same word > 1 times
	Map doc_words = map_create(compare_strings, (DestroyFunc)free, NULL);		// (key, value) -> (word, word), so we delete only key or value
	map_set_hash_function(doc_words, hash_string);

	word token = strtok((word)doc_str, " ");

	while (token != NULL) {
		// deduplication of doc's words
		if ((map_find(doc_words, token)) != NULL) {	// if the word exists in the set means that it has already been checked
			token = strtok(NULL, " ");		// take new word
			continue;
		}
		else {										// case of not found the word in the set
			char* new_word = malloc((strlen(token)+1) * sizeof(char));
			strcpy(new_word, token);
			map_insert(doc_words, new_word, new_word);				// put the new word in the set
		}

		// start matching
		
		// lookup the hash table
		entry* res_entry = map_find(exact_dist, token);		// take the one entry(if it exists) that the hash table will return since it has to be the same word
		if (res_entry != NULL) {
			set_entry_dist(res_entry, 0);							// set dist = 0 because it is from exact
			set_entry_matched(res_entry, true);						// show it matched
			add_entry(result_list, res_entry);						// append it in the result list
		}

		// lookup trees with the higher match_dist value so it considers every match_dist from 1 to 3

		// lookup the BK_tree of edit distance
		if ((lookup_entry_index(token, edit_dist, 3, &result_list)) != EC_SUCCESS)
			return EC_FAIL;

		// lookup the Hamming BK_tree of hamming distance
		if ((lookup_hamming_index(token, ham_dist, 3, &result_list)) != EC_SUCCESS)
			return EC_FAIL;
		
		token = strtok(NULL, " ");
	}
	map_destroy(doc_words);		// destroy the map of document's words

	Deque match_queries = deque_create(0, NULL);		// create a deque to store temporary the matching queries

	// check the queries to find the matching ones
	for (SetNode snode = set_first(queries); snode != SET_EOF; snode = set_next(queries, snode)) {
		Query* query = set_node_value(queries, snode);
		bool matched = true;		// if all the query's words are true then this variable will remain true, else will turn to false

		// check every entry
		for (entry_list_node* lnode = get_first(query->entrylist); lnode != LIST_EOF; lnode = get_next(query->entrylist, lnode)) {
			entry* entr = entry_list_node_value(lnode);
			if (!(get_entry_matched(entr) && (get_entry_dist(entr) <= query->match_dist))) {		// check if the entry for this query is valid
				matched = false;
				break;
			}
		}

		// if query has been matched then add its id to the result queries
		if (matched)
			deque_insert_last(match_queries, &(query->query_id));
	}
	doc->num_res = deque_size(match_queries);				// store number of results

	// pass all the result queries in the documents array
	if (doc->num_res > 0) {										// there are result queries
		doc->query_ids = malloc(doc->num_res * sizeof(int));		// allocate memory for the array

		// copy every id
		int i = 0;
		for (DequeNode dnode = deque_first(match_queries); dnode != DEQUE_EOF; dnode = deque_next(match_queries, dnode)) {
			int* id = deque_node_value(match_queries, dnode);
			doc->query_ids[i++] = *id;
		}
	}
	else			// no queries matched
		doc->query_ids = NULL;

	// destroy deque
	deque_destroy(match_queries);

	// initialize all the entries' matched values for the next document
	for (entry_list_node* lnode = get_first(result_list); lnode != LIST_EOF; lnode = get_next(result_list, lnode)) {
		entry* value = entry_list_node_value(lnode);
		set_entry_dist(value, MAX_INT);
		set_entry_matched(value, false);					// set matched value back to false, for the next document's word
	}

	// destroy result list
	destroy_entry_list(result_list);

	// add doc in the set of docs
	set_insert(docs, doc);

	return EC_SUCCESS;
}

ErrorCode GetNextAvailRes (DocID * p_doc_id, unsigned int * p_num_res, QueryID ** p_query_ids) {
	// Get the first undeliverd result from "docs" and return it
	
	if(set_size(docs) == 0) return EC_NO_AVAIL_RES;
	
	Document* doc = set_get_at(docs, 0);		// get first document
	*p_doc_id = doc->doc_id;
	*p_num_res = doc->num_res;
	*p_query_ids = doc->query_ids;

	if (!(set_remove(docs, doc)))
		return EC_FAIL;

	return EC_SUCCESS;
}