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
#include "job_scheduler.h"
#include "job.h"


#define THREADS_NUM 2


// declaration of multi-thread functions
ErrorCode MatchDocument_mt(Pointer);

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
Deque docs;
pthread_mutex_t mtx_docs;
pthread_mutex_t pousths;

//////////// structs for matchtypes ////////////
// we create structs depend on the match_type and the match_distance
Map exact_dist;				// struct for exact distance

Index* edit_dist;			// struct for edit distance
hamIndex* ham_dist;			// struct for hamming distance


/////////// Job Scheduler /////////////////////
JobScheduler* JobSch; 		// Job Scheduler for multithreading

//////////// core.h functions ///////////////

ErrorCode InitializeIndex() {
	// initialize structs for queries and documents
	queries = set_create(compare_queries, destroy_query);
	docs = deque_create(0, destroy_document);

	// create dictionary for exact distance
	// we use the entry->word as a key and entry as a value in the dictionary, so to delete the it only takes to delete entry
	exact_dist = map_create(compare_strings, NULL, (DestroyFunc)destroy_entry);		//(key, value) -> (word, entry), "word" = entry->word
	map_set_hash_function(exact_dist, hash_string);

	// create BK_tree for edit distance
	create_entry_index(&edit_dist, MT_EDIT_DIST, (DestroyFunc)destroy_entry);

	// create hamming tree for hamming distance
	ham_dist = create_hamming_index((DestroyFunc)destroy_entry);

	// initialize mutexes for docs and exact_dist
	pthread_mutex_init(&mtx_docs,0);
	pthread_cond_init(&pousths,0);

	// create JobScheduler
	JobSch = initialize_scheduler(THREADS_NUM);

	return EC_SUCCESS;
}

ErrorCode DestroyIndex() {
	map_destroy(exact_dist);
	destroy_entry_index(edit_dist);
	destroy_hamming_index(ham_dist);

	set_destroy(queries);
	deque_destroy(docs);

	if (destroy_scheduler(JobSch) < 0)
		return EC_FAIL;

	pthread_mutex_destroy(&mtx_docs);

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

ErrorCode MatchDocument(DocID doc_id, const char * doc_str) {
	// allocate memory for documents string
	char* text = malloc(MAX_DOC_LENGTH * sizeof(char));
	strcpy(text, (char*)doc_str);

	// create a job and push it to the scheduler
	Job* job = job_create(MatchDocument_mt, docargs_create(doc_id, text));
	submit_job(JobSch, job);

	return EC_SUCCESS;
}

ErrorCode GetNextAvailRes(DocID * p_doc_id, unsigned int * p_num_res, QueryID ** p_query_ids) {
	// check if there are jobs to execute, and if they do execute them
	if (jobscheduler_size(JobSch) > 0) {
		if (execute_all_jobs(JobSch) < 0)
			return EC_FAIL;
		if (wait_all_tasks_finish(JobSch) < 0)
			return EC_FAIL;
	}


	// Get the first undeliverd result from "docs" and return it
	
	if(deque_size(docs) == 0) return EC_NO_AVAIL_RES;
	
	Document* doc = deque_get_at(docs, 0);		// get first document
	*p_doc_id = doc->doc_id;
	*p_num_res = doc->num_res;
	*p_query_ids = doc->query_ids;

	deque_remove_first(docs);

	return EC_SUCCESS;
}



///////////////////////////////// multi-threading ////////////////////////////////////

ErrorCode MatchDocument_mt(Pointer arguments) {
    // get the arguments to proceed

    int doc_id = ((DocArgs*)arguments)->id;
	char* doc_str = ((DocArgs*)arguments)->str;
	char *safe_str;
	pthread_t thread_id = pthread_self();			// get threads id

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

	word token = strtok_r((word)doc_str, " ",&safe_str);
	//pthread_mutex_lock(&pousths);
	while (token != NULL) {
		// deduplication of doc's words
		if ((map_find(doc_words, token)) != NULL) {	// if the word exists in the set means that it has already been checked
			token = strtok_r(NULL, " ",&safe_str);		// take new word
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
			entry_add_thread(res_entry, thread_id);					// add thread id to entry's matched threads
			set_entry_dist(res_entry, thread_id, 0);							// set dist of this thread = 0 because it is from exact
			add_entry(result_list, res_entry);						// append it in the result list
		}

		// lookup trees with the higher match_dist value so it considers every match_dist from 1 to 3

		// lookup the BK_tree of edit distance
		if ((lookup_entry_index(token, edit_dist, 3, &result_list, thread_id)) != EC_SUCCESS)
			return EC_FAIL;

		// lookup the Hamming BK_tree of hamming distance
		if ((lookup_hamming_index(token, ham_dist, 3, &result_list, thread_id)) != EC_SUCCESS)
			return EC_FAIL;
		
		token = strtok_r(NULL, " ",&safe_str);
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
			// pthread_t* thread_found  = get_entry_pthread_t(entr, thread_id);
			if (!(get_entry_pthread_t(entr, thread_id) != NULL && (get_entry_dist(entr, thread_id) <= query->match_dist))) {		// check if the entry for this query is valid
				if((doc->doc_id)==1){
					if(get_entry_pthread_t(entr, thread_id) != NULL)
						printf("Thread_id = %lu, target thread_id = %lu, entry_dist = %d, query->match_dist = %d\n",pthread_self(),*(get_entry_pthread_t(entr, thread_id)),get_entry_dist(entr, thread_id),query->match_dist);
				}
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
		entry_remove_thread(value, thread_id);				// remove thread id from entry's matched threads
	}

	// destroy result list
	destroy_entry_list(result_list);

	// add doc in the set of docs
	pthread_mutex_lock(&mtx_docs);
	// printf("2.Thread :%lu doc:%d num_of_queries:%d\n",pthread_self(),doc->doc_id,doc->num_res);
	// for(int i=0; i<doc->num_res; i++){
	// 	printf("Thread :%lu i:%d query:%d\n",pthread_self(),i,doc->query_ids[i]);
	// }

	deque_insert_last(docs, doc);
	pthread_mutex_unlock(&mtx_docs);
	//pthread_mutex_unlock(&pousths);
	return EC_SUCCESS;
}