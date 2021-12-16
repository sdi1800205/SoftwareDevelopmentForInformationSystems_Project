#include "core.h"

#include "ADTMap.h"
#include "ADTSet.h"
#include "entry_list.h"
#include "BK_tree.h"


//////////// extra functions ////////////////
int compare_ints(Pointer a, Pointer b) {
    return *(int*)a - *(int*)b;
}

int compare_entries_exact(Pointer a, Pointer b) {
	// exact_matc
}


// Keeps all information related to an active query
struct Query
{
	QueryID query_id;
	char str[MAX_QUERY_LENGTH];
	MatchType match_type;
	unsigned int match_dist;
};

// Keeps all query ID results associated with a dcoument
struct Document
{
	DocID doc_id;
	unsigned int num_res;
	QueryID* query_ids;
};

// Keeps all currently active queries
Set queries = set_create(compare_ints, free);

// Keeps all currently available results that has not been returned yet
Set docs = set_create(compare_ints, free);


//////////// structs for matchtypes ////////////
Map exact_dist;
Index* edit_dist;
// hamIndex* ham_dist;

//////////// core.h functions ///////////////

ErrorCode InitializeIndex() {
	exact_dist = map_create(compare_entries_exact, (DestroyFunc)destroy_entry, NULL);		// we only use key of dictionary, not the value
	create_entry_index(&edit_dist, MT_EDIT_DIST);
	// create ham index
}

ErrorCode DestroyIndex() {
	map_destroy(exact_dist);
	destroy_entry_index(edit_dist);
	// destroy ham index
}

ErrorCode StartQuery (QueryID query_id, const char * query_str, MatchType match_type, unsigned int match_dist ) {

}

ErrorCode EndQuery(QueryID query_id) {

}

ErrorCode MatchDocument (DocID doc_id, const char * doc_str) {

}

ErrorCode GetNextAvailRes (DocID * p_doc_id, unsigned int * p_num_res, QueryID ** p_query_ids) {

}